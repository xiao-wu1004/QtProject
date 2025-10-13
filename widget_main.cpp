#include "widget_main.h"
#include "smartdialogwidget.h"
#include "visualrecogwidget.h"
#include "smarthomewidget.h"
#include "weatherwidget.h"
#include "industrywidget.h"
#include "motorcontrolwidget.h"

#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>

Widget_Main::Widget_Main(QWidget* parent) : QWidget(parent)
{
    /*************************** 1、窗口基础配置（不变） ***************************/
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground); // 主窗口透明，允许背景穿透
    setFixedSize(850, 575);
    setWindowIcon(QIcon(":/头像2"));

    /*************************** 2、初始化辅助背景控件（关键：显式 show + 层级控制） ***************************/
    // 2.1 主窗口背景（最底层，先创建+显示）
    mainBackdrop = new Widget_Assist_Backdrop(this);
    mainBackdrop->setFixedSize(850, 575);       // 与主窗口尺寸完全匹配
    mainBackdrop->set_color(QColor(246, 250, 254)); // 浅蓝背景
    mainBackdrop->show();                       // 显式调用 show() 确保显示
    mainBackdrop->lower();                      // 强制置于最底层

    // 2.2 左侧菜单背景（主背景之上，左侧容器之下）
    leftMenuBackdrop = new Widget_Assist_Menu(this);
    leftMenuBackdrop->setFixedSize(180, 532);  // 与左侧菜单容器尺寸匹配
    leftMenuBackdrop->move(10, 20);             // 与左侧菜单容器位置一致
    leftMenuBackdrop->show();                   // 显式调用 show()
    leftMenuBackdrop->raise();
    mainBackdrop->lower();     // 确保 mainBackdrop 在最底层                  // 置于主背景之上、左侧容器之下

    /*************************** 3、顶部控件（关闭按钮 + 标题） ***************************/
    // 3.1 关闭按钮（层级：置顶）
    butto_closr = new Button_Main_Close(this);
    butto_closr->move(width() - 20, 5);
    butto_closr->raise(); // 置顶，确保点击有效
    connect(butto_closr, &Button_Main_Close::signals_click, this, &Widget_Main::slots_close_window);

    // 3.2 标题标签（透明背景，不遮挡主背景）
    Window_Title = new QLabel(this);
    Window_Title->setText("Art Vision");
    Window_Title->setFont(QFont("华文隶书", 13));
    Window_Title->setAlignment(Qt::AlignCenter);
    Window_Title->setFixedSize(150, 30);
    Window_Title->move((width() / 2) - (Window_Title->width() / 2), -2);
    Window_Title->setStyleSheet("background-color: transparent;"); // 透明背景
    Window_Title->raise();

    /*************************** 4、左侧菜单容器（透明背景，显示左侧菜单背景） ***************************/
    leftWidget = new QWidget(this);
    leftWidget->setFixedSize(130, 532);
    leftWidget->move(10, 20);
    leftWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false); // 允许鼠标事件
    leftWidget->setStyleSheet("background-color: transparent;");       // 透明背景
    leftWidget->raise(); // 置于左侧菜单背景之上


    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(15, 20, 15, 20);
    leftLayout->setSpacing(18);

    // 4.1 头像标签（透明背景）
    label_head_img = new QLabel(leftWidget);
    label_head_img->setPixmap(QPixmap(":/头像2"));
    label_head_img->setScaledContents(true);
    label_head_img->setFixedSize(80, 80);
    label_head_img->setStyleSheet("background-color: transparent;"); // 透明背景
    leftLayout->addWidget(label_head_img, 0, Qt::AlignCenter);

    // 4.2 菜单按钮（自身绘图，不依赖背景透明度）
    label_smart_chat = new Button_Main_Menu(leftWidget);
    label_smart_chat->Set_Test("智能对话");
    label_smart_chat->Set_Image(":/智能对话");
    leftLayout->addWidget(label_smart_chat);

    label_vision_recog = new Button_Main_Menu(leftWidget);
    label_vision_recog->Set_Test("视觉识别");
    label_vision_recog->Set_Image(":/视觉识别");
    leftLayout->addWidget(label_vision_recog);

    label_smart_home = new Button_Main_Menu(leftWidget);
    label_smart_home->Set_Test("智能家居");
    label_smart_home->Set_Image(":/智能家居");
    leftLayout->addWidget(label_smart_home);

    label_weather = new Button_Main_Menu(leftWidget);
    label_weather->Set_Test("天气预报");
    label_weather->Set_Image(":/天气预报");
    leftLayout->addWidget(label_weather);

    label_control_screen = new Button_Main_Menu(leftWidget);
    label_control_screen->Set_Test("工控大屏");
    label_control_screen->Set_Image(":/工控大屏");
    leftLayout->addWidget(label_control_screen);

    label_motor = new Button_Main_Menu(leftWidget);
    label_motor->Set_Test("电机控制");
    label_motor->Set_Image(":/电机控制");
    leftLayout->addWidget(label_motor);

    leftLayout->addStretch();

    /*************************** 5、右侧页面容器（透明背景，显示主背景） ***************************/
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setFixedSize(650, 530);
    stackedWidget->move(200, 20);
    stackedWidget->setStyleSheet("background-color: transparent;"); // 透明背景
    stackedWidget->raise();

    /*************************** 6、功能页面初始化（透明背景，显示主背景） ***************************/
    Widget_Smart_chat = new SmartDialogWidget(stackedWidget);
    Widget_Smart_chat->setStyleSheet("background-color: transparent;"); // 页面透明
    Widget_Smart_vision = new VisualRecogWidget(stackedWidget);
    Widget_Smart_vision->setStyleSheet("background-color: transparent;");
    Widget_Smart_Home = new SmartHomeWidget(stackedWidget);
    Widget_Smart_Home->setStyleSheet("background-color: transparent;");
    Widget_Weather = new WeatherWidget(stackedWidget);
    Widget_Weather->setStyleSheet("background-color: transparent;");
    Widget_Control_industrial = new IndustryWidget(stackedWidget);
    Widget_Control_industrial->setStyleSheet("background-color: transparent;");
    Widget_Control_motor = new MotorControlWidget(stackedWidget);
    Widget_Control_motor->setStyleSheet("background-color: transparent;");

    stackedWidget->addWidget(Widget_Smart_chat);
    stackedWidget->addWidget(Widget_Smart_vision);
    stackedWidget->addWidget(Widget_Smart_Home);
    stackedWidget->addWidget(Widget_Weather);
    stackedWidget->addWidget(Widget_Control_industrial);
    stackedWidget->addWidget(Widget_Control_motor);

    const int pageW = 650, pageH = 530;
    Widget_Smart_chat->setFixedSize(pageW, pageH);
    Widget_Smart_vision->setFixedSize(pageW, pageH);
    Widget_Smart_Home->setFixedSize(pageW, pageH);
    Widget_Weather->setFixedSize(pageW, pageH);
    Widget_Control_industrial->setFixedSize(pageW, pageH);
    Widget_Control_motor->setFixedSize(pageW, pageH);

    /*************************** 7、菜单信号连接与默认状态 ***************************/
    connect(label_smart_chat, &Button_Main_Menu::signals_click, this, &Widget_Main::slots_click_menu);
    connect(label_vision_recog, &Button_Main_Menu::signals_click, this, &Widget_Main::slots_click_menu);
    connect(label_smart_home, &Button_Main_Menu::signals_click, this, &Widget_Main::slots_click_menu);
    connect(label_weather, &Button_Main_Menu::signals_click, this, &Widget_Main::slots_click_menu);
    connect(label_control_screen, &Button_Main_Menu::signals_click, this, &Widget_Main::slots_click_menu);
    connect(label_motor, &Button_Main_Menu::signals_click, this, &Widget_Main::slots_click_menu);

    stackedWidget->setCurrentIndex(PAGE_SMART_CHAT);
    label_smart_chat->update_start(Mode_Press); // 选中态设为 Mode_Press
    // 主窗口构造函数中，添加调试打印
    qDebug() << "leftMenuBackdrop geometry:" << leftMenuBackdrop->geometry();
    qDebug() << "leftWidget geometry:" << leftWidget->geometry();
}

