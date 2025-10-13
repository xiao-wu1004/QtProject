#include "musicsearchdialog.h"
#include <QDebug>
#include <QUrlQuery>
#include <QRegExp>

MusicSearchDialog::MusicSearchDialog(QWidget *parent, QMediaPlayer *mediaPlayer)
    : QDialog(parent),  networkManager(new QNetworkAccessManager(this)),player(mediaPlayer)
{
    lyricTimer = new QTimer(this);
    lyricTimer->setInterval(1000);

    setWindowTitle("音乐搜索");
    setFixedSize(400, 400);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("输入歌曲/歌手名搜索...");
    searchEdit->setMinimumHeight(35);

    searchBtn = new QPushButton("搜索", this);
    searchBtn->setMinimumSize(80, 35);
    connect(searchBtn, &QPushButton::clicked, this, &MusicSearchDialog::onSearchClicked);
    connect(searchEdit, &QLineEdit::returnPressed, this, &MusicSearchDialog::onSearchClicked);

    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchBtn);
    mainLayout->addLayout(searchLayout);

    musicList = new QListWidget(this);
    musicList->setMinimumHeight(180);
    connect(musicList, &QListWidget::itemClicked, this, &MusicSearchDialog::onSongItemClicked);
    mainLayout->addWidget(musicList);

    lyricLabel = new QLabel(this);
    lyricLabel->setMinimumHeight(80);
    lyricLabel->setAlignment(Qt::AlignCenter);
    lyricLabel->setFont(QFont("SimHei", 11));
    lyricLabel->setStyleSheet("color: #333; background-color: #F0F7FF; border-radius: 6px; padding: 5px;");
    mainLayout->addWidget(lyricLabel);

    connect(networkManager, &QNetworkAccessManager::finished,
            this, [this](QNetworkReply *reply)
            {
                if (reply->url().toString().contains("api/search/get"))
                {
                    onSearchReplyFinished(reply);
                } else if (reply->url().toString().contains("api/song/lyric")) {
                    onLyricReplyFinished(reply);
                }
            });

    connect(lyricTimer, &QTimer::timeout, this, &MusicSearchDialog::updateLyric);
    if (player) {
        // ========== 1. 处理媒体状态（正常流程） ==========
        connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
            qDebug() << "[媒体状态] 当前状态：" << status;
            switch (status) {
            case QMediaPlayer::LoadingMedia:
                qDebug() << "[状态] 媒体正在加载...";
                break;
            case QMediaPlayer::LoadedMedia:
                qDebug() << "[状态] 媒体加载完成，时长：" << player->duration() / 1000 << "秒";
                break;
            case QMediaPlayer::BufferingMedia:
                qDebug() << "[状态] 媒体缓冲中...";
                break;
            case QMediaPlayer::EndOfMedia:
                qDebug() << "[状态] 播放到媒体末尾";
                break;
            // 其他状态可按需扩展
            default:
                break;
            }
        });

        // ========== 2. 处理错误（异常情况） ==========
        connect(player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            this, [this](QMediaPlayer::Error error) {
                    if (error != QMediaPlayer::NoError) {
                        qWarning() << "[播放错误] 错误码：" << error
                      << "，错误描述：" << player->errorString();
                        // 同步主界面显示错误（需配合 signalUpdateSongInfo）
                        emit signalUpdateSongInfo(currentSongName, "播放失败：" + player->errorString());
                    }
                });

        // ========== 3. 处理播放/暂停状态（原有逻辑） ==========
        connect(player, &QMediaPlayer::stateChanged, this, [this](QMediaPlayer::State state) {
            emit signalUpdatePlayState(state == QMediaPlayer::PlayingState);
        });
    }

    loadStyleSheet();
    sendSearchRequest("");
}

MusicSearchDialog::~MusicSearchDialog()
{
    lyricTimer->stop();
}

