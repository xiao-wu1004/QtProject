#include "smartdialogwidget.h"
#include "MessageWidget.h"
#include <QDebug>
#include <QMessageBox>

SmartDialogWidget::SmartDialogWidget(QWidget* parent) : QWidget(parent)
    , m_apiKey("sk-noyjnyndpkiruhwwgzufiidwqqxenfxuizdggyiqjbogymqs")
    , m_isRequesting(false)
{
    // 1. 强制清洗API密钥（不变）
    m_apiKey = m_apiKey.trimmed();
    qDebug() << "[硅基流动密钥] 清洗后：" << m_apiKey;
    qDebug() << "[密钥长度]：" << m_apiKey.length();

    // 1. 整体背景：浅蓝（与参考图色调匹配，可微调色值）
    this->setStyleSheet(R"(
        SmartDialogWidget {
            background-color: #E4EEFF; /* 浅蓝背景，可根据实际图微调 */
        }
    )");
    // 2. 初始化主布局（不变）
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(15);

    // 3. 聊天列表（白色圆角+轻微阴影，突出层次）
    m_chatListWidget = new QListWidget(this);
    m_chatListWidget->setStyleSheet(R"(
        QListWidget {
            background-color: #CCEBFF; /* 聊天区域白底，与参考“消息接收区”一致 */
            border: none;
            border-radius: 12px;   /* 柔和圆角 */
            padding: 15px;         /* 内边距更舒适 */
            font-family: "Microsoft YaHei", "SimHei";
            color: #333;
            outline: none;
            box-shadow: 0 2px 8px rgba(0,0,0,0.05); /* 轻微阴影，模拟悬浮感 */
        }
        QListWidget::item {
            border: none;
            height: auto;
        }
        QScrollBar:vertical {
            border: none;
            background: #F0F0F0;
            width: 8px;
            margin: 0;
            border-radius: 4px; /* 滚动条圆角 */
        }
        QScrollBar::handle:vertical {
            background: #CCCCCC;
            border-radius: 4px;
        }
    )");
    m_chatListWidget->setFocusPolicy(Qt::NoFocus); // 取消列表焦点（避免选中效果）
    m_mainLayout->addWidget(m_chatListWidget, 1); // 1表示占满剩余空间

    // 4. 输入区布局（完全不变）
    m_inputLayout = new QHBoxLayout();
    m_inputLayout->setSpacing(10);

    m_inputEdit = new QLineEdit(this);
    m_inputEdit->setStyleSheet(R"(
        QLineEdit {
            background-color: white;
            border: 1px solid #C0D8F0; /* 浅蓝边框，柔和过渡 */
            border-radius: 20px;
            padding: 0 18px;
            font-family: "Microsoft YaHei", "SimHei";
            font-size: 14px;
            height: 40px;
            box-shadow: 0 1px 3px rgba(0,0,0,0.05); /* 轻微阴影 */
        }
        QLineEdit:focus {
            border-color: #409EFF; /* 聚焦时高亮边框 */
            outline: none;
        }
    )");
    m_inputEdit->setPlaceholderText("请输入你的问题...");
    m_inputLayout->addWidget(m_inputEdit, 1);

    m_sendBtn = new QPushButton("发送", this);
    m_sendBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #409EFF; /* 主色调蓝色按钮 */
            color: white;
            border: none;
            border-radius: 20px;
            padding: 0 24px;
            font-family: "Microsoft YaHei", "SimHei";
            font-size: 14px;
            height: 40px;
            cursor: pointer;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1); /* 按钮阴影增强层次 */
        }
        QPushButton:hover {
            background-color: #3688E8; /* hover时加深颜色 */
        }
        QPushButton:disabled {
            background-color: #A0CFFF;
            cursor: not-allowed;
        }
    )");
    m_inputLayout->addWidget(m_sendBtn);

    m_clearBtn = new QPushButton("清空", this);
    m_clearBtn->setStyleSheet(R"(
        QPushButton {
            background-color: white;
            color: #666;
            border: 1px solid #C0D8F0; /* 浅蓝边框，与输入框呼应 */
            border-radius: 20px;
            padding: 0 18px;
            font-family: "Microsoft YaHei", "SimHei";
            font-size: 14px;
            height: 40px;
            cursor: pointer;
            box-shadow: 0 1px 3px rgba(0,0,0,0.05);
        }
        QPushButton:hover {
            background-color: #F5F9FF; /* hover时浅蓝底色，增强反馈 */
        }
    )");
    m_inputLayout->addWidget(m_clearBtn);

    m_mainLayout->addLayout(m_inputLayout);

    // 5. 初始化网络管理器+绑定信号槽（不变）
    m_networkManager = new QNetworkAccessManager(this);
    connect(m_sendBtn, &QPushButton::clicked, this, &SmartDialogWidget::onSendClicked);
    connect(m_clearBtn, &QPushButton::clicked, this, &SmartDialogWidget::onClearClicked);
    connect(m_inputEdit, &QLineEdit::returnPressed, this, &SmartDialogWidget::onSendClicked);
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &SmartDialogWidget::onNetworkReplyFinished);

    // 6. 初始提示（替换为QListWidget的气泡提示）
    if (m_apiKey.isEmpty() || m_apiKey == "sk-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx") {
        addMessageToChat("提示：请先替换为你的硅基流动API密钥（从https://www.siliconflow.cn/获取）！", MessageType::AiType);
        m_sendBtn->setDisabled(true);
    }
}

