#include "MessageWidget.h"

MessageWidget::MessageWidget(const QString& content, MessageType type, QWidget *parent)
    : QWidget(parent), m_content(content), m_type(type)
{
    initUI();
}

void MessageWidget::initUI()
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 3, 10, 3);
    m_mainLayout->setSpacing(0);

    m_msgLabel = new QLabel(m_content, this);
    m_msgLabel->setWordWrap(true); // 长文本自动换行
    m_msgLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);


    // 初始设置最大宽度：如果有父容器，取父容器宽度的75%（避免过宽）
    if (parentWidget())
    {
        m_msgLabel->setMaximumWidth(parentWidget()->width() * 0.75);
    }
    else
    {
        m_msgLabel->setMaximumWidth(500); // 无父容器时用默认值
    }
    if (m_type == MessageType::UserType) {
        m_mainLayout->addStretch(1);
        // 关键：用户气泡标签设置为“首选宽度”（仅占内容所需宽度），避免拉伸占满空间
        m_msgLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        m_mainLayout->addWidget(m_msgLabel);
        m_msgLabel->setStyleSheet(R"(
            QLabel {
                background-color: #409EFF; /* 用户气泡用主色调蓝色，与发送按钮呼应 */
                color: white;
                padding: 12px 18px;
                border-radius: 20px 20px 4px 20px; /* 右上/右下圆角，左下直角 */
                font-family: "Microsoft YaHei", "SimHei";
                font-size: 14px;
                line-height: 1.6;
                box-shadow: 0 2px 5px rgba(0,0,0,0.1); /* 轻微阴影，增强立体度 */
            }
        )");
    } else {
        m_mainLayout->addWidget(m_msgLabel);
        m_mainLayout->addStretch(1);
        m_msgLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred); // AI气泡也用Preferred
        m_msgLabel->setStyleSheet(R"(
            QLabel {
                background-color: #DADADA;
                color: #333;
                padding: 12px 18px;
                border-radius: 20px 20px 20px 4px; /* 左上/左下圆角，右下直角 */
                font-family: "Microsoft YaHei", "SimHei";
                font-size: 14px;
                line-height: 1.6;
                box-shadow: 0 4px 7px rgba(0,0,0,0.4); /* AI气泡也加阴影，保持统一 */
            }
        )");
    }

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

// 新增：窗口大小变化时，重新设置气泡最大宽度为父容器的75%
void MessageWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_msgLabel && parentWidget())
    {
        // 关键：用父容器（QListWidget）的宽度计算最大宽度，而非自身宽度
        int maxWidth = parentWidget()->width() * 0.75;
        // 限制最小宽度，避免过窄导致文字挤压
        maxWidth = qMax(maxWidth, 200);
        m_msgLabel->setMaximumWidth(maxWidth);
        // 宽度变化后强制重新计算尺寸
        m_msgLabel->adjustSize();
        this->adjustSize();
    }
}