void MusicSearchDialog::sendSearchRequest(const QString &keyword)
{
    // 关键：新搜索时，清空旧播放列表和索引
    songList.clear();
    currentIndex = -1;
    musicList->clear(); // 清空列表显示

    // 原有URL和请求逻辑不变...
    QUrl url("http://music.163.com/api/search/get/web");
    QUrlQuery query;
    query.addQueryItem("csrf_token", "");
    query.addQueryItem("hlpretag", "");
    query.addQueryItem("hlposttag", "");
    query.addQueryItem("s", keyword);
    query.addQueryItem("type", "1");
    query.addQueryItem("offset", "0");
    query.addQueryItem("total", "true");
    query.addQueryItem("limit", "10");

    url.setQuery(query);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
    networkManager->get(request);
}

void MusicSearchDialog::onSearchClicked()
{
    sendSearchRequest(searchEdit->text().trimmed());
}

void MusicSearchDialog::onSearchReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "[搜索错误] 请求失败：" << reply->errorString();
        reply->deleteLater();
        return;
    }

    // 1. 读取并解析JSON
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "[搜索错误] JSON格式无效";
        reply->deleteLater();
        return;
    }

    QJsonObject rootObj = doc.object();
    if (rootObj["code"].toInt() != 200) {
        qWarning() << "[搜索错误] 接口返回失败，错误码：" << rootObj["code"].toInt();
        reply->deleteLater();
        return;
    }

    // 2. 清空原有数据
    songList.clear();
    musicList->clear();

    // 3. 提取歌曲列表（核心解析逻辑）
    QJsonObject resultObj = rootObj["result"].toObject();
    QJsonArray songsArray = resultObj["songs"].toArray();
    qDebug() << "[搜索成功] 共返回" << songsArray.size() << "首歌曲，开始过滤解析";

    foreach (const QJsonValue &val, songsArray) {
        if (!val.isObject()) continue; // 跳过非对象数据
        QJsonObject songObj = val.toObject();

        // 3.1 过滤不可播放的歌曲（仅保留status=0）
        int songStatus = songObj["status"].toInt(-1);
        if (songStatus != 0) {
            qDebug() << "[过滤歌曲] 歌曲名：" << songObj["name"].toString()
                     << "，状态码：" << songStatus << "（不可播放）";
            continue;
        }

        // 3.2 提取歌曲ID（必须字段，无则跳过）
        QString songId = QString::number(songObj["id"].toInt());
        if (songId.isEmpty()) {
            qWarning() << "[解析警告] 跳过无ID的歌曲";
            continue;
        }

        // 3.3 提取歌曲名 + 别名（如有）
        QString songName = songObj["name"].toString();
        QJsonArray aliasArray = songObj["alias"].toArray();
        if (!aliasArray.isEmpty()) {
            QString alias = aliasArray.first().toString(); // 取第一个别名（通常只有一个）
            songName += QString("（%1）").arg(alias);
        }

        // 3.4 提取歌手名（兼容多个歌手/无歌手）
        QString artists = "未知歌手";
        QJsonArray artistsArray = songObj["artists"].toArray();
        if (!artistsArray.isEmpty()) {
            artists.clear();
            foreach (const QJsonValue &artistVal, artistsArray) {
                if (artistVal.isObject()) {
                    artists += artistVal.toObject()["name"].toString() + "/";
                }
            }
            artists.chop(1); // 去掉最后一个"/"
        }

        // 3.5 存储歌曲信息并显示到列表
        SongInfo info;
        info.songId = songId;
        info.name = songName;
        info.artist = artists;
        songList.append(info);

        // 显示格式：「歌曲名（别名） - 歌手1/歌手2」
        QString listItemText = QString("%1 - %2").arg(info.name).arg(info.artist);
        musicList->addItem(listItemText);
        qDebug() << "[解析成功] " << listItemText << "，歌曲ID：" << info.songId;
    }

    // 4. 处理无有效歌曲的情况
    if (musicList->count() == 0) {
        musicList->addItem("未找到可播放的歌曲");
        qWarning() << "[搜索结果] 无有效可播放歌曲";
    } else {
        qDebug() << "[搜索结果] 共显示" << musicList->count() << "首可播放歌曲";
    }

    reply->deleteLater();
}

