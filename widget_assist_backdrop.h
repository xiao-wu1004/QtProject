#ifndef WIDGET_ASSIST_BACKDROP_H
#define WIDGET_ASSIST_BACKDROP_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QBrush>

class Widget_Assist_Backdrop : public QWidget
{
    Q_OBJECT
public:
    explicit Widget_Assist_Backdrop(QWidget* parent = nullptr);
    // 设置背景颜色
    void set_color(QColor color);
    void setLabelText(const QString& text);   // 新增：设置容器标签文本

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QColor color; // 背景色
    QString m_labelText;                    // 新增：存储标签文本
};

#endif // WIDGET_ASSIST_BACKDROP_H