Widget_Main::~Widget_Main()
{
}

void Widget_Main::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    /*************************** 绘制背景 ***************************/
    // 背景为浅蓝色
    QPainter painter(this);

    // 开启抗锯齿
    painter.setRenderHint(QPainter::Antialiasing);

    // 设置画笔  #E4EEFF QColor(228 , 238 , 255)  1
    QPen pen;
    pen.setColor(QColor(246, 250, 254));
    pen.setWidth(0);

    // 设置画刷
    QBrush Brush;
    Brush.setColor(QColor(246, 250, 254));
    Brush.setStyle(Qt::SolidPattern);

    // 设定画笔 画刷
    painter.setPen(pen);
    painter.setBrush(Brush);

    // 绘制图形
    painter.drawRoundedRect(this->rect(), 12, 12);
}

void Widget_Main::mousePressEvent(QMouseEvent* event)
{
//    if (event->button() == Qt::LeftButton)
//    {
//        // 定义「左侧菜单区域」的矩形（与 leftWidget 位置/尺寸一致）
//        QRect leftMenuRect(10, 20, 130, 532);
//        if (leftMenuRect.contains(event->pos())) {
//            // 点击在左侧菜单内 → 不启动窗口拖动，直接返回
//            return;
//        }

//        // 非菜单区域：判断是否点击标题
//        QPoint labelPos = Window_Title->mapFromParent(event->pos());
//        if (Window_Title->rect().contains(labelPos)) {
//            Window_Title->setText("今天也要开心鸭");
//        } else {
//            // 记录拖动起始位置（仅非菜单、非标题区域才启动拖动）
//            Mouse_Press_Pos = event->pos();
//        }
//    }
//    QWidget::mousePressEvent(event);

if (event->button() != Qt::LeftButton) {
    QWidget::mousePressEvent(event);
    return;
}

// 1. 定义所有“不可拖动区域”
QRect leftMenuRect(10, 20, 130, 532);       // 左侧菜单区域
QRect closeBtnRect(width() - 20, 5, 15, 15); // 关闭按钮区域（尺寸与按钮一致）
QRect titleRect((width()/2)-(150/2), -2, 150, 30); // 标题区域

// 2. 若点击在“不可拖动区域”，直接返回，不记录拖动起始位置
if (leftMenuRect.contains(event->pos())
        || closeBtnRect.contains(event->pos())
        || titleRect.contains(event->pos())) {
    return;
}

// 3. 仅在“可拖动区域”记录起始位置，并处理标题文本变化
QPoint labelPos = Window_Title->mapFromParent(event->pos());
if (titleRect.contains(event->pos())) { // 标题区域虽不可拖动，但仍处理文本变化
    Window_Title->setText("今天也要开心鸭");
}

// 记录拖动起始位置（仅可拖动区域）
Mouse_Press_Pos = event->pos();
QWidget::mousePressEvent(event);
}

