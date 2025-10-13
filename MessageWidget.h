#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QSizePolicy>

enum class MessageType {
    UserType,
    AiType
};

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget(const QString& content, MessageType type, QWidget *parent = nullptr);

protected:
    // 新增：窗口大小变化时，动态调整气泡宽度
    void resizeEvent(QResizeEvent *event) override;

private:
    void initUI();

private:
    QString m_content;
    MessageType m_type;
    QHBoxLayout* m_mainLayout;
    QLabel* m_msgLabel;
};

#endif // MESSAGEWIDGET_H
