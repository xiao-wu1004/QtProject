#include "widget_assist_menu.h"

Widget_Assist_Menu::Widget_Assist_Menu(QWidget* parent) : QWidget(parent)
{
    // 菜单背景不拦截鼠标事件
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setStyleSheet("background-color: transparent;"); // 自身背景透明（避免遮挡）
}

void Widget_Assist_Menu::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制固定蓝色圆角背景
    QPen pen;
    pen.setColor(menuColor);
    pen.setWidth(1);
    painter.setPen(pen);

    QBrush brush;
    brush.setColor(menuColor);
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);

    painter.drawRoundedRect(rect(), 15, 15); // 圆角15px
}