// 新增：添加消息到QListWidget（核心函数）
void SmartDialogWidget::addMessageToChat(const QString& content, MessageType type)
{
    QListWidgetItem* item = new QListWidgetItem(m_chatListWidget);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

    MessageWidget* msgWidget = new MessageWidget(content, type, m_chatListWidget);
    m_chatListWidget->setItemWidget(item, msgWidget);

    // 关键：强制气泡布局更新，确保 sizeHint 准确
    msgWidget->show(); // 先显示，否则尺寸计算可能不准确
    msgWidget->adjustSize();

    item->setSizeHint(msgWidget->sizeHint()); // 基于准确的大小设置列表项尺寸

    m_chatListWidget->scrollToBottom();
}

// 发送按钮点击（修改：用addMessageToChat替代原HTML逻辑）
void SmartDialogWidget::onSendClicked()
{
    if (m_isRequesting) return;
    if (!checkApiKeyValid()) return;

    QString userInput = m_inputEdit->text().trimmed();
    if (userInput.isEmpty()) {
        QMessageBox::information(this, "输入为空", "请输入你的问题后再发送！");
        return;
    }

    // 1. 添加用户消息（右对齐绿色气泡）
    addMessageToChat(userInput, MessageType::UserType);
    m_inputEdit->clear();

    // 2. 显示"AI正在思考"（左对齐白色气泡，临时消息）
    addMessageToChat("AI正在思考...", MessageType::AiType);
    m_sendBtn->setDisabled(true);
    m_isRequesting = true;

    // 3. 构造网络请求（完全不变）
    QJsonObject requestBody;
    requestBody["model"] = "deepseek-ai/DeepSeek-V3.2-Exp";
    QJsonArray messagesArr;
    QJsonObject msgObj;
    msgObj["role"] = "user";
    msgObj["content"] = userInput;
    messagesArr.append(msgObj);
    requestBody["messages"] = messagesArr;
    requestBody["temperature"] = 0.7;
    requestBody["max_tokens"] = 2048;

    QNetworkRequest request;
    QUrl apiUrl("https://api.siliconflow.cn/v1/chat/completions");
    request.setUrl(apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString authHeader = QString("Bearer %1").arg(m_apiKey);
    request.setRawHeader("Authorization", authHeader.toUtf8());

    qDebug() << "[API地址]：" << apiUrl.toString();
    qDebug() << "[认证头]：" << authHeader;
    qDebug() << "[请求体]：" << QJsonDocument(requestBody).toJson(QJsonDocument::Indented);

    m_requestTimer.start();
    QJsonDocument doc(requestBody);
    m_networkManager->post(request, doc.toJson(QJsonDocument::Compact));
}

// 清空对话（修改：清空QListWidget）
void SmartDialogWidget::onClearClicked()
{
    m_chatListWidget->clear();   // 直接清空界面列表
    m_chatMessages.clear();      // 直接清空历史消息数据
    // 若API密钥未设置，重新显示提示
    if (m_apiKey.isEmpty() || m_apiKey == "sk-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx") {
        addMessageToChat("提示：请先替换为你的硅基流动API密钥（从https://www.siliconflow.cn/获取）！", MessageType::AiType);
    }
}

// 网络响应处理（修改：删除"AI正在思考"，添加AI回复）
void SmartDialogWidget::onNetworkReplyFinished(QNetworkReply* reply)
{
    m_isRequesting = false;
    m_sendBtn->setEnabled(true);

    // 1. 打印响应日志（不变）
    int httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "\n===================================== API响应日志 =====================================";
    qDebug() << "[请求URL]：" << reply->request().url().toString();
    qDebug() << "[HTTP状态码]：" << httpStatusCode << "（" << getHttpStatusCodeDesc(httpStatusCode) << "）";
    qDebug() << "[响应耗时]：" << m_requestTimer.elapsed() << "ms";

    QByteArray responseData = reply->readAll();
    qDebug() << "\n[1. 原始响应数据]：" << QString::fromUtf8(responseData);

    QJsonParseError jsonError;
    QJsonDocument responseDoc = QJsonDocument::fromJson(responseData, &jsonError);
    qDebug() << "\n[2. JSON格式化响应]：";
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析失败！原因：" << jsonError.errorString();
    } else {
        qDebug() << responseDoc.toJson(QJsonDocument::Indented);
        if (responseDoc.isObject()) {
            QJsonObject responseObj = responseDoc.object();
            if (responseObj.contains("model")) qDebug() << "[关键信息] 模型名：" << responseObj["model"].toString();
            if (responseObj.contains("usage")) {
                QJsonObject usageObj = responseObj["usage"].toObject();
                qDebug() << "[关键信息] Token使用：输入" << usageObj["prompt_tokens"].toInt()
                         << "个，输出" << usageObj["completion_tokens"].toInt()
                         << "个，总计" << usageObj["total_tokens"].toInt() << "个";
            }
        }
    }
    qDebug() << "=====================================================================================\n";

    // 2. 移除"AI正在思考"临时消息（关键：删除列表最后一项）
    if (m_chatListWidget->count() > 0) {
        m_chatListWidget->takeItem(m_chatListWidget->count() - 1); // 移除最后一项
    }

    // 3. 错误处理（不变，仅修改UI显示方式）
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("网络错误：%1（HTTP状态码：%2）").arg(reply->errorString()).arg(httpStatusCode);
        addMessageToChat(errorMsg, MessageType::AiType); // 用AI气泡显示错误
        reply->deleteLater();
        return;
    }

    if (jsonError.error != QJsonParseError::NoError) {
        addMessageToChat("API响应格式错误：不是有效的JSON！", MessageType::AiType);
        reply->deleteLater();
        return;
    }

    QJsonObject responseObj = responseDoc.object();
    if (responseObj.contains("error")) {
        QString errorMsg = responseObj["error"].toObject()["message"].toString();
        QString errorType = responseObj["error"].toObject()["type"].toString();
        addMessageToChat(QString("API错误（%1）：%2").arg(errorType).arg(errorMsg), MessageType::AiType);
        reply->deleteLater();
        return;
    }

    // 4. 解析AI回复并显示（用addMessageToChat添加）
    if (responseObj.contains("choices") && responseObj["choices"].isArray()) {
        QJsonArray choicesArr = responseObj["choices"].toArray();
        if (!choicesArr.isEmpty()) {
            QJsonObject choiceObj = choicesArr.first().toObject();
            QJsonObject messageObj = choiceObj["message"].toObject();
            QString aiContent = messageObj["content"].toString().trimmed();

            // 处理换行和多余空格（保持原逻辑）
            aiContent = aiContent.replace("\\\\n", "\n").replace("\\\\t", "\t");
            aiContent.replace(QRegExp(R"(<br\s*\/?>)", Qt::CaseInsensitive), "\n");
            aiContent.replace(QRegExp(R"(\n{3,})"), "\n\n");
            aiContent = aiContent.split("\n").replaceInStrings(QRegExp(R"(\s+$)"), "").join("\n");

            // 添加AI回复（左对齐白色气泡）
            addMessageToChat(aiContent, MessageType::AiType);
        }
    }

    reply->deleteLater();
}

