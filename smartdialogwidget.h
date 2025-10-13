#ifndef SMARTDIALOGWIDGET_H
#define SMARTDIALOGWIDGET_H

#include <QWidget>
#include <QListWidget>  // 替换QTextEdit为QListWidget
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QElapsedTimer>
#include "MessageWidget.h"  // 引入自定义气泡控件

class SmartDialogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SmartDialogWidget(QWidget *parent = nullptr);
    void setSiliconFlowApiKey(const QString& key) { m_apiKey = key; }

private slots:
    void onSendClicked();
    void onClearClicked();
    void onReturnPressed();
    void onNetworkReplyFinished(QNetworkReply* reply);

private:
    // UI控件：删除QTextEdit，新增QListWidget
    QListWidget* m_chatListWidget; // 聊天列表容器（替代原m_chatHistoryEdit）
    QLineEdit* m_inputEdit;
    QPushButton* m_sendBtn;
    QPushButton* m_clearBtn;
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_inputLayout;

    // 网络相关（不变）
    QNetworkAccessManager* m_networkManager;
    QString m_apiKey;
    bool m_isRequesting;
    QElapsedTimer m_requestTimer;
     QList<QVariantMap> m_chatMessages; // 存储聊天消息的数据模型

    // 新增：添加消息到列表（替代原updateChatHistory）
    void addMessageToChat(const QString& content, MessageType type);
    // 辅助函数（不变）
    bool checkApiKeyValid();
    QString getHttpStatusCodeDesc(int code);
    // 移除原createUserMessageHtml/createAIMessageHtml（无需HTML了）
};

#endif // SMARTDIALOGWIDGET_H