void Widget_Main::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint labelPos = Window_Title->mapFromParent(event->pos());
        if (Window_Title->rect().contains(labelPos)) {
            Window_Title->setText("Echo Vision");
        }

        // 关键：释放鼠标时，重置拖动起始位置
        Mouse_Press_Pos = QPoint(-1, -1);
    }
    QWidget::mouseReleaseEvent(event);
}

void Widget_Main::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint movePos = event->globalPos() - Mouse_Press_Pos;
        move(movePos);
    }
}

void Widget_Main::slots_click_menu()
{
    QObject* senderObj = sender();

    // 智能对话
    if (senderObj != label_smart_chat)
    {
        label_smart_chat->update_start(Mode_leave);
    }
    else
    {
        stackedWidget->setCurrentIndex(PAGE_SMART_CHAT);
        label_smart_chat->update_start(Mode_Press);
    }

    // 视觉识别
    if (senderObj != label_vision_recog)
    {
        label_vision_recog->update_start(Mode_leave);
    }
    else
    {
        stackedWidget->setCurrentIndex(PAGE_VISION_RECOG);
        label_vision_recog->update_start(Mode_Press);
    }

    // 智能家居
    if (senderObj != label_smart_home)
    {
        label_smart_home->update_start(Mode_leave);
    }
    else
    {
        stackedWidget->setCurrentIndex(PAGE_SMART_HOME);
        label_smart_home->update_start(Mode_Press);
    }

    // 天气预报
    if (senderObj != label_weather)
    {
        label_weather->update_start(Mode_leave);
    }
    else
    {
        stackedWidget->setCurrentIndex(PAGE_WEATHER);
        label_weather->update_start(Mode_Press);
    }

    // 工控大屏
    if (senderObj != label_control_screen)
    {
        label_control_screen->update_start(Mode_leave);
    }
    else
    {
        stackedWidget->setCurrentIndex(PAGE_CONTROL_SCREEN);
        label_control_screen->update_start(Mode_Press);
    }

    // 电机控制
    if (senderObj != label_motor)
    {
        label_motor->update_start(Mode_leave);
    }
    else
    {
        stackedWidget->setCurrentIndex(PAGE_MOTOR);
        label_motor->update_start(Mode_Press);
    }
}

void Widget_Main::slots_close_window()
{
    close();
}
