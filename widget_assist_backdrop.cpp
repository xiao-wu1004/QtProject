#include "widget_assist_backdrop.h"

Widget_Assist_Backdrop::Widget_Assist_Backdrop(QWidget* parent) : QWidget(parent)
{
    // 👉 强制透传事件，确保子部件（HomeModeLabel）能收到点击
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    // 默认背景色：浅蓝色
    color = QColor(204, 235, 255);
//    // 背景控件不拦截鼠标事件（确保上层控件能正常响应）
//    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void Widget_Assist_Backdrop::set_color(QColor color)
{
    this->color = color;
    update(); // 触发重绘
}
void Widget_Assist_Backdrop::setLabelText(const QString& text)
{
    m_labelText = text;
    update(); // 触发重绘，显示新文本
}
void Widget_Assist_Backdrop::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿

    // 绘制圆角背景
    QPen pen;
    pen.setColor(color);
    pen.setWidth(0);
    painter.setPen(pen);

    QBrush brush;
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);

    painter.drawRoundedRect(rect(), 15, 15); // 圆角15px

    // 2. 绘制标签文本（居中显示）
    if (!m_labelText.isEmpty())
    {
        QFont font;
        font.setPointSize(10);
        painter.setFont(font);
        painter.setPen(Qt::black);
        painter.drawText(rect(), Qt::AlignCenter, m_labelText);
    }
}