void MusicSearchDialog::onSongItemClicked(QListWidgetItem *item)
{
    int index = musicList->row(item); // 获取点击项的索引
    if (index < 0 || index >= songList.size()) return; // 无效索引直接返回
    if (!player) {
        qWarning() << "[播放错误] QMediaPlayer 未初始化";
        return;
    }

    // 关键：更新当前播放索引
    currentIndex = index;
    SongInfo info = songList[index];
    currentSongName = info.name;

    // 1. 构建播放URL（带Referer避免403）
    QString playUrlStr = QString("http://music.163.com/song/media/outer/url?id=%1.mp3").arg(info.songId);
    QUrl playUrl(playUrlStr);
    QNetworkRequest playRequest(playUrl);
    playRequest.setRawHeader("Referer", "http://music.163.com/");
    playRequest.setHeader(QNetworkRequest::UserAgentHeader,
                          "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");

    // 2. 停止当前播放→设置新媒体→播放
    player->stop();
    player->setMedia(QMediaContent(playRequest));
    player->play();

    // 3. 同步主界面（歌名+加载提示）
    emit signalUpdateSongInfo(info.name, "正在加载歌词...");
    emit signalPlayMedia(playUrl);
    // 高亮搜索列表中的当前歌曲
    musicList->setCurrentRow(currentIndex);
    // 4. 请求当前歌曲的歌词
    sendLyricRequest(info.songId);

    qDebug() << "[播放] 索引：" << currentIndex << "，歌曲：" << info.name;
}

void MusicSearchDialog::sendLyricRequest(const QString &songId)
{
    QUrl url(QString("http://music.163.com/api/song/lyric?id=%1&lv=1&kv=1&tv=-1").arg(songId));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
    networkManager->get(request);
}

void MusicSearchDialog::onLyricReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "[歌词错误] 请求失败：" << reply->errorString();
        lyricLabel->setText("获取歌词失败");
        emit signalUpdateSongInfo(currentSongName, "获取歌词失败");
        reply->deleteLater();
        return;
    }

    // 1. 读取并解析歌词JSON
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "[歌词错误] JSON格式无效";
        lyricLabel->setText("获取歌词失败");
        emit signalUpdateSongInfo(currentSongName, "获取歌词失败");
        reply->deleteLater();
        return;
    }

    QJsonObject rootObj = doc.object();
    lyricMap.clear();
    if (!rootObj.contains("lrc") || !rootObj["lrc"].isObject()) {
        qWarning() << "[歌词错误] 无lrc字段";
        lyricLabel->setText("暂无歌词");
        emit signalUpdateSongInfo(currentSongName, "暂无歌词");
        reply->deleteLater();
        return;
    }

    QString lyricText = rootObj["lrc"].toObject()["lyric"].toString().trimmed();
    if (lyricText.isEmpty()) {
        qWarning() << "[歌词错误] 歌词内容为空";
        lyricLabel->setText("暂无歌词");
        emit signalUpdateSongInfo(currentSongName, "暂无歌词");
        reply->deleteLater();
        return;
    }

    // 2. 关键：极简正则（仅匹配核心格式，兼容2/3位毫秒）
    // 格式：[分:秒.毫秒] 歌词（毫秒1-3位，忽略中英文括号和前后空格）
    QRegExp lyricReg("\\[(\\d+):(\\d+\\.\\d{1,3})\\]\\s*(.*)");
    lyricReg.setCaseSensitivity(Qt::CaseInsensitive); // 忽略大小写（无意义，仅保险）
    QStringList lines = lyricText.split("\n");
    int validCount = 0;

    qDebug() << "[歌词解析] 开始，共" << lines.size() << "行，正则：" << lyricReg.pattern();

    // 3. 逐行匹配（用indexIn而非exactMatch，更灵活）
    foreach (const QString &line, lines) {
        QString cleanLine = line.trimmed(); // 去除行首行尾空格/换行符（避免隐藏字符干扰）
        if (cleanLine.isEmpty()) {
            qDebug() << "[歌词解析] 跳过空行";
            continue;
        }

        // 用indexIn匹配（返回匹配位置，-1表示失败），不要求整行完全匹配
        int matchPos = lyricReg.indexIn(cleanLine);
        if (matchPos == -1) {
            qDebug() << "[歌词解析] 匹配失败：" << cleanLine;
            continue;
        }

        // 提取分组（1=分钟，2=秒.毫秒，3=歌词内容）
        QString minuteStr = lyricReg.cap(1);
        QString secMsStr = lyricReg.cap(2);
        QString lyric = lyricReg.cap(3).trimmed();

        // 过滤空歌词
        if (lyric.isEmpty()) {
            qDebug() << "[歌词解析] 跳过空歌词：" << cleanLine;
            continue;
        }

        // 计算时间戳（毫秒）
        bool minOk = false, secOk = false;
        int minute = minuteStr.toInt(&minOk);
        double secMs = secMsStr.toDouble(&secOk);
        if (!minOk || !secOk) {
            qDebug() << "[歌词解析] 时间格式错误：" << cleanLine;
            continue;
        }
        qint64 timeMs = (minute * 60 + secMs) * 1000;

        // 存储歌词
        lyricMap[timeMs] = lyric;
        validCount++;
        qDebug() << "[歌词解析成功] 时间：" << timeMs << "ms | 歌词：" << lyric;
    }

    // 4. 处理结果
    qDebug() << "[歌词解析] 完成，有效歌词：" << validCount << "句";
    if (validCount == 0) {
        qWarning() << "[歌词错误] 无有效歌词";
        lyricLabel->setText("暂无歌词");
        emit signalUpdateSongInfo(currentSongName, "暂无歌词");
    } else {
        if (!lyricTimer->isActive()) {
            lyricTimer->start();
            qDebug() << "[歌词定时器] 启动（1秒更新）";
        }
        // 初始显示第一句
        if (!lyricMap.isEmpty()) {
            qint64 firstTime = lyricMap.keys().first();
            QString firstLyric = lyricMap[firstTime];
            lyricLabel->setText(firstLyric);
            emit signalUpdateSongInfo(currentSongName, firstLyric);
        }
    }

    reply->deleteLater();
}

