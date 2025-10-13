#ifndef WIDGET_ASSIST_MENU_H
#define WIDGET_ASSIST_MENU_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QBrush>
#include <QPen>

class Widget_Assist_Menu : public QWidget
{
    Q_OBJECT
public:
    explicit Widget_Assist_Menu(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    // 菜单背景固定色：蓝色 #0099FF
    const QColor menuColor = QColor( 47,185,238);
};

#endif // WIDGET_ASSIST_MENU_H
