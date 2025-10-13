#ifndef BUTTON_CUSTOM_H
#define BUTTON_CUSTOM_H
#include <QWidget>      // 基本窗口控件
#include <QLabel>       // 标签控件

#include <QIcon>        // 图标类
#include <QImage>       // 处理图片数据类
#include <QString>      // 字符串类

#include <QMouseEvent>  // 鼠标事件类
#include <QEvent>       // Qt 事件基类
#include <QPainter>     // 绘图类
#include <QPen>         // 画笔类
#include <QBrush>       // 画刷类

#include <QColor>       // 颜色类
#include <QFont>        // 字体类
#include <QRect>        // 矩形类

#include <QDebug>       // 调试输出类

#include <QMouseEvent>

// 自定义控件 标签按钮 父类

#define Mode_No         0   // 无事件
#define Mode_enter      1   // 鼠标进入
#define Mode_leave      2   // 鼠标离开
#define Mode_Press      3   // 鼠标按下
#define Mode_Release    4   // 鼠标释放

class Button_Cust_Base : public QLabel
{
    Q_OBJECT
public:
    explicit Button_Cust_Base(QWidget* parent = nullptr);

public:
    // 设定图标
    void Set_Image(QString Image_path);

    // 文本
    void Set_Test(QString Text);

    // 更新状态
    void update_start(int Mode_Code);

protected:
    // 图标
    QImage M_Image;
    // 文本
    QString M_Text;
    // 按钮状态码
    int Mode_Code = -1;

protected:
    //    // 绘图事件
    //    void paintEvent(QPaintEvent *event);

    //    // 进入事件
    //    void enterEvent(QEvent *event);

    //    // 离开事件
    //    void leaveEvent(QEvent *event);

    //    // 按下事件
    //    void mousePressEvent(QMouseEvent *event);

    //    // 释放事件
    //    void mouseReleaseEvent(QMouseEvent *event);

    // 移动事件
    void mouseMoveEvent(QMouseEvent* event);

signals:
    // 当前按钮被按下则发出此信号
    void signals_click();

};

// 自定义控件 便签按钮 关闭按钮

class Button_Main_Close : public Button_Cust_Base
{
    Q_OBJECT
public:
    explicit Button_Main_Close(QWidget* parent = nullptr);

protected:

    // 进入事件
    void enterEvent(QEvent* event);

    // 离开事件
    void leaveEvent(QEvent* event);

    // 按下事件
    void mousePressEvent(QMouseEvent* event);

    // 释放事件
    void mouseReleaseEvent(QMouseEvent* event);

signals:


};

// 自定义控件 标签按钮 菜单按钮

class Button_Main_Menu : public Button_Cust_Base
{
    Q_OBJECT
public:
    explicit Button_Main_Menu(QWidget* parent = nullptr);

protected:
    // 绘图事件
    void paintEvent(QPaintEvent* event);

    // 进入事件
    void enterEvent(QEvent* event);

    // 离开事件
    void leaveEvent(QEvent* event);

    void mouseMoveEvent(QMouseEvent* event);

    // 按下事件
    void mousePressEvent(QMouseEvent* event);

    // 释放事件
    void mouseReleaseEvent(QMouseEvent* event);


signals:


};

#endif // BUTTON_CUSTOM_H
