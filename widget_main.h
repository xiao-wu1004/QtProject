#ifndef WIDGET_MAIN_H
#define WIDGET_MAIN_H

#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QMouseEvent>
#include <QPainter>
#include "button_custom.h"  // 仅包含整合后的按钮头文件
#include "widget_assist_backdrop.h"
#include "widget_assist_menu.h"

// 页面索引枚举（不变）
enum PageIndex
{
    PAGE_SMART_CHAT = 0,
    PAGE_VISION_RECOG,
    PAGE_SMART_HOME,
    PAGE_WEATHER,
    PAGE_CONTROL_SCREEN,
    PAGE_MOTOR
};

// 【关键修改】删除自定义状态宏，直接使用 button_custom.h 中的宏
// 原：#define Mode_No -1  #define Mode_Selected 0
// 改为使用 button_custom.h 中的 Mode_No/Mode_Press 等

class Widget_Main : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_Main(QWidget* parent = nullptr);
    ~Widget_Main();

protected:
    void paintEvent(QPaintEvent* event) ;
    void mousePressEvent(QMouseEvent* event) ;
    void mouseReleaseEvent(QMouseEvent* event) ;
    void mouseMoveEvent(QMouseEvent* event) ;

private slots:
    void slots_click_menu();  // 菜单按钮点击逻辑
    void slots_close_window(); // 关闭窗口

private:
    // 1. 辅助背景控件（不变）
    Widget_Assist_Backdrop* mainBackdrop;
    Widget_Assist_Menu* leftMenuBackdrop;

    // 2. 顶部控件（按钮类型为 button_custom.h 中的类）
    Button_Main_Close* butto_closr;  // 关闭按钮（来自 button_custom.h）
    QLabel* Window_Title;

    // 3. 左侧菜单（菜单按钮类型为 button_custom.h 中的类）
    QWidget* leftWidget;
    QLabel* label_head_img;
    Button_Main_Menu* label_smart_chat;  // 菜单按钮（来自 button_custom.h）
    Button_Main_Menu* label_vision_recog;
    Button_Main_Menu* label_smart_home;
    Button_Main_Menu* label_weather;
    Button_Main_Menu* label_control_screen;
    Button_Main_Menu* label_motor;

    // 4. 右侧页面容器（不变）
    QStackedWidget* stackedWidget;

    // 5. 功能页面（不变）
    class SmartDialogWidget* Widget_Smart_chat;
    class VisualRecogWidget* Widget_Smart_vision;
    class SmartHomeWidget* Widget_Smart_Home;
    class WeatherWidget* Widget_Weather;
    class IndustryWidget* Widget_Control_industrial;
    class MotorControlWidget* Widget_Control_motor;

    // 6. 窗口拖动（不变）
    QPoint Mouse_Press_Pos;
};

#endif // WIDGET_MAIN_H