// 检查API密钥（不变）
bool SmartDialogWidget::checkApiKeyValid()
{
    if (m_apiKey.isEmpty()) {
        QMessageBox::warning(this, "API密钥错误", "API密钥不能为空，请从硅基流动官网（https://www.siliconflow.cn/）获取！");
        return false;
    }
    if (!m_apiKey.startsWith("sk-") || m_apiKey.length() < 32) {
        QMessageBox::warning(this, "API密钥错误", "API密钥格式不正确（应为sk-开头的字符串，长度约40位），请检查！");
        return false;
    }
    return true;
}

// HTTP状态码描述（不变）
QString SmartDialogWidget::getHttpStatusCodeDesc(int code)
{
    switch (code) {
    case 200: return "成功（OK）";
    case 400: return "客户端错误（Bad Request）：请求参数错误";
    case 401: return "客户端错误（Unauthorized）：认证失败（密钥无效/缺失）";
    case 403: return "客户端错误（Forbidden）：权限不足（无模型调用权限）";
    case 404: return "客户端错误（Not Found）：API地址错误";
    case 429: return "客户端错误（Too Many Requests）：请求频率超限";
    case 500: return "服务端错误（Internal Server Error）：服务端异常";
    case 503: return "服务端错误（Service Unavailable）：服务暂不可用";
    default: return "未知状态码";
    }
}

// 输入框按Enter（不变）
void SmartDialogWidget::onReturnPressed()
{
    onSendClicked();
}