void MusicSearchDialog::updateLyric()
{
    if (lyricMap.isEmpty() || !player) {
        return;
    }

    // 获取当前播放器进度（毫秒）
    qint64 currentPlayMs = player->position();
    QString targetLyric; // 目标歌词（当前进度对应的歌词）
    qint64 closestTime = 0; // 最接近当前进度的时间戳

    // 遍历所有歌词时间戳，找到最匹配的歌词
    QList<qint64> allTimeMs = lyricMap.keys();
    foreach (qint64 timeMs, allTimeMs) {
        // 核心逻辑：时间戳 ≤ 当前进度，且尽可能接近当前进度
        if (timeMs <= currentPlayMs && timeMs > closestTime) {
            closestTime = timeMs;
            targetLyric = lyricMap[timeMs];
        }
    }

    // 同步显示歌词（主界面 + 搜索对话框）
    if (!targetLyric.isEmpty()) {
        lyricLabel->setText(targetLyric);
        emit signalUpdateSongInfo(currentSongName, targetLyric);
        qDebug() << "[歌词同步] 当前进度：" << currentPlayMs << "ms → 歌词：" << targetLyric;
    }
}

// 上一首逻辑
void MusicSearchDialog::onPrevMedia()
{
    // 1. 检查播放列表是否为空
    if (songList.isEmpty()) {
        qWarning() << "[上一首] 播放列表为空";
        emit signalUpdateSongInfo("", "无播放歌曲");
        lyricLabel->setText("无播放歌曲");
        return;
    }

    // 2. 计算上一首索引（未播放过则默认第一首）
    if (currentIndex == -1) {
        currentIndex = 0; // 未播放时，上一首=第一首
    } else {
        currentIndex--;
        if (currentIndex < 0) { // 小于0→跳到最后一首
            currentIndex = songList.size() - 1;
        }
    }

    // 3. 获取上一首歌曲信息
    SongInfo info = songList[currentIndex];
    currentSongName = info.name;

    // 4. 播放上一首（同点击播放逻辑）
    QString playUrlStr = QString("http://music.163.com/song/media/outer/url?id=%1.mp3").arg(info.songId);
    QUrl playUrl(playUrlStr);
    QNetworkRequest playRequest(playUrl);
    playRequest.setRawHeader("Referer", "http://music.163.com/");
    playRequest.setHeader(QNetworkRequest::UserAgentHeader,
                          "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");

    player->stop();
    player->setMedia(QMediaContent(playRequest));
    player->play();

    // 5. 同步更新（主界面+搜索列表）
    emit signalUpdateSongInfo(info.name, "正在加载歌词...");
    emit signalPlayMedia(playUrl);
    musicList->setCurrentRow(currentIndex); // 高亮上一首
    sendLyricRequest(info.songId); // 请求上一首歌词

    qDebug() << "[上一首] 切换到索引：" << currentIndex << "，歌曲：" << info.name;
}

