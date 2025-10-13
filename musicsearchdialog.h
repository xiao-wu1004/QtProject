#ifndef MUSICSEARCHDIALOG_H
#define MUSICSEARCHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMediaPlayer> // 新增：媒体播放器
#include <QTimer>
#include <QLabel>
#include <QPair> // 用于存储“歌名-播放URL”

struct SongInfo {
    QString name;
    QString artist;
    QString songId;
};

class MusicSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MusicSearchDialog(QWidget *parent = nullptr, QMediaPlayer *player = nullptr);
    ~MusicSearchDialog() override;

signals:
    void signalPlayMedia(const QUrl &url);          // 发射“播放媒体”信号
    void signalUpdateSongInfo(const QString &name, const QString &lyric); // 发射“更新歌曲信息”信号
    void signalUpdatePlayState(bool isPlaying);     // 发射“更新播放状态”信号
    void signalNextMedia();                         // 发射“下一首”信号
    void signalPrevMedia();                         // 发射“上一首”信号

private slots:
    void onSearchClicked();
    void onSearchReplyFinished(QNetworkReply *reply);
    void onSongItemClicked(QListWidgetItem *item);
    void onLyricReplyFinished(QNetworkReply *reply);
    void updateLyric();
public slots:
    // 新增：上下首槽函数声明
    void onPrevMedia(); // 上一首
    void onNextMedia(); // 下一首

private:
    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    QListWidget *musicList;
    QLabel *lyricLabel;
    QNetworkAccessManager *networkManager;
    QMediaPlayer *player; // 共享的播放器
    QList<SongInfo> songList;
    QMap<qint64, QString> lyricMap;
    QTimer *lyricTimer;
    QString currentSongName; // 当前歌曲名（用于同步）
    // 新增：当前播放歌曲在songList中的索引（-1表示未播放）
    int currentIndex = -1;


    void loadStyleSheet();
    void sendSearchRequest(const QString &keyword);
    void sendLyricRequest(const QString &songId);
    void closeEvent(QCloseEvent *event) override;
};

#endif // MUSICSEARCHDIALOG_H