// 下一首逻辑
void MusicSearchDialog::onNextMedia()
{
    // 1. 检查播放列表是否为空
    if (songList.isEmpty()) {
        qWarning() << "[下一首] 播放列表为空";
        emit signalUpdateSongInfo("", "无播放歌曲");
        lyricLabel->setText("无播放歌曲");
        return;
    }

    // 2. 计算下一首索引（未播放过则默认第一首）
    if (currentIndex == -1) {
        currentIndex = 0; // 未播放时，下一首=第一首
    } else {
        currentIndex++;
        if (currentIndex >= songList.size()) { // 超过列表长度→跳到第一首
            currentIndex = 0;
        }
    }

    // 3. 获取下一首歌曲信息
    SongInfo info = songList[currentIndex];
    currentSongName = info.name;

    // 4. 播放下一首（同点击播放逻辑）
    QString playUrlStr = QString("http://music.163.com/song/media/outer/url?id=%1.mp3").arg(info.songId);
    QUrl playUrl(playUrlStr);
    QNetworkRequest playRequest(playUrl);
    playRequest.setRawHeader("Referer", "http://music.163.com/");
    playRequest.setHeader(QNetworkRequest::UserAgentHeader,
                          "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");

    player->stop();
    player->setMedia(QMediaContent(playRequest));
    player->play();

    // 5. 同步更新（主界面+搜索列表）
    emit signalUpdateSongInfo(info.name, "正在加载歌词...");
    emit signalPlayMedia(playUrl);
    musicList->setCurrentRow(currentIndex); // 高亮下一首
    sendLyricRequest(info.songId); // 请求下一首歌词

    qDebug() << "[下一首] 切换到索引：" << currentIndex << "，歌曲：" << info.name;
}
void MusicSearchDialog::loadStyleSheet()
{
    QString style = R"(
        QDialog {
            background-color: #C1E3F7;
            border-radius: 10px;
        }
        QLineEdit {
            border: 1px solid #DCDFE6;
            border-radius: 6px;
            padding-left: 12px;
            font-size: 14px;
            color: #333;
            background-color: white;
        }
        QLineEdit:focus {
            border-color: #409EFF;
            outline: none;
        }
        QPushButton {
            background-color: #409EFF;
            color: white;
            border: none;
            border-radius: 6px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #357ABD;
        }
        QPushButton:pressed {
            background-color: #2A6CBF;
        }
        QListWidget {
            background-color: white;
            border: 1px solid #DCDFE6;
            border-radius: 6px;
            font-size: 14px;
            color: #333;
        }
        QListWidget::item {
            outline: none;
            padding: 5px 10px;
        }
        QListWidget::item:hover {
            background-color: #E6F7FF;
            border-radius: 6px;
        }
        QListWidget::item:selected {
            background-color: #409EFF;
            color: white;
            border-radius: 6px;
        }
        QScrollBar:vertical {
            border: none;
            background: #F5F7FA;
            width: 8px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background: #C0C4CC;
            border-radius: 4px;
        }
    )";
    setStyleSheet(style);
}
