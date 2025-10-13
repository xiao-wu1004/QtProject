#include "smarthomewidget.h"
#include "musicsearchdialog.h"
#include <QDebug>

SmartHomeWidget::SmartHomeWidget(QWidget* parent) : QWidget(parent),
    searchDialog(nullptr) // 初始化为空，标识“无对话框”

{
    /*************************** 1、窗口基础配置（匹配 XML） ***************************/
//    setFixedSize(696, 532);
//    setMinimumSize(696, 532);
//    setMaximumSize(696, 532);
    setFixedSize(650, 530);   // 匹配 stackedWidget 的尺寸
    setMinimumSize(650, 530);
    setMaximumSize(650, 530);
    setWindowTitle("Form");

    // 初始化设备状态（默认全关）
    deviceStates = {
        {"home", false}, {"leave", false}, {"sleep", false}, {"wake", false},
        {"light", false}, {"air", false}, {"tv", false}, {"curtain", false},
        {"humid", false}, {"wifi", false}
    };
    /*************************** 2、主垂直布局（对应 XML: verticalLayout_3） ***************************/
    mainVLayout = new QVBoxLayout(this);
    mainVLayout->setContentsMargins(0, 0, 0, 0); // 边距：左=上=右=下=0
    mainVLayout->setSpacing(0);                  // 布局内控件间距=0
    // 新增：设置主布局左上对齐，所有内容靠左上方排列
    mainVLayout->setAlignment(Qt::AlignLeft );

    /*************************** 3、构建第一行（对应 XML: widget_12） ***************************/
    widget12 = new QWidget(this);
    widget12->setMaximumSize(INT_MAX, 100); // 最大高度=100（XML: maximumSize.height=100）
    widget12HLayout = new QHBoxLayout(widget12);
    widget12HLayout->setContentsMargins(0, 0, 0, 0); // 边距=0
    widget12HLayout->setSpacing(6);                  // 间距=6（XML: horizontalLayout_2.spacing=6）

    returnHomeOff.load(":/不回家");
    returnHomeOn.load(":/回家");
    leaveHomeOff.load(":/不离家");
    leaveHomeOn.load(":/离家");
    sleepOff.load(":/不睡觉");
    sleepOn.load(":/睡觉");
    wakeOff.load(":/不起床");
    wakeOn.load(":/起床");
    // 新增：六个家居的关/开图片（替换为实际资源路径）
    wifiOff.load(":/img/wifi_off.png");       // WiFi关状态图片
    wifiOn.load(":/img/wifi_on.png");         // WiFi开状态图片
    airOff.load(":/img/air_off.png");         // 空调关状态图片
    airOn.load(":/img/air_on.png");           // 空调开状态图片
    lightOff.load(":/img/light_off.png");     // 灯光关状态图片
    lightOn.load(":/img/light_on.png");       // 灯光开状态图片
    tvOff.load(":/img/tv_off.png");           // 电视关状态图片
    tvOn.load(":/img/tv_on.png");             // 电视开状态图片
    curtainOff.load(":/img/curtain_off.png"); // 窗帘关状态图片
    curtainOn.load(":/img/curtain_on.png");   // 窗帘开状态图片
    humidifierOff.load(":/img/humidifier_off.png"); // 加湿器关状态图片
    humidifierOn.load(":/img/humidifier_on.png");   // 加湿器开状态图片

    // 3.1 背景控件：widget_return_home（XML: widget_return_home）
    backdropReturnHome = new Widget_Assist_Backdrop(widget12);
    backdropReturnHome->setMinimumSize(120, 80); // 最小尺寸：120x80
    backdropReturnHome->setMaximumSize(120, 90); // 最大尺寸：120x90
    //backdropReturnHome->setLabelText("widget_return_home"); // 设置标签

    QLabel* returnHomeText = new QLabel("归家", backdropReturnHome);
    returnHomeText->setGeometry(20, 10, 40, 20); // 位置：x20,y15（左侧），尺寸：40x20
    returnHomeText->setFont(QFont("SimHei", 10)); // 字体：黑体10号
    returnHomeText->setAlignment(Qt::AlignCenter); // 文本居中
    returnHomeText->setStyleSheet("color: #333;"); // 文字颜色（可调整）

    returnHomePic = new QLabel(backdropReturnHome);
    returnHomePic->setGeometry(20, 30, 40, 40); // 图片位置（可根据布局调整）
    returnHomePic->setPixmap(returnHomeOff.scaled(80, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    returnHomePic->setScaledContents(true);     // 图片自适应标签

    btnReturnHome = new HomeModeLabel(
        ":/状态开关_关",  // 关状态图片
        ":/状态开关_开",  // 开状态图片
        this // 父部件：背景控件
    );
    btnReturnHome->setGeometry(70, 10, 40, 40);  // 位置：x70,y10；尺寸：40x40
    btnReturnHome->setMinimumSize(40, 40);
    btnReturnHome->setMaximumSize(40, 40);

    btnReturnHome->raise(); // 确保按钮可点击
    widget12HLayout->addWidget(backdropReturnHome);

    // 3.2 背景控件：widget_run_home（XML: widget_run_home）
    backdropRunHome = new Widget_Assist_Backdrop(widget12);
    backdropRunHome->setMinimumSize(120, 90); // 最小/最大尺寸：120x90
    backdropRunHome->setMaximumSize(120, 90);
    //backdropRunHome->setLabelText("widget_run_home"); // 设置标签

    QLabel* leaveHomeText = new QLabel("离家", backdropRunHome);
    leaveHomeText->setGeometry(20, 10, 40, 20); // 与“归家”标签位置一致
    leaveHomeText->setFont(QFont("SimHei", 10));
    leaveHomeText->setAlignment(Qt::AlignCenter);
    leaveHomeText->setStyleSheet("color: #333;");

    leaveHomePic = new QLabel(backdropRunHome);
    leaveHomePic->setGeometry(20, 30, 40, 40);
    leaveHomePic->setPixmap(leaveHomeOff.scaled(80, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    leaveHomePic->setScaledContents(true);
    btnRunHome = new HomeModeLabel(
        ":/状态开关_关",  // 关状态图片
        ":/状态开关_开",  // 开状态图片
        this  // 父部件：背景控件
    );
    btnRunHome->setGeometry(196, 10, 40, 40);  // 位置：x196,y10；尺寸：40x40
    btnRunHome->setMinimumSize(40, 40);
    btnRunHome->setMaximumSize(40, 40);

    btnRunHome->raise();
    widget12HLayout->addWidget(backdropRunHome);

    // 3.3 背景控件：widget_14（XML: widget_14）
    backdrop14 = new Widget_Assist_Backdrop(widget12);
    backdrop14->setMinimumSize(120, 90);
    backdrop14->setMaximumSize(120, 90);
    //backdrop14->setLabelText("widget_sleep"); // 设置标签

    QLabel* sleepText = new QLabel("睡觉", backdrop14);
    sleepText->setGeometry(20, 10, 40, 20); // 位置与其他标签对齐
    sleepText->setFont(QFont("SimHei", 10));
    sleepText->setAlignment(Qt::AlignCenter);
    sleepText->setStyleSheet("color: #333;");

    sleepPic = new QLabel(backdrop14);
    sleepPic->setGeometry(20, 30, 40, 40);
    sleepPic->setPixmap(sleepOff.scaled(80, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    sleepPic->setScaledContents(true);

    btnSleep = new HomeModeLabel(
        ":/状态开关_关",  // 睡觉“关”状态图片（需替换为实际资源路径）
        ":/状态开关_开",   // 睡觉“开”状态图片（需替换为实际资源路径）
        this
        );
    btnSleep->setGeometry(322, 10, 40, 40); // 按钮位置（与其他按钮对齐）
    btnSleep->setMinimumSize(40, 40);
    btnSleep->setMaximumSize(40, 40);
    btnSleep->raise(); // 确保按钮在最上层（可点击）
    widget12HLayout->addWidget(backdrop14);

    // 3.4 背景控件：widget_16（XML: widget_16）
    backdrop16 = new Widget_Assist_Backdrop(widget12);
    backdrop16->setMinimumSize(120, 90);
    backdrop16->setMaximumSize(120, 90);
    //backdrop16->setLabelText("widget_wake");

    QLabel* wakeText = new QLabel("起床", backdrop16);
    wakeText->setGeometry(20, 10, 40, 20); // 与其他标签位置统一
    wakeText->setFont(QFont("SimHei", 10));
    wakeText->setAlignment(Qt::AlignCenter);
    wakeText->setStyleSheet("color: #333;");

    wakeUpPic = new QLabel(backdrop16);
    wakeUpPic->setGeometry(20, 30, 40, 40);
    wakeUpPic->setPixmap(wakeOff.scaled(80, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    wakeUpPic->setScaledContents(true);
    btnWakeUp = new HomeModeLabel(
        ":/状态开关_关",  // 睡觉“关”状态图片（需替换为实际资源路径）
        ":/状态开关_开",   // 睡觉“开”状态图片（需替换为实际资源路径）
        this
        );
    btnWakeUp->setGeometry(448, 10, 40, 40); // 按钮位置（与其他按钮对齐）
    btnWakeUp->setMinimumSize(40, 40);
    btnWakeUp->setMaximumSize(40, 40);
    btnWakeUp->raise();
    widget12HLayout->addWidget(backdrop16);

    // 3.5 水平 spacer（XML: horizontalSpacer）
    //topHorizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    // 新代码：Expanding 类型（仅填充剩余空间，不强制占宽，布局自动左移）
    topHorizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    widget12HLayout->addItem(topHorizontalSpacer);

    mainVLayout->addWidget(widget12);


    /*************************** 4、构建第二行（对应 XML: widget_11） ***************************/
    widget11 = new QWidget(this);
    widget11HLayout = new QHBoxLayout(widget11);
    widget11HLayout->setContentsMargins(0, 0, 0, 0); // 边距=0
    widget11HLayout->setSpacing(6);                  // 间距=6（XML: horizontalLayout.spacing=6）
    // 新增：设置第二行水平布局左对齐
    widget11HLayout->setAlignment(Qt::AlignLeft);

    // ---------- 4.1 第二行左侧：widget_10（XML: widget_10） ----------
    widget10 = new QWidget(widget11);
    widget10->setMinimumSize(430, 0); // 最小宽度=430（XML: minimumSize.width=430）
    widget10VLayout = new QVBoxLayout(widget10);
    widget10VLayout->setContentsMargins(0, 0, 0, 0); // 边距=0
    widget10VLayout->setSpacing(0);                  // 间距=0（XML: verticalLayout_2.spacing=0）

    // ~ 4.1.1 背景控件：widget_9（XML: widget_9） ~
    backdrop9 = new QWidget(widget10);
    //backdrop9->setLabelText("widget_9"); // 设置标签
    backdrop9->setStyleSheet(
        "background-color: #CCEBFF; "       // 浅蓝色背景
        "border-radius: 15px; "              // 圆角
        "border: none;"        // 边框
        );
    // 主垂直布局：控制整体上下排列
    QVBoxLayout* tcpVLayout = new QVBoxLayout(backdrop9);
    tcpVLayout->setContentsMargins(15, 15, 15, 15); // 内边距
    tcpVLayout->setSpacing(10);                   // 控件间距


    // ---------- 第一行：服务器IP + 端口（横向排列） ----------
    QHBoxLayout* ipPortRow = new QHBoxLayout();
    ipPortRow->setSpacing(15); // IP与端口列的间距

    QLabel* ipLabel = new QLabel("服务器IP：", backdrop9);
    ipLabel->setFont(QFont("SimHei", 10));

    tcpServerIPEdit = new QLineEdit("192.168.50.95", backdrop9);
    tcpServerIPEdit->setFont(QFont("SimHei", 10));
    tcpServerIPEdit->setStyleSheet(
        "padding: 5px; border: 1px solid #999; border-radius: 3px;"
        "background-color: white; color: #333;"
        );
    tcpServerIPEdit->setMinimumWidth(150); // 确保IP输入宽度

    QLabel* portLabel = new QLabel("端口：", backdrop9);
    portLabel->setFont(QFont("SimHei", 10));

    tcpPortEdit = new QLineEdit("8888", backdrop9);
    tcpPortEdit->setFont(QFont("SimHei", 10));
    tcpPortEdit->setStyleSheet(
        "padding: 5px; border: 1px solid #999; border-radius: 3px;"
        "background-color: white; color: #333;"
        );
    tcpPortEdit->setValidator(new QIntValidator(1, 65535, this));
    tcpPortEdit->setMaximumWidth(80); // 端口输入宽度适中

    ipPortRow->addWidget(ipLabel);
    ipPortRow->addWidget(tcpServerIPEdit);
    ipPortRow->addWidget(portLabel);
    ipPortRow->addWidget(tcpPortEdit);
    tcpVLayout->addLayout(ipPortRow);


    // ---------- 第二行：连接按钮 + 状态标签（横向排列，状态居右） ----------
    QHBoxLayout* connectRow = new QHBoxLayout();
    connectRow->setSpacing(15);

    tcpConnectBtn = new QPushButton("连接", backdrop9);
    tcpConnectBtn->setFont(QFont("SimHei", 10));
    tcpConnectBtn->setStyleSheet(
        "background-color: #4CAF50; color: white; padding: 6px 12px; border-radius: 3px;"
        "hover { background-color: #45a049; }"
        "border: none;"
        );
    tcpConnectBtn->setMinimumSize(150, 30); // 按钮大小适中

    tcpStatusLabel = new QLabel("状态：未连接", backdrop9);
    tcpStatusLabel->setFont(QFont("SimHei", 10));
    tcpStatusLabel->setStyleSheet("color: #E74C3C; background-color: rgba(0,0,0,0);");
    tcpStatusLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // 用Spacer将“状态标签”推到右侧
    connectRow->addWidget(tcpConnectBtn);
    connectRow->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    connectRow->addWidget(tcpStatusLabel);
    tcpVLayout->addLayout(connectRow);


    // ---------- 第三行：发送输入框 + 发送按钮（横向排列，输入框占比大） ----------
    QHBoxLayout* sendRow = new QHBoxLayout();
    sendRow->setSpacing(10);

    tcpSendEdit = new QLineEdit(backdrop9);
    tcpSendEdit->setFont(QFont("SimHei", 10));
    tcpSendEdit->setPlaceholderText("输入消息...");
    tcpSendEdit->setStyleSheet(
        "padding: 5px; border: 1px solid #999; border-radius: 3px;"
        "background-color: white; color: #333;"
        );
    tcpSendEdit->setEnabled(false); // 未连接时禁用
    tcpSendEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 输入框占满水平空间
    tcpSendEdit->setMinimumHeight(30);

    tcpSendBtn = new QPushButton("发送", backdrop9);
    tcpSendBtn->setFont(QFont("SimHei", 10));
    tcpSendBtn->setStyleSheet(
        "background-color: #2196F3; color: white; padding: 6px 15px; border-radius: 3px;"
        "hover { background-color: #1976D2; }"
        "border: none;"
        );
    tcpSendBtn->setEnabled(false); // 未连接时禁用
    tcpSendBtn->setMinimumSize(60, 30);

    sendRow->addWidget(tcpSendEdit);
    sendRow->addWidget(tcpSendBtn);
    tcpVLayout->addLayout(sendRow);


    // ---------- 第四行：消息接收区（标签 + 文本框） ----------
    QLabel* recvLabel = new QLabel("消息接收区", backdrop9);
    recvLabel->setFont(QFont("SimHei", 10, QFont::Medium));
    recvLabel->setStyleSheet("color: #333; margin-top: 5px;");

    tcpVLayout->addWidget(recvLabel);

    tcpRecvText = new QTextEdit(backdrop9);
    tcpRecvText->setFont(QFont("SimHei", 10));
    tcpRecvText->setReadOnly(true);
    tcpRecvText->setStyleSheet(
        "border: 1px solid #999; border-radius: 3px; background-color: white;"
        "color: #333; padding: 8px;"
        );
    tcpRecvText->setMinimumHeight(80); // 足够显示多条消息
    tcpRecvText->setMaximumHeight(90); // 最大高度（高于此值会被压缩）
    tcpVLayout->addWidget(tcpRecvText);

    widget10VLayout->addWidget(backdrop9);

    // ~ 4.1.2 嵌套容器：widget（含 verticalLayout_4） ~
    widgetInner = new QWidget(widget10);
    widgetInnerVLayout = new QVBoxLayout(widgetInner);
    widgetInnerVLayout->setContentsMargins(0, 0, 0, 0); // 边距=0
    widgetInnerVLayout->setSpacing(0);                  // 间距=0（XML: verticalLayout_4.spacing=0）

    // ~~ widgetInner 内第一行：widget_2（XML: widget_2） ~~
    widget2 = new QWidget(widgetInner);
    widget2->setMaximumSize(INT_MAX, 90); // 最大高度=90（XML: maximumSize.height=90）
    widget2HLayout = new QHBoxLayout(widget2);
    widget2HLayout->setContentsMargins(0, 0, 0, 0); // 边距=0
    widget2HLayout->setSpacing(0);                  // 间距=0（XML: horizontalLayout_3.spacing=0）

    backdrop4 = new QWidget(widget2);
    backdrop4->setMinimumSize(120, 90);
    backdrop4->setMaximumSize(120, 90);
    backdrop4->setStyleSheet(
        "background-color: #CCEBFF; "       // 浅蓝色背景
        "border-radius: 15px; "              // 圆角
        "border: none;"        // 边框
        );
    //backdrop4->setLabelText("widget_4"); // 设置标签
    widget2HLayout->addWidget(backdrop4);

    // ====================== 在这里插入灯光控件的代码 ======================
    // 图标图片标签（初始显示“关”状态）
     lightPic = new QLabel(backdrop4);
    lightPic->setGeometry(20, 30, 40, 40);
    lightPic->setPixmap(lightOff.scaled(80, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 初始用lightOff
    lightPic->setScaledContents(true);

    // 文字标签（显示“灯光”）
    QLabel* lightText = new QLabel("灯光", backdrop4);
    lightText->setGeometry(20, 10, 40, 20); // 与“归家”文字标签位置一致
    lightText->setFont(QFont("SimHei", 10));
    lightText->setAlignment(Qt::AlignCenter);
    lightText->setStyleSheet("color: #333;");

    // 状态按钮（关→开切换）
    lightBtn = new HomeModeLabel(
        ":/状态开关_关",  // 开关按钮的关图标（复用现有资源）
        ":/状态开关_开",  // 开关按钮的开图标（复用现有资源）
        backdrop4
        );
    lightBtn->setGeometry(70, 10, 40, 40); // 与“归家”按钮位置一致
    lightBtn->setMinimumSize(40, 40);
    lightBtn->setMaximumSize(40, 40);
    lightBtn->raise(); // 确保按钮在最上层（可点击）

    // 连接按钮状态→图片切换
    connect(lightBtn, &HomeModeLabel::clicked, [this](bool isOn) {
        lightPic->setPixmap(isOn ? lightOn : lightOff); // 开→lightOn，关→lightOff
        qDebug() << "灯光状态：" << (isOn ? "开" : "关");
    });

    widget2Spacer1 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    widget2HLayout->addItem(widget2Spacer1);

    backdrop5 = new QWidget(widget2);
    backdrop5->setMinimumSize(120, 90);
    backdrop5->setMaximumSize(120, 90);
    backdrop5->setStyleSheet(
        "background-color: #CCEBFF; "
        "border-radius: 15px; "
        "border: none;"
        );
    //backdrop5->setLabelText("widget_5"); // 设置标签
    widget2HLayout->addWidget(backdrop5);

    QLabel* airText = new QLabel("空调", backdrop5);
    airText->setGeometry(20, 10, 40, 20); // 与其他文字标签位置对齐
    airText->setFont(QFont("SimHei", 10));
    airText->setAlignment(Qt::AlignCenter);
    airText->setStyleSheet("color: #333;");

    airPic = new QLabel(backdrop5);
    airPic->setGeometry(20, 30, 40, 40);
    airPic->setPixmap(airOff.scaled(80, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 初始airOff
    airPic->setScaledContents(true);

     airBtn = new HomeModeLabel(
        ":/状态开关_关",
        ":/状态开关_开",
        backdrop5
        );
    airBtn->setGeometry(70, 10, 40, 40);
    airBtn->raise();

    // 连接按钮状态→图片切换
    connect(airBtn, &HomeModeLabel::clicked, [this](bool isOn) {
        airPic->setPixmap(isOn ? airOn : airOff);
        qDebug() << "空调状态：" << (isOn ? "开" : "关");
    });

    widget2Spacer2 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    widget2HLayout->addItem(widget2Spacer2);

    backdrop6 = new QWidget(widget2);
    backdrop6->setMinimumSize(120, 90);
    backdrop6->setMaximumSize(120, 90);
    backdrop6->setStyleSheet(
        "background-color: #CCEBFF; "
        "border-radius: 15px; "
        "border: none;"
        );
    //backdrop6->setLabelText("widget_6"); // 设置标签
    widget2HLayout->addWidget(backdrop6);

    // ====================== 在这里插入电视控件的代码 ======================
    // 文字标签（显示“电视”）
    QLabel* tvText = new QLabel("电视", backdrop6);
    tvText->setGeometry(20, 10, 40, 20); // 与其他文字标签位置一致
    tvText->setFont(QFont("SimHei", 10));
    tvText->setAlignment(Qt::AlignCenter);
    tvText->setStyleSheet("color: #333;");

    // 图标图片标签（初始显示“关”状态）
    tvPic = new QLabel(backdrop6);
    tvPic->setGeometry(20, 30, 40, 40);
    tvPic->setPixmap(tvOff.scaled(80, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 初始用tvOff
    tvPic->setScaledContents(true);

    // 状态按钮（关→开切换）
     tvBtn = new HomeModeLabel(
        ":/状态开关_关",  // 复用开关图标
        ":/状态开关_开",
        backdrop6
        );
    tvBtn->setGeometry(70, 10, 40, 40); // 与其他按钮位置一致
    tvBtn->setMinimumSize(40, 40);
    tvBtn->setMaximumSize(40, 40);
    tvBtn->raise();

    // 连接按钮状态→图片切换
    connect(tvBtn, &HomeModeLabel::clicked, [this](bool isOn) {
        tvPic->setPixmap(isOn ? tvOn : tvOff);
        qDebug() << "电视状态：" << (isOn ? "开" : "关");
    });


    widgetInnerVLayout->addWidget(widget2);

    // ~~ widgetInner 内第二行：widget_3（XML: widget_3） ~~
    widget3 = new QWidget(widgetInner);
    widget3->setMinimumSize(0, 90);   // 最小高度=90（XML: minimumSize.height=90）
    widget3->setMaximumSize(INT_MAX, 90); // 最大高度=90（XML: maximumSize.height=90）
    widget3HLayout = new QHBoxLayout(widget3);
    widget3HLayout->setContentsMargins(0, 0, 0, 0); // 边距=0
    widget3HLayout->setSpacing(0);                  // 间距=0（XML: horizontalLayout_4.spacing=0）

    backdrop17 = new QWidget(widget3);
    backdrop17->setMinimumSize(120, 90);
    backdrop17->setMaximumSize(120, 90);
    backdrop17->setStyleSheet(
        "background-color: #CCEBFF; "
        "border-radius: 15px; "
        "border: none;"
        );
    //backdrop17->setLabelText("widget_17"); // 设置标签
    widget3HLayout->addWidget(backdrop17);

    // ====================== 在这里插入窗帘控件的代码 ======================
    // 文字标签（显示“窗帘”）
    QLabel* curtainText = new QLabel("窗帘", backdrop17);
    curtainText->setGeometry(20, 10, 40, 20);
    curtainText->setFont(QFont("SimHei", 10));
    curtainText->setAlignment(Qt::AlignCenter);
    curtainText->setStyleSheet("color: #333;");

    // 图标图片标签（初始显示“关”状态）
    curtainPic = new QLabel(backdrop17);
    curtainPic->setGeometry(20, 30, 40, 40);
    curtainPic->setPixmap(curtainOff.scaled(80, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 初始用curtainOff
    curtainPic->setScaledContents(true);

    // 状态按钮（关→开切换）
    curtainBtn = new HomeModeLabel(
        ":/状态开关_关",
        ":/状态开关_开",
        backdrop17
        );
    curtainBtn->setGeometry(70, 10, 40, 40);
    curtainBtn->setMinimumSize(40, 40);
    curtainBtn->setMaximumSize(40, 40);
    curtainBtn->raise();

    // 连接按钮状态→图片切换
    connect(curtainBtn, &HomeModeLabel::clicked, [this](bool isOn) {
        curtainPic->setPixmap(isOn ? curtainOn : curtainOff);
        qDebug() << "窗帘状态：" << (isOn ? "开" : "关");
    });

    widget3Spacer1 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    widget3HLayout->addItem(widget3Spacer1);

    backdrop18 = new QWidget(widget3);
    backdrop18->setMinimumSize(120, 90);
    backdrop18->setMaximumSize(120, 90);
    backdrop18->setStyleSheet(
        "background-color: #CCEBFF; "
        "border-radius: 15px; "
        "border: none;"
        );
    //backdrop18->setLabelText("widget_18"); // 设置标签
    widget3HLayout->addWidget(backdrop18);

    // ====================== 在这里插入加湿器控件的代码 ======================
    // 文字标签（显示“加湿器”）
    QLabel* humidifierText = new QLabel("加湿器", backdrop18);
    humidifierText->setGeometry(15, 15, 50, 20); // 文字稍长，宽度略调
    humidifierText->setFont(QFont("SimHei", 10));
    humidifierText->setAlignment(Qt::AlignCenter);
    humidifierText->setStyleSheet("color: #333;");

    // 图标图片标签（初始显示“关”状态）
    humidifierPic = new QLabel(backdrop18);
    humidifierPic->setGeometry(20, 30, 40, 40);
    humidifierPic->setPixmap(humidifierOff.scaled(80, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 初始用humidifierOff
    humidifierPic->setScaledContents(true);

    // 状态按钮（关→开切换）
    humidifierBtn = new HomeModeLabel(
        ":/状态开关_关",
        ":/状态开关_开",
        backdrop18
        );
    humidifierBtn->setGeometry(70, 10, 40, 40);
    humidifierBtn->setMinimumSize(40, 40);
    humidifierBtn->setMaximumSize(40, 40);
    humidifierBtn->raise();

    // 连接按钮状态→图片切换
    connect(humidifierBtn, &HomeModeLabel::clicked, [this](bool isOn) {
        humidifierPic->setPixmap(isOn ? humidifierOn : humidifierOff);
        qDebug() << "加湿器状态：" << (isOn ? "开" : "关");
    });

    widget3Spacer2 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    widget3HLayout->addItem(widget3Spacer2);

    backdrop19 = new QWidget(widget3);
    backdrop19->setMinimumSize(120, 90);
    backdrop19->setMaximumSize(120, 90);
    backdrop19->setStyleSheet(
        "background-color: #CCEBFF; "
        "border-radius: 15px; "
        "border: none;"
        );
    //backdrop19->setLabelText("widget_19"); // 设置标签
    widget3HLayout->addWidget(backdrop19);

    // ====================== 在这里插入WiFi控件的代码 ======================
    // 文字标签（显示“WiFi”）
    QLabel* wifiText = new QLabel("WiFi", backdrop19);
    wifiText->setGeometry(20, 10, 40, 20);
    wifiText->setFont(QFont("SimHei", 10));
    wifiText->setAlignment(Qt::AlignCenter);
    wifiText->setStyleSheet("color: #333;");

    // 图标图片标签（初始显示“关”状态）
     wifiPic = new QLabel(backdrop19);
    wifiPic->setGeometry(20, 30, 40, 40);
    wifiPic->setPixmap(wifiOff.scaled(80, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 初始用wifiOff
    wifiPic->setScaledContents(true);

    // 状态按钮（关→开切换）
     wifiBtn = new HomeModeLabel(
        ":/状态开关_关",
        ":/状态开关_开",
        backdrop19
        );
    wifiBtn->setGeometry(70, 10, 40, 40);
    wifiBtn->setMinimumSize(40, 40);
    wifiBtn->setMaximumSize(40, 40);
    wifiBtn->raise();

    // 连接按钮状态→图片切换
    connect(wifiBtn, &HomeModeLabel::clicked, [this](bool isOn) {
        wifiPic->setPixmap(isOn ? wifiOn : wifiOff);
        qDebug() << "WiFi状态：" << (isOn ? "开" : "关");
    });

    widgetInnerVLayout->addWidget(widget3);
    widget10VLayout->addWidget(widgetInner);
    widget11HLayout->addWidget(widget10);

    // ---------- 4.2 第二行右侧：widget_7（XML: widget_7） ----------
    widget7 = new QWidget(widget11);
    widget7->setMinimumSize(240, 0);   // 最小宽度=240（XML: minimumSize.width=240）
    widget7->setMaximumSize(240, INT_MAX); // 最大宽度=240（XML: maximumSize.width=240）
    widget7VLayout = new QVBoxLayout(widget7);
    widget7VLayout->setContentsMargins(25, 0, 10, 10); // 边距=0
    widget7VLayout->setSpacing(5);                  // 间距=0（XML: verticalLayout.spacing=0）

    backdrop8 = new QWidget(widget7);
    //backdrop8->setLabelText("widget_8"); // 设置标签
    backdrop8->setStyleSheet(
        "background-color: #CCEBFF; "       // 浅蓝色背景
        "border-radius: 15px; "              // 圆角
        "border: none;"        // 边框
        );
//    backdrop8->setStyleSheet(R"(
//    background: qradialgradient(cx:0.5, cy:0.5, radius:0.8,
//                               fx:0.5, fy:0.5,
//                               stop:0 #E6F7FF,  // 中心浅蓝
//                               stop:1 #CCEBFF); // 边缘原背景色
//    border-radius: 15px;
//    box-shadow: 0 4px 8px rgba(0,0,0,0.1); // 外层阴影
//    border: 1px solid rgba(153,204,255,0.5); // 淡蓝边框
//)");

    // 给 backdrop8 设置固定大小（确保光碟有足够显示空间）
    backdrop8->setMinimumSize(200, 200);
    backdrop8->setMaximumSize(200, 200);

    // 2. 给 backdrop8 加垂直布局（设置布局居中）
    QVBoxLayout* cdLayout = new QVBoxLayout(backdrop8);
    cdLayout->setContentsMargins(0, 0, 0, 0);
    cdLayout->setSpacing(0);
    cdLayout->setAlignment(Qt::AlignCenter); // 布局内控件居中

    // 【1. 外层发光环形装饰（作为容器，包裹光碟）】
    QLabel* glowRing = new QLabel(backdrop8);
    glowRing->setObjectName("GlowRing");
    glowRing->setMinimumSize(180, 180);  // 外层环尺寸：180x180
    glowRing->setMaximumSize(180, 180);
    glowRing->setStyleSheet(R"(
    QLabel#GlowRing
    {
    border: 2px solid #7FDEFF; /* 浅蓝色边框（替换原橙色 #FF9800） */
    border-radius: 90px;
    box-shadow: 0 0 15px rgba(127, 222, 255, 0.7), /* 外发光（浅蓝色透明） */
                inset 0 0 10px rgba(127, 222, 255, 0.8); /* 内发光（浅蓝色透明） */
    background-color: transparent;
    margin: 0px;
    padding: 0px;
    }
    )");



    // 【2. 给发光环添加内部布局：确保光碟在环中心】
    QVBoxLayout* ringInnerLayout = new QVBoxLayout(glowRing);
    ringInnerLayout->setContentsMargins(15, 15, 15, 15);  // 内边距15：让光碟与环有间距
    ringInnerLayout->setSpacing(0);
    ringInnerLayout->setAlignment(Qt::AlignCenter);  // 内部控件（光碟）居中

    // 【3. 音乐光碟图片（作为发光环的子控件，实现“嵌入”）】
    cdLabel = new RotatableLabel(glowRing);  // 关键：父控件设为glowRing，而非backdrop8
    cdLabel->setMinimumSize(150, 150);      // 光碟尺寸：150x150（比环小30，留边框空间）
    cdLabel->setMaximumSize(150, 150);
    cdLabel->setStyleSheet(R"(
    QLabel
    {
    image-rendering: smooth;
    background: transparent;
    box-shadow: 0 2px 5px rgba(0,0,0,0.2); /* 光碟自身阴影 */
    border-radius: 75px; /* 半径=光碟尺寸的1/2（150/2=75）→ 正圆形 */
    margin: 0px; /* 清除额外边距（关键4：避免光碟偏移） */
    padding: 0px;
    }
    )");
    cdLabel->setAlignment(Qt::AlignCenter);

    // 【在创建 cdLabel 后，添加反射层与缩放动画】
    // 1. 反射层1：中心高光点（模拟CD中心的强反射点）
    QLabel* centerHighlight = new QLabel(cdLabel); // 父控件设为cdLabel，随光碟旋转
    // 高光点尺寸：20x20（光碟的1/7.5，大小适中不突兀）
    centerHighlight->setFixedSize(20, 20);
    // 位置：固定在光碟中心（(150-20)/2 = 65，确保居中）
    centerHighlight->setGeometry(65, 65, 20, 20);
    // 样式：白色半透明+模糊边缘（模拟高光扩散效果）
    centerHighlight->setStyleSheet(R"(
    background-color: rgba(255, 255, 255, 0.9); /* 白色80%透明 */
    border-radius: 10px; /* 正圆形（20/2=10） */
)");
    // 给高光点加模糊效果（增强真实感，避免边缘生硬）
    QGraphicsDropShadowEffect* highlightBlur = new QGraphicsDropShadowEffect(centerHighlight);
    highlightBlur->setColor(QColor(255, 255, 255, 0.8)); // 模糊色：白色60%透明
    highlightBlur->setBlurRadius(8); // 模糊半径5px（轻微扩散）
    highlightBlur->setOffset(0, 0); // 居中无偏移
    centerHighlight->setGraphicsEffect(highlightBlur);


    // 2. 反射层2：环形渐变层（模拟CD表面的环形反射带）
    QLabel* ringReflect = new QLabel(cdLabel); // 父控件设为cdLabel，随光碟旋转
    // 环形尺寸：外直径120px，内直径80px（环形宽度20px，位于光碟中间层）
    ringReflect->setFixedSize(120, 120);
    // 位置：固定在光碟中心（(150-120)/2 = 15，确保居中）
    ringReflect->setGeometry(15, 15, 120, 120);
    // 样式：环形渐变（中心透明→边缘浅蓝白，适配背景色）
    ringReflect->setStyleSheet(R"(
    background: qradialgradient(
        cx:0.5, cy:0.5, radius:0.5,  /* 圆心在中心，半径占控件50% */
        fx:0.5, fy:0.5,              /* 渐变焦点在中心 */
        stop:0.33 rgba(255, 255, 255, 0),    /* 内圈（半径33%）：完全透明 */
        stop:0.5  rgba(255, 255, 255, 0.3),  /* 中圈（半径50%）：白色30%透明 */
        stop:0.66 rgba(255, 255, 255, 0.1),  /* 外圈（半径66%）：白色10%透明 */
        stop:1    rgba(255, 255, 255, 0)     /* 最外圈：完全透明 */
    );
    border-radius: 60px; /* 正圆形（120/2=60） */
)");

    // 3. 反射层3：顶部弱反射条（保留原顶部反射逻辑，降低透明度避免重复）
    QLabel* topReflect = new QLabel(cdLabel); // 父控件设为cdLabel，随光碟旋转
    // 尺寸：宽度150px（与光碟一致），高度20px（比原30px更窄，更自然）
    topReflect->setGeometry(0, 0, 150, 25);
    // 样式：顶部到中间的渐变（仅顶部有弱反射，模拟光线照射效果）
    topReflect->setStyleSheet(R"(
    background: qlineargradient(
        x1:0, y1:0, x2:0, y2:1,  /* 垂直渐变（从上到下） */
        stop:0 rgba(255, 255, 255, 0.5),  /* 顶部：白色20%透明 */
        stop:1 rgba(255, 255, 255, 0)     /* 底部：完全透明 */
    );
    border-top-left-radius: 75px;  /* 与光碟顶部圆角匹配（150/2=75） */
    border-top-right-radius: 75px;
)");
    // 给顶部反射条加轻微外发光，避免与光碟边缘融合
    QGraphicsDropShadowEffect* topReflectGlow = new QGraphicsDropShadowEffect(topReflect);
    topReflectGlow->setColor(QColor(255, 255, 255, 0.5));
    topReflectGlow->setBlurRadius(3);
    topReflectGlow->setOffset(0, 2); // 向下偏移2px，模拟光线照射
    topReflect->setGraphicsEffect(topReflectGlow);

    // 【新增：光碟整体弱反光层（增强全局光泽感）】
    QLabel* globalReflect = new QLabel(cdLabel);
    globalReflect->setGeometry(0, 0, 150, 150); // 覆盖整个光碟
    globalReflect->setStyleSheet(R"(
background: qradialgradient(
    cx:0.3, cy:0.3, radius:0.8,  /* 反光中心偏移（模拟光线从左上照射） */
    fx:0.3, fy:0.3,
    stop:0 rgba(255, 255, 255, 0.15),  /* 高光区15%透明 */
    stop:1 rgba(255, 255, 255, 0)
);
border-radius: 75px;
)");


    // 【在构造函数中，安装事件过滤器 + 绑定点击】
    glowRing->installEventFilter(this); // 让 SmartHomeWidget 捕获其事件
    glowRing->setCursor(Qt::PointingHandCursor); // 鼠标悬浮显示“手型”，提示可交互
    // 加载光碟图片
    QPixmap cdPixmap(":/img/cd.png");
    if (cdPixmap.isNull())
    {
        qWarning() << "[错误] 光碟图片加载失败！路径：:/img/cd.png";
    }
    else
    {
        // 按光碟控件尺寸缩放，保持圆形比例
        cdLabel->setPixmap(cdPixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    cdLabel->setScaledContents(false);
    cdLabel->setStyleSheet(R"(
    QLabel
    {
        image-rendering: smooth;
        background: transparent;
        box-shadow: 0 4px 5px rgba(0,0,0,0.2); /* 轻微阴影，增强立体感 */
        border-radius: 75px; /* 与光碟尺寸匹配（150/2=75），确保阴影是圆形 */
    }
    )");
    cdLabel->setAlignment(Qt::AlignCenter);

    // 【4. 把光碟加入发光环的内部布局（确保居中）】
    ringInnerLayout->addWidget(cdLabel);

    // 【5. 把发光环（带光碟）加入外层布局（整体居中）】
    cdLayout->addWidget(glowRing);  // 发光环作为整体居中

    // 【在创建 glowRing 后，添加阴影与呼吸动画】
    QGraphicsDropShadowEffect *glowEffect = new QGraphicsDropShadowEffect(glowRing);
    glowEffect->setColor(QColor(127, 222, 255, 180)); // 浅蓝色阴影（替换原橙色 #FF9800）
    glowEffect->setBlurRadius(15);
    glowEffect->setOffset(0, 0);
    glowRing->setGraphicsEffect(glowEffect);

    QPropertyAnimation *breathAnim = new QPropertyAnimation(glowEffect, "color");
    breathAnim->setDuration(2000);
    breathAnim->setStartValue(QColor(127, 222, 255, 100)); // 浅蓝色最暗（替换原橙色）
    breathAnim->setEndValue(QColor(127, 222, 255, 200));   // 浅蓝色最亮（替换原橙色）
    breathAnim->setEasingCurve(QEasingCurve::SineCurve);
    breathAnim->setLoopCount(-1);
    breathAnim->start();



    // 5. 光碟旋转动画（播放时启动，停止时复位）
    cdAnimation = new QPropertyAnimation(cdLabel, "rotation", this);
    cdAnimation->setDuration(5000); // 旋转速度：5秒一圈（数值越小越快）
    cdAnimation->setStartValue(0);  // 起始角度：0°（正位）
    cdAnimation->setEndValue(360);  // 结束角度：360°（一圈）
    cdAnimation->setLoopCount(-1);  // 循环模式：-1=无限循环
    cdAnimation->setEasingCurve(QEasingCurve::Linear); // 匀速旋转（无加速减速）

    // 2. 缩放动画：旋转时轻微放大，增强3D感
    QPropertyAnimation *scaleAnim = new QPropertyAnimation(cdLabel, "scale");
    scaleAnim->setDuration(5000); // 与旋转周期同步（5秒一圈）
    scaleAnim->setStartValue(1.0);
    scaleAnim->setEndValue(1.05); // 轻微放大（避免过度变形）
    scaleAnim->setLoopCount(-1);
    scaleAnim->setEasingCurve(QEasingCurve::Linear);

    // 3. 组合旋转 + 缩放动画
    cdGroupAnim = new QParallelAnimationGroup(this);
    cdGroupAnim->addAnimation(cdAnimation); // 原有的旋转动画
    cdGroupAnim->addAnimation(scaleAnim);
    //cdGroupAnim->start();

    widget7VLayout->addWidget(backdrop8);

    // ========== 加载音乐播放器图标（替换为实际资源路径） ==========
    musicIcon = QIcon(":/img/music.png");   // 顶部音乐图标
    playIcon = QIcon(":/img/play.png");     // 播放状态图标
    prevIcon = QIcon(":/img/prev.png");     // 上一曲图标
    pauseIcon = QIcon(":/img/pause.png");   // 暂停状态图标
    nextIcon = QIcon(":/img/next.png");     // 下一曲图标
    volumeIcon = QIcon(":/img/volume.png"); // 新增：音量图标（替换为实际资源路径）

    backdrop20 = new QWidget(widget7);
    //backdrop20->setLabelText("widget_20"); // 设置标签
    backdrop20->setMinimumSize(200, 180); // 确保内容有足够显示空间
    backdrop20->setStyleSheet(
        "background-color: #CCEBFF; "       // 浅蓝色背景
        "border-radius: 15px; "              // 圆角
        "border: none;"        // 边框（可选，增强视觉）
        );

    // ========== 创建音乐播放器内部布局 ==========
    QVBoxLayout *musicVLayout = new QVBoxLayout(backdrop20);
    musicVLayout->setContentsMargins(10, 10, 10, 10);
    musicVLayout->setSpacing(10);

    /*************************** 1. 顶部：音乐按钮 + 歌曲名 ***************************/
    QHBoxLayout *topHLayout = new QHBoxLayout();

    // 音乐按钮
    musicButton = new QPushButton(backdrop20);
    musicButton->setIcon(musicIcon);
    musicButton->setIconSize(QSize(64,64));
    musicButton->setStyleSheet("background-color: transparent; border: none;");

    // 歌曲名标签
    songNameLabel = new QLabel("唐人恋曲", backdrop20);
    songNameLabel->setFont(QFont("SimHei", 12));
    songNameLabel->setMaximumSize(100,30);
    songNameLabel->setStyleSheet("background-color: #AED6F1; padding: 5px 15px; border-radius: 15px;");

    topHLayout->addWidget(musicButton);
    topHLayout->addWidget(songNameLabel);
    topHLayout->addStretch(); // 右侧拉伸，使内容靠左
    musicVLayout->addLayout(topHLayout);

    /*************************** 2. 中间：歌词标签 ***************************/
    lyricLabel = new QLabel("弹一首七绝 话离别", backdrop20);
    QFont lyricLabelFont= initCustomFont(":/fonts/zCookHappy.ttf", 10);
    lyricLabel->setFont(QFont(lyricLabelFont));
    musicVLayout->addWidget(lyricLabel, 0, Qt::AlignCenter); // 水平居中


    /*************************** 3. 进度条（带左侧时间） ***************************/
    QHBoxLayout *progressWithTimeLayout = new QHBoxLayout(); // 水平布局：时间 + 进度条

    // 左侧：当前播放时间标签
    currentTimeLabel = new QLabel("00:00", backdrop20); // 初始显示 00:00
    currentTimeLabel->setFont(QFont("SimHei", 10));     // 字体与歌词标签一致
    currentTimeLabel->setStyleSheet("color: #333; background-color: transparent;"); // 透明背景
    currentTimeLabel->setFixedWidth(40);                // 固定宽度，避免文字晃动
    currentTimeLabel->setAlignment(Qt::AlignLeft);      // 文字左对齐

    progressBar = new QSlider(Qt::Horizontal, backdrop20);
    progressBar->setStyleSheet(
        "QSlider::groove:horizontal { height: 4px; background: #999; border-radius: 2px; }"
        "QSlider::handle:horizontal { width: 12px; margin: -4px 0; background: #555; border-radius: 6px; }"
        );
    progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 进度条占满剩余空间

    // 将时间标签和进度条加入水平布局
    progressWithTimeLayout->addWidget(currentTimeLabel);       // 左侧：时间
    progressWithTimeLayout->addWidget(progressBar);           // 右侧：进度条
    progressWithTimeLayout->setContentsMargins(0, 0, 0, 0);   // 消除布局边距
    progressWithTimeLayout->setSpacing(5);                    // 时间与进度条间距5px
    // 将整体布局加入播放器垂直布局
    musicVLayout->addLayout(progressWithTimeLayout);
    /*************************** 新增：音量调节（带图标） ***************************/
    QHBoxLayout *volumeLayout = new QHBoxLayout(); // 水平布局：图标 + 滑块

    // 音量图标标签
    volumeIconLabel = new QLabel(backdrop20);
    volumeIconLabel->setPixmap(volumeIcon.pixmap(QSize(20, 20))); // 设置图标大小
    volumeIconLabel->setAlignment(Qt::AlignCenter);              // 图标居中
    volumeIconLabel->setStyleSheet("background-color: transparent;"); // 透明背景，避免遮挡

    volumeSlider = new QSlider(Qt::Horizontal, backdrop20);
    volumeSlider->setRange(0, 100);   // 音量范围：0 ~ 100
    volumeSlider->setValue(50);       // 初始音量设为 50
    volumeSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 4px; background: #999; border-radius: 2px; }"
        "QSlider::handle:horizontal { width: 12px; margin: -4px 0; background: #555; border-radius: 6px; }"
        );
    volumeSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 滑块占满中间空间

    // 3. 新增：音量数值标签（显示百分比）
    volumeValueLabel = new QLabel("50%", backdrop20); // 初始显示 50%
    volumeValueLabel->setFont(QFont("SimHei", 10));   // 字体与时间标签一致
    volumeValueLabel->setStyleSheet("color: #333; background-color: transparent;");
    volumeValueLabel->setFixedWidth(30);              // 固定宽度，避免文字晃动
    volumeValueLabel->setAlignment(Qt::AlignRight);   // 文字右对齐

    // 将三个元素加入水平布局
    volumeLayout->addWidget(volumeIconLabel);    // 左：图标
    volumeLayout->addWidget(volumeSlider);      // 中：滑块（占满空间）
    volumeLayout->addWidget(volumeValueLabel);  // 右：数值
    volumeLayout->setContentsMargins(0, 0, 0, 0);
    volumeLayout->setSpacing(5);                 // 元素间距5px

    // 加入播放器垂直布局
    musicVLayout->addLayout(volumeLayout);
    /*************************** 4. 底部：控制按钮（上一曲、暂停、下一曲） ***************************/
    QHBoxLayout *ctrlHLayout = new QHBoxLayout();

    // 上一曲按钮（正确写法）
    prevBtn = new QPushButton(backdrop20);
    prevBtn->setIcon(prevIcon);                  // 用 setIcon 设置图标
    prevBtn->setIconSize(QSize(30, 30));         // 设置图标大小（关键，否则可能不显示）
    prevBtn->setStyleSheet("background-color: transparent; border: none;"); // 透明无边界
    // 关键：记录上一首按钮的原始尺寸
    prevBtnOriginalSize = prevBtn->size();

    // 暂停/播放按钮（正确写法）
    playPauseBtn = new QPushButton(backdrop20);
    playPauseBtn->setIcon(playIcon);            // 初始显示暂停图标
    playPauseBtn->setIconSize(QSize(30, 30));    // 图标大小
    playPauseBtn->setStyleSheet("background-color: transparent; border: none;");

    // 下一曲按钮（正确写法）
    nextBtn = new QPushButton(backdrop20);
    nextBtn->setIcon(nextIcon);
    nextBtn->setIconSize(QSize(30, 30));
    nextBtn->setStyleSheet("background-color: transparent; border: none;");
    // 关键：记录下一首按钮的原始尺寸
    nextBtnOriginalSize = nextBtn->size();

    ctrlHLayout->addStretch();
    ctrlHLayout->addWidget(prevBtn);
    ctrlHLayout->addWidget(playPauseBtn);
    ctrlHLayout->addWidget(nextBtn);
    ctrlHLayout->addStretch();
    musicVLayout->addLayout(ctrlHLayout);
    connect(playPauseBtn, &QPushButton::clicked, this, &SmartHomeWidget::on_playPauseClicked);
    widget7VLayout->addWidget(backdrop20);

    widget11HLayout->addWidget(widget7);
    mainVLayout->addWidget(widget11);

 // ---------- 共享播放器初始化 + 信号槽连接 ----------
    player = new QMediaPlayer(this); // 创建全局播放器


    // 播放器状态变化：控制光碟暂停/恢复（补充日志和兜底启动）
    connect(player, &QMediaPlayer::stateChanged, this, [this](QMediaPlayer::State state) {
        qDebug() << "[播放器状态] " << (state == QMediaPlayer::PlayingState ? "播放中" : "暂停/停止")
                 << " | 动画组当前状态：" << (cdGroupAnim ? cdGroupAnim->state() : QAbstractAnimation::Stopped)
                 << " | 暂停标记：" << isCdAnimationPaused;

        if (!cdGroupAnim) return; // 改为判断动画组是否存在

        if (state == QMediaPlayer::PlayingState) {
            // 情况1：动画组之前被暂停 → 恢复
            if (isCdAnimationPaused) {
                cdGroupAnim->setPaused(false); // 恢复动画组
                isCdAnimationPaused = false;
                qDebug() << "[光碟动画组] 从暂停状态恢复（旋转+缩放）";
            }
            // 情况2：动画组未启动（Stopped）→ 启动
            else if (cdGroupAnim->state() == QAbstractAnimation::Stopped) {
                cdGroupAnim->start(); // 启动动画组
                qDebug() << "[光碟动画组] 从停止状态启动（旋转+缩放）";
            }
        } else {
            // 暂停/停止时：冻结动画组（不停止，避免角度跳变）
            if (cdGroupAnim->state() == QAbstractAnimation::Running) {
                cdGroupAnim->setPaused(true); // 暂停动画组
                isCdAnimationPaused = true;
                qDebug() << "[光碟动画组] 暂停（旋转+缩放），当前角度：" << cdLabel->rotation() << "°";
            }
        }
        emit signalUpdatePlayState(state == QMediaPlayer::PlayingState);
    });

    // 连接音量滑块 → 播放器音量 + 数值标签更新
    connect(volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        player->setVolume(value);                  // 同步播放器音量
        volumeValueLabel->setText(QString("%1%").arg(value)); // 显示百分比（如 75%）
    });


    // 新增：播放位置变化时，更新时间标签
    // 播放进度变化时，同步光碟角度（适配多圈场景）
    // 【核心修改4：播放进度变化（删除手动setRotation，消除冲突闪烁）】
    connect(player, &QMediaPlayer::positionChanged, this, [this](qint64 currentPosition) {
        // 1. 仅更新时间标签（原逻辑保留）
        int minutes = currentPosition / 60000;
        int seconds = (currentPosition % 60000) / 1000;
        currentTimeLabel->setText(QString("%1:%2")
                                      .arg(minutes, 2, 10, QChar('0'))
                                      .arg(seconds, 2, 10, QChar('0')));

        // 2. 播放结束：光碟复位（仅此处手动改角度，避免结束时角度偏移）
        // 2. 播放结束：停止动画组并复位
        if (songTotalDuration > 0 && currentPosition >= songTotalDuration - 100) {
            if (cdGroupAnim) { // 改为判断动画组
                cdGroupAnim->stop();       // 停止动画组
                cdLabel->setRotation(0);   // 角度归0
                isCdAnimationPaused = false;
            }
            player->stop();
            playPauseBtn->setIcon(playIcon);
            qDebug() << "[播放结束] 光碟动画组停止并复位";
        }
    });

    // 记录歌曲总时长（仅用于判断播放结束，无多圈绑定）
    connect(player, &QMediaPlayer::durationChanged, this, [this](qint64 duration) {
        songTotalDuration = duration;
        qDebug() << "[歌曲时长] 加载完成：" << songTotalDuration << "ms";
    });

    // 连接“上一首/下一首”按钮与【动画槽函数】（替代原有的直接发射信号）
    connect(prevBtn, &QPushButton::clicked, this, &SmartHomeWidget::animatePrevButton);
    connect(nextBtn, &QPushButton::clicked, this, &SmartHomeWidget::animateNextButton);

    // 连接音量滑块 → 播放器音量
    connect(volumeSlider, &QSlider::valueChanged, player, &QMediaPlayer::setVolume);


    // 连接进度条与播放器位置
    connect(progressBar, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);
    connect(player, &QMediaPlayer::positionChanged, progressBar, &QSlider::setValue);
    connect(player, &QMediaPlayer::durationChanged, progressBar, &QSlider::setMaximum);

    // 连接“音乐按钮”与搜索对话框（传递播放器）
    // 连接“音乐按钮”与搜索对话框（单例控制）
    connect(musicButton, &QPushButton::clicked, this, [this]() {
        if (!player) {
            qWarning() << "[错误] QMediaPlayer 未初始化";
            return;
        }

        if (searchDialog == nullptr) { // 无对话框 → 创建新对话框
            searchDialog = new MusicSearchDialog(this, player);
            searchDialog->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动释放内存

            // 对话框关闭时，将指针置空（下次可重新创建）
            connect(searchDialog, &QDialog::destroyed, [this]() {
                searchDialog = nullptr;
            });

            // 连接信号（仅创建时连接一次，避免重复连接）
            connect(this, &SmartHomeWidget::signalPrevMedia, searchDialog, &MusicSearchDialog::onPrevMedia);
            connect(this, &SmartHomeWidget::signalNextMedia, searchDialog, &MusicSearchDialog::onNextMedia);
            connect(searchDialog, &MusicSearchDialog::signalPlayMedia, this, &SmartHomeWidget::signalPlayMedia);
            connect(searchDialog, &MusicSearchDialog::signalUpdateSongInfo, this, &SmartHomeWidget::signalUpdateSongInfo);
            connect(searchDialog, &MusicSearchDialog::signalUpdatePlayState, this, &SmartHomeWidget::signalUpdatePlayState);
            connect(searchDialog, &MusicSearchDialog::signalNextMedia, this, &SmartHomeWidget::signalNextMedia);
            connect(searchDialog, &MusicSearchDialog::signalPrevMedia, this, &SmartHomeWidget::signalPrevMedia);

            searchDialog->show(); // 显示新对话框
        }
        else
        { // 有对话框 → 激活已有对话框（提到最前）
            searchDialog->raise();         // 窗口置顶
            searchDialog->activateWindow(); // 激活窗口（获取焦点）
        }
    });

    // 连接主界面自身的同步信号
    connect(this, &SmartHomeWidget::signalPlayMedia, this, &SmartHomeWidget::onPlayMedia);
    connect(this, &SmartHomeWidget::signalUpdateSongInfo, this, &SmartHomeWidget::onUpdateSongInfo);
    connect(this, &SmartHomeWidget::signalUpdatePlayState, this, &SmartHomeWidget::onUpdatePlayState);
    connect(this, &SmartHomeWidget::signalNextMedia, this, &SmartHomeWidget::onNextMedia);
    connect(this, &SmartHomeWidget::signalPrevMedia, this, &SmartHomeWidget::onPrevMedia);


    // 打印 btnReturnHome 的全局坐标
    QPoint globalPos = btnReturnHome->mapToGlobal(QPoint(0, 0));
    QRect globalRect(globalPos, btnReturnHome->size());
    qDebug() << "[SmartHome] 标签全局范围：" << globalRect;
    /*************************** 5、按钮信号连接（保持互斥逻辑） ***************************/
//    // 归家标签点击：离家标签设为相反状态
//    connect(btnReturnHome, &HomeModeLabel::clicked, [ = ](bool isOn)
//    {
//        btnRunHome->setOn(!isOn);
//        qDebug() << "归家模式：" << (isOn ? "开" : "关") << "，离家模式：" << (isOn ? "关" : "开");
//    });

//    // 离家标签点击：归家标签设为相反状态
//    connect(btnRunHome, &HomeModeLabel::clicked, [ = ](bool isOn)
//    {
//        btnReturnHome->setOn(!isOn);
//        qDebug() << "离家模式：" << (isOn ? "开" : "关") << "，归家模式：" << (isOn ? "关" : "开");
//    });

//    qDebug() << "[SmartHome] HomeModeLabel 互斥逻辑初始化完成！";

    connect(btnSleep, &HomeModeLabel::clicked, this, &SmartHomeWidget::slots_sleepWakeClicked);
    connect(btnWakeUp, &HomeModeLabel::clicked, this, &SmartHomeWidget::slots_sleepWakeClicked);

//    // 归家开关：控制“归家/离家”图片切换
//    connect(btnReturnHome, &HomeModeLabel::clicked, [this](bool isOn) {
//        returnHomePic->setPixmap(isOn ? returnHomeOn : returnHomeOff); // 归家图切换
//        leaveHomePic->setPixmap(!isOn ? leaveHomeOn : leaveHomeOff);  // 离家图反向切换
//        btnRunHome->setOn(!isOn); // 保持互斥逻辑
//    });

//    // 离家开关：控制“离家/归家”图片切换
//    connect(btnRunHome, &HomeModeLabel::clicked, [this](bool isOn) {
//        leaveHomePic->setPixmap(isOn ? leaveHomeOn : leaveHomeOff);  // 离家图切换
//        returnHomePic->setPixmap(!isOn ? returnHomeOn : returnHomeOff); // 归家图反向切换
//        btnReturnHome->setOn(!isOn); // 保持互斥逻辑
//    });

    // 睡觉开关：控制“睡觉/起床”图片切换
    connect(btnSleep, &HomeModeLabel::clicked, [this](bool isOn) {
        sleepPic->setPixmap(isOn ? sleepOn : sleepOff);    // 睡觉图切换
        wakeUpPic->setPixmap(!isOn ? wakeOn : wakeOff);   // 起床图反向切换
        btnWakeUp->setOn(!isOn); // 保持互斥逻辑
    });

    // 起床开关：控制“起床/睡觉”图片切换
    connect(btnWakeUp, &HomeModeLabel::clicked, [this](bool isOn) {
        wakeUpPic->setPixmap(isOn ? wakeOn : wakeOff);   // 起床图切换
        sleepPic->setPixmap(!isOn ? sleepOn : sleepOff); // 睡觉图反向切换
        btnSleep->setOn(!isOn); // 保持互斥逻辑
    });

    // 初始化TCP服务端和客户端
    initTcpServer();   // 启动服务端（监听8888端口）
    initTcpClient();   // 初始化客户端
    connectToServer(); // 客户端连接服务端（默认连接本地127.0.0.1）

    // 绑定设备按钮点击事件（核心：点击后更新状态并触发TCP发送）
    bindDeviceButtons();
    // TCP连接界面按钮信号绑定
    connect(tcpConnectBtn, &QPushButton::clicked, this, &SmartHomeWidget::onTcpConnectBtnClicked);
    connect(tcpSendBtn, &QPushButton::clicked, this, &SmartHomeWidget::onTcpSendBtnClicked);
    // 按Enter键发送消息
    connect(tcpSendEdit, &QLineEdit::returnPressed, this, &SmartHomeWidget::onTcpSendBtnClicked);
}

// 槽函数逻辑不变，仅修改“状态设置方法”和“sender类型”
void SmartHomeWidget::slots_button_start(bool start)
{
    // 1. 把 sender 转换成 HomeModeLabel*（原来的是 Button_home_start*）
    HomeModeLabel* senderLabel = qobject_cast<HomeModeLabel*>(sender());
    if (senderLabel == nullptr) // 防止异常情况
    {
        qWarning() << "[SmartHome] 无效的信号发送者！";
        return;
    }

    // 2. 互斥逻辑不变，仅把 set_start 改成 setOn（HomeModeLabel 的状态设置方法）
    if (senderLabel == btnRunHome)
    {
        btnRunHome->setOn(start);       // 原来的 btnRunHome->set_start(start)
        btnReturnHome->setOn(!start);   // 原来的 btnReturnHome->set_start(!start)
        qDebug() << "离家模式：" << start << "，归家模式：" << !start;
    }
    else if (senderLabel == btnReturnHome)
    {
        btnReturnHome->setOn(start);    // 原来的 btnReturnHome->set_start(start)
        btnRunHome->setOn(!start);      // 原来的 btnRunHome->set_start(!start)
        qDebug() << "归家模式：" << start << "，离家模式：" << !start;
    }
}

void SmartHomeWidget::slots_sleepWakeClicked(bool isOn)
{
    HomeModeLabel* senderBtn = qobject_cast<HomeModeLabel*>(sender());
    if (senderBtn == nullptr) {
        qWarning() << "[SmartHome] 无效的信号发送者！";
        return;
    }

    // 点击“睡觉”按钮 → 起床按钮设为相反状态
    if (senderBtn == btnSleep) {
        btnSleep->setOn(isOn);
        btnWakeUp->setOn(!isOn);
        qDebug() << "睡觉模式：" << isOn << "，起床模式：" << !isOn;
    }
    // 点击“起床”按钮 → 睡觉按钮设为相反状态
    else if (senderBtn == btnWakeUp) {
        btnWakeUp->setOn(isOn);
        btnSleep->setOn(!isOn);
        qDebug() << "起床模式：" << isOn << "，睡觉模式：" << !isOn;
    }
}

void SmartHomeWidget::on_playPauseClicked()
{
    if (!player)
    {
        qWarning() << "[暂停错误] QMediaPlayer 未初始化";
        return;
    }

    // 直接根据播放器实际状态切换（避免静态变量不一致）
    if (player->state() == QMediaPlayer::PlayingState)
    {
        player->pause();          // 暂停播放
        playPauseBtn->setIcon(playIcon); // 切换为“播放”图标
    }
    else
    {
        player->play();           // 开始播放
        playPauseBtn->setIcon(pauseIcon); // 切换为“暂停”图标
    }
}
// ---------- 播放器同步槽函数实现 ----------
// 播放指定媒体
void SmartHomeWidget::onPlayMedia(const QUrl &url)
{
    player->setMedia(QMediaContent(url));
    player->play();  // 启动播放器，触发stateChanged信号（由该信号启动动画组）

    // 正确：操作动画组（cdGroupAnim），而非单个动画（cdAnimation）
    if (cdGroupAnim) {
        cdGroupAnim->stop();          // 停止动画组（确保状态干净）
        cdLabel->setRotation(0);      // 光碟角度归0（避免切换歌曲时角度偏移）
        isCdAnimationPaused = false;  // 清除暂停标记（确保stateChanged能正常启动）
        cdGroupAnim->start();
        qDebug() << "[onPlayMedia] 动画组已重置（角度归0，暂停标记清除）";
    }
    qDebug() << "[播放新歌曲] 光碟已重置到初始角度，等待播放器启动动画";
}

// 更新歌名与歌词
// 槽函数实现（确保正确更新 UI）
void SmartHomeWidget::onUpdateSongInfo(const QString &name, const QString &lyric)
{
    // 直接更新歌名标签
    songNameLabel->setText(name);
    // 更新歌词标签
    lyricLabel->setText(lyric);
    qDebug() << "[主界面] 同步歌名：" << name << "，歌词：" << lyric; // 调试用
}

// 更新播放/暂停状态（切换图标）
void SmartHomeWidget::onUpdatePlayState(bool isPlaying)
{
    playPauseBtn->setIcon(isPlaying ? pauseIcon : playIcon);
}

// 下一首（需扩展播放列表逻辑）
void SmartHomeWidget::onNextMedia()
{
    qDebug() << "[Player] 下一首请求";

    // 重置动画组
    if (cdGroupAnim) {
        cdGroupAnim->stop();
        cdLabel->setRotation(0);
        isCdAnimationPaused = false;
    }
}

// 上一首（需扩展播放列表逻辑）
void SmartHomeWidget::onPrevMedia()
{
    qDebug() << "[Player] 上一首请求";
    // 切换歌曲时重置光碟
    // 重置动画组
    if (cdGroupAnim) {
        cdGroupAnim->stop();
        cdLabel->setRotation(0);
        isCdAnimationPaused = false;
    }
}

// MusicSearchDialog.cpp 中重写关闭事件
void MusicSearchDialog::closeEvent(QCloseEvent *event)
{
    lyricTimer->stop(); // 停止歌词定时器
    QDialog::closeEvent(event);
}

void SmartHomeWidget::animatePrevButton()
{
    // 1. 记录按钮的原始图标尺寸
    prevBtnOriginalSize = prevBtn->iconSize();

    // 2. 执行“缩小图标”动画（100ms 缩放到 80%）
    QPropertyAnimation* shrinkAnim = new QPropertyAnimation(prevBtn, "iconSize");
    shrinkAnim->setDuration(100);
    shrinkAnim->setStartValue(prevBtnOriginalSize); // 原始大小
    shrinkAnim->setEndValue(QSize(
        prevBtnOriginalSize.width() * 0.8,  // 宽度缩放到 80%
        prevBtnOriginalSize.height() * 0.8  // 高度缩放到 80%
        ));
    shrinkAnim->start(QAbstractAnimation::DeleteWhenStopped); // 动画结束后自动销毁

    // 3. 延迟 100ms 后，执行“恢复图标”动画
    QTimer::singleShot(100, [this]() { // 捕获 this 指针，访问类成员
        QPropertyAnimation* expandAnim = new QPropertyAnimation(prevBtn, "iconSize");
        expandAnim->setDuration(100);
        expandAnim->setStartValue(QSize(
            this->prevBtnOriginalSize.width() * 0.8,
            this->prevBtnOriginalSize.height() * 0.8
            )); // 从缩小状态开始
        expandAnim->setEndValue(this->prevBtnOriginalSize); // 恢复原始大小
        expandAnim->start(QAbstractAnimation::DeleteWhenStopped);
    });

    // 4. 发射“上一首”信号（触发歌曲切换）
    emit signalPrevMedia();
}

void SmartHomeWidget::animateNextButton()
{
    // 1. 记录按钮的原始图标尺寸
    nextBtnOriginalSize = nextBtn->iconSize();

    // 2. 执行“缩小图标”动画（100ms 缩放到 80%）
    QPropertyAnimation* shrinkAnim = new QPropertyAnimation(nextBtn, "iconSize");
    shrinkAnim->setDuration(100);
    shrinkAnim->setStartValue(nextBtnOriginalSize);
    shrinkAnim->setEndValue(QSize(
        nextBtnOriginalSize.width() * 0.8,
        nextBtnOriginalSize.height() * 0.8
        ));
    shrinkAnim->start(QAbstractAnimation::DeleteWhenStopped);

    // 3. 延迟 100ms 后，执行“恢复图标”动画
    QTimer::singleShot(100, [this]() { // 捕获 this 指针，访问类成员
        QPropertyAnimation* expandAnim = new QPropertyAnimation(nextBtn, "iconSize");
        expandAnim->setDuration(100);
        expandAnim->setStartValue(QSize(
            this->nextBtnOriginalSize.width() * 0.8,
            this->nextBtnOriginalSize.height() * 0.8
            ));
        expandAnim->setEndValue(this->nextBtnOriginalSize);
        expandAnim->start(QAbstractAnimation::DeleteWhenStopped);
    });

    // 4. 发射“下一首”信号
    emit signalNextMedia();
}

// 绑定设备按钮点击事件（所有按钮点击最终调用updateDeviceState）
void SmartHomeWidget::bindDeviceButtons()
{
    // 归家模式（与离家互斥）
    connect(btnReturnHome, &HomeModeLabel::clicked, [this](bool isOn) {
        updateDeviceState("home", isOn);
        updateDeviceState("leave", !isOn); // 离家状态反向
    });

    // 离家模式（与归家互斥）
    connect(btnRunHome, &HomeModeLabel::clicked, [this](bool isOn) {
        updateDeviceState("leave", isOn);
        updateDeviceState("home", !isOn); // 归家状态反向
    });

    // 睡觉模式（与起床互斥）
    connect(btnSleep, &HomeModeLabel::clicked, [this](bool isOn) {
        updateDeviceState("sleep", isOn);
        updateDeviceState("wake", !isOn); // 起床状态反向
    });

    // 起床模式（与睡觉互斥）
    connect(btnWakeUp, &HomeModeLabel::clicked, [this](bool isOn) {
        updateDeviceState("wake", isOn);
        updateDeviceState("sleep", !isOn); // 睡觉状态反向
    });

    // 灯光（独立设备）
    connect(lightBtn, &HomeModeLabel::clicked, [this](bool isOn) {
        updateDeviceState("light", isOn);
    });

    // 空调（独立设备）
    connect(airBtn, &HomeModeLabel::clicked, [this](bool isOn) {
        updateDeviceState("air", isOn);
    });

    // 电视（独立设备）
    connect(tvBtn, &HomeModeLabel::clicked, [this](bool isOn) {
        updateDeviceState("tv", isOn);
    });

    // 窗帘（独立设备）
    connect(curtainBtn, &HomeModeLabel::clicked, [this](bool isOn) {
        updateDeviceState("curtain", isOn);
    });

    // 加湿器（独立设备）
    connect(humidifierBtn, &HomeModeLabel::clicked, [this](bool isOn) {
        updateDeviceState("humid", isOn);
    });

    // WiFi（独立设备）
    connect(wifiBtn, &HomeModeLabel::clicked, [this](bool isOn) {
        updateDeviceState("wifi", isOn);
    });
}

// 初始化TCP服务端（监听端口，等待客户端连接）
void SmartHomeWidget::initTcpServer()
{
    tcpServer = new QTcpServer(this);
    // 监听所有网络接口的8888端口（QHostAddress::Any表示接受任意IP连接）
    if (!tcpServer->listen(QHostAddress::Any, tcpServerPort)) {
        QMessageBox::warning(this, "TCP服务端",
            QString("启动失败！原因：%1").arg(tcpServer->errorString()));
        qWarning() << "[TCP服务端] 启动失败：" << tcpServer->errorString();
    } else {
        connect(tcpServer, &QTcpServer::newConnection, this, &SmartHomeWidget::onNewClientConnected);
        qDebug() << "[TCP服务端] 启动成功，监听端口：" << tcpServerPort;
    }
}

// 新客户端连接时触发
void SmartHomeWidget::onNewClientConnected()
{
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    if (!clientSocket) return;

    // 关键：开启 TCP 低延迟模式
    clientSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    tcpClientList.append(clientSocket);
    connect(clientSocket, &QTcpSocket::readyRead, this, &SmartHomeWidget::readTcpClientData);
    connect(clientSocket, &QTcpSocket::disconnected, this, &SmartHomeWidget::onClientDisconnected);

    qDebug() << "[TCP服务端] 新客户端连接：" << clientSocket->peerAddress().toString()
             << ":" << clientSocket->peerPort();
}

// 读取客户端发送的数据
void SmartHomeWidget::readTcpClientData()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    // 读取所有可用数据（不依赖换行符等待）
    QByteArray allData = clientSocket->readAll();
    if (allData.isEmpty()) return;

    // 按换行符分割，处理每一条指令
    QList<QByteArray> lines = allData.split('\n');
    for (QByteArray line : lines) {
        line = line.trimmed(); // 去除空格和换行符
        if (line.isEmpty()) continue;

        QString cmd = QString::fromUtf8(line);
        qDebug() << "[TCP服务端] 收到客户端数据：" << cmd
                 << "（来自" << clientSocket->peerAddress().toString() << "）";

        // 解析指令（格式：设备标识,状态，如"light,1"）
        QStringList parts = cmd.split(",");
        if (parts.size() != 2) {
            qWarning() << "[TCP服务端] 指令格式错误：" << cmd;
            continue;
        }
        QString devType = parts[0];
        bool isOn = (parts[1] == "1"); // "1"表示开启

        updateDeviceState(devType, isOn);
    }
}

// 客户端断开连接时触发
void SmartHomeWidget::onClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket && tcpClientList.contains(clientSocket)) {
        qDebug() << "[TCP服务端] 客户端断开连接：" << clientSocket->peerAddress().toString();
        tcpClientList.removeOne(clientSocket);
        clientSocket->deleteLater(); // 释放资源
    }
}

// 向所有连接的客户端广播指令（如设备状态变化时）
void SmartHomeWidget::sendToAllClients(const QString &cmd)
{
    if (tcpClientList.isEmpty()) {
        qDebug() << "[TCP服务端] 无连接的客户端，不发送指令：" << cmd;
        return;
    }

    QByteArray data = (cmd + "\r\n").toUtf8();
    for (QTcpSocket *client : tcpClientList) {
        if (client->state() == QTcpSocket::ConnectedState) {
            qint64 written = client->write(data);
            if (written != data.size()) {
                qWarning() << "[TCP服务端] 向客户端" << client->peerAddress().toString()
                           << "发送数据不完整，期望：" << data.size() << "，实际：" << written;
            }
            client->flush(); // 强制立即发送，不缓冲
        }
    }
    qDebug() << "[TCP服务端] 广播指令：" << cmd;
}

// 初始化TCP客户端
// 初始化TCP客户端（修改 readyRead 信号绑定）
void SmartHomeWidget::initTcpClient()
{
    tcpClient = new QTcpSocket(this);
    tcpServerIP.setAddress("192.168.50.95");
    tcpClientPort = 8888;

    tcpClient->setSocketOption(QAbstractSocket::LowDelayOption, 1); // 低延迟

    // 连接成功/断开：更新状态（不变）
    connect(tcpClient, &QTcpSocket::connected, this, [this]() { updateTcpStatus(true); });
    connect(tcpClient, &QTcpSocket::disconnected, this, [this]() { updateTcpStatus(false); });

    // 关键：收到数据时调用解析函数（核心逻辑，已正确绑定）
    connect(tcpClient, &QTcpSocket::readyRead, this, &SmartHomeWidget::readTcpServerData);

    // 错误处理（不变）
    connect(tcpClient, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            [this](QAbstractSocket::SocketError err) {
                (void)err;
                QString errMsg = tcpClient->errorString();
                tcpRecvText->append(QString("[%1] 错误：%2")
                                        .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                                        .arg(errMsg));
                updateTcpStatus(false);
            });
}

// 连接到TCP服务端
void SmartHomeWidget::connectToServer()
{
    if (tcpClient->state() == QTcpSocket::ConnectedState) {
        qDebug() << "[TCP客户端] 已连接到服务端，无需重复连接";
        return;
    }

    tcpClient->connectToHost(tcpServerIP, tcpClientPort);
    qDebug() << "[TCP客户端] 正在连接服务端：" << tcpServerIP.toString() << ":" << tcpClientPort;
}

// 读取服务端发送的数据（修改后：显示+更新状态）
void SmartHomeWidget::readTcpServerData()
{
    QByteArray allData = tcpClient->readAll();
    if (allData.isEmpty()) return;

    QList<QByteArray> lines = allData.split('\n');
    for (QByteArray line : lines) {
        line = line.trimmed();
        if (line.isEmpty()) continue;

        QString cmd = QString::fromUtf8(line);
        qDebug() << "[TCP客户端] 收到服务端数据：" << cmd;

        // 1. 显示消息
        appendTcpRecvData(cmd);

        // 2. 解析指令并更新设备状态
        QStringList parts = cmd.split(",");
        if (parts.size() == 2) {
            QString devType = parts[0];
            bool isOn = (parts[1] == "1");
            updateDeviceState(devType, isOn); // 调用核心函数更新界面
        } else {
            tcpRecvText->append(QString("[%1] 指令格式错误：%2（正确格式：设备,状态，如 light,1）")
                                    .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                                    .arg(cmd));
        }
    }
}

// 与服务端断开连接时触发
void SmartHomeWidget::onServerDisconnected()
{
    qDebug() << "[TCP客户端] 与服务端断开连接，尝试重连...";
    // 可选：自动重连
    QTimer::singleShot(3000, this, &SmartHomeWidget::connectToServer);
}

// 向服务端发送指令（如设备状态变化时）
void SmartHomeWidget::sendToServer(const QString &cmd)
{
    if (tcpClient->state() != QTcpSocket::ConnectedState) {
        qWarning() << "[TCP客户端] 未连接到服务端，无法发送指令：" << cmd;
        return;
    }

    QByteArray data = (cmd + "\r\n").toUtf8();
    qint64 written = tcpClient->write(data);
    if (written != data.size()) {
        qWarning() << "[TCP客户端] 发送数据不完整，期望：" << data.size() << "，实际：" << written;
    }
    tcpClient->flush(); // 强制立即发送，不缓冲
    qDebug() << "[TCP客户端] 发送指令到服务端：" << cmd;
}

// 更新设备状态（同步UI + 发送TCP指令）
void SmartHomeWidget::updateDeviceState(const QString &devType, bool isOn)
{
    // 1. 更新状态中枢记录
    if (deviceStates[devType] == isOn) {
        return; // 状态未变化，无需处理
    }

    bool oldState = deviceStates[devType]; // 记录修改前的旧状态
    deviceStates[devType] = isOn;          // 更新为新状态
    qDebug() << "[状态更新] " << devType << (isOn ? "开启" : "关闭")
             << "（旧状态：" << (oldState ? "开启" : "关闭") << "）";

//    deviceStates[devType] = isOn;
//    qDebug() << "[状态更新] " << devType << (isOn ? "开启" : "关闭");
    // -------------------------- 新增：互斥组双向联动逻辑 --------------------------
    // 规则：互斥组内设备始终反向，一方状态变化时，另一方自动设为相反值
    if (devType == "home") {
        // home 变化 → leave 设为 !isOn（无论home是1→0还是0→1）
        if (deviceStates["leave"] != !isOn) { // 只有当leave状态与目标值不同时才更新
            updateDeviceState("leave", !isOn);
        }
    } else if (devType == "leave") {
        // leave 变化 → home 设为 !isOn
        if (deviceStates["home"] != !isOn) {
            updateDeviceState("home", !isOn);
        }
    } else if (devType == "sleep") {
        // sleep 变化 → wake 设为 !isOn
        if (deviceStates["wake"] != !isOn) {
            updateDeviceState("wake", !isOn);
        }
    } else if (devType == "wake") {
        // wake 变化 → sleep 设为 !isOn
        if (deviceStates["sleep"] != !isOn) {
            updateDeviceState("sleep", !isOn);
        }
    }

    // 2. 更新UI（按钮状态 + 图片）
    if (devType == "home") {
        btnReturnHome->setOn(isOn);
        returnHomePic->setPixmap(isOn ? returnHomeOn : returnHomeOff);
    } else if (devType == "leave") {
        btnRunHome->setOn(isOn);
        leaveHomePic->setPixmap(isOn ? leaveHomeOn : leaveHomeOff);
    } else if (devType == "sleep") {
        btnSleep->setOn(isOn);
        sleepPic->setPixmap(isOn ? sleepOn : sleepOff);
    } else if (devType == "wake") {
        btnWakeUp->setOn(isOn);
        wakeUpPic->setPixmap(isOn ? wakeOn : wakeOff);
    } else if (devType == "light") {
        lightBtn->setOn(isOn);
        lightPic->setPixmap(isOn ? lightOn : lightOff);
    } else if (devType == "air") {
        airBtn->setOn(isOn);
        airPic->setPixmap(isOn ? airOn : airOff);
    } else if (devType == "tv") {
        tvBtn->setOn(isOn);
        tvPic->setPixmap(isOn ? tvOn : tvOff);
    } else if (devType == "curtain") {
        curtainBtn->setOn(isOn);
        curtainPic->setPixmap(isOn ? curtainOn : curtainOff);
    } else if (devType == "humid") {
        humidifierBtn->setOn(isOn);
        humidifierPic->setPixmap(isOn ? humidifierOn : humidifierOff);
    } else if (devType == "wifi") {
        wifiBtn->setOn(isOn);
        wifiPic->setPixmap(isOn ? wifiOn : wifiOff);
    }

    // 3. 发送TCP指令（根据角色选择发送方式）
    QString cmd = QString("%1,%2").arg(devType).arg(isOn ? 1 : 0);
    if (tcpServer->isListening()) {
        // 作为服务端：广播给所有客户端
        sendToAllClients(cmd);
    }
    if (tcpClient->state() == QTcpSocket::ConnectedState) {
        // 作为客户端：发送给服务端
        sendToServer(cmd);
    }
}

// 析构函数：释放资源
SmartHomeWidget::~SmartHomeWidget()
{
    // 关闭服务端
    if (tcpServer && tcpServer->isListening()) {
        tcpServer->close();
    }
    // 断开客户端连接
    if (tcpClient && tcpClient->state() == QTcpSocket::ConnectedState) {
        tcpClient->disconnectFromHost();
    }
    // 释放客户端列表
    for (QTcpSocket *client : tcpClientList) {
        client->deleteLater();
    }
}

// 连接/断开按钮点击事件
void SmartHomeWidget::onTcpConnectBtnClicked()
{
    if (tcpClient->state() == QTcpSocket::ConnectedState) {
        // 已连接：断开连接
        tcpClient->disconnectFromHost();
        tcpConnectBtn->setText("连接");
        tcpConnectBtn->setStyleSheet(
            "background-color: #4CAF50; color: white; padding: 5px; border-radius: 5px;"
            "hover { background-color: #45a049; }"
            );
        tcpSendBtn->setEnabled(false);
        updateTcpStatus(false);
    } else {
        // 未连接：解析IP和端口并连接
        QString ip = tcpServerIPEdit->text().trimmed();
        quint16 port = tcpPortEdit->text().toUInt();

        if (ip.isEmpty() || port == 0) {
            QMessageBox::warning(this, "输入错误", "IP或端口无效！");
            return;
        }

        tcpServerIP.setAddress(ip);
        tcpClientPort = port;
        connectToServer(); // 调用原连接函数

        // 临时更新状态（等待实际连接结果）
        tcpStatusLabel->setText("状态：连接中...");
        tcpStatusLabel->setStyleSheet("color: #FFC107; background-color: rgba(0,0,0,0);"); // 黄色
    }
}

// 发送按钮点击事件
void SmartHomeWidget::onTcpSendBtnClicked()
{
    QString msg = tcpSendEdit->text().trimmed();
    if (msg.isEmpty()) {
        QMessageBox::information(this, "提示", "发送消息不能为空！");
        return;
    }

    // 发送消息（复用原发送函数，格式："msg,实际消息"，区分设备指令）
    sendToServer(QString("msg,%1").arg(msg));
    tcpSendEdit->clear(); // 清空输入框
}

// 更新TCP连接状态显示
void SmartHomeWidget::updateTcpStatus(bool isConnected)
{
    if (isConnected) {
        tcpStatusLabel->setText(QString("状态：已连接 %1:%2")
                                    .arg(tcpServerIP.toString()).arg(tcpClientPort));
        tcpStatusLabel->setStyleSheet("color: #4CAF50; background-color: rgba(0,0,0,0);"); // 绿色
        tcpConnectBtn->setText("断开");
        tcpConnectBtn->setStyleSheet(
            "background-color: #f44336; color: white; padding: 5px; border-radius: 5px;"
            "hover { background-color: #d32f2f; }"
            );
        // 关键修复：同时启用输入框和发送按钮
        tcpSendEdit->setEnabled(true);  // 启用输入框
        tcpSendBtn->setEnabled(true);   // 启用发送按钮
    } else {
        tcpStatusLabel->setText("状态：未连接");
        tcpStatusLabel->setStyleSheet("color: #E74C3C; background-color: rgba(0,0,0,0);"); // 红色
        // 未连接时禁用
        tcpSendEdit->setEnabled(false);
        tcpSendBtn->setEnabled(false);
    }
}

// 追加接收的TCP消息到界面
void SmartHomeWidget::appendTcpRecvData(const QString& data)
{
    // 区分设备指令和普通消息（设备指令格式："devType,state"，普通消息："msg,内容"）
    QStringList parts = data.split(",");
    QString displayText;

    if (parts.size() == 2 && parts[0] == "msg") {
        // 普通消息：显示原始内容
        displayText = QString("[%1] 收到：%2")
                          .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                          .arg(parts[1]);
    } else {
        // 设备指令：显示格式化内容（如"light,1" → "灯光：开启"）
        QString devName = "";
        if (parts[0] == "light") devName = "灯光";
        else if (parts[0] == "air") devName = "空调";
        else if (parts[0] == "tv") devName = "电视";
        else if (parts[0] == "curtain") devName = "窗帘";
        else if (parts[0] == "humid") devName = "加湿器";
        else if (parts[0] == "wifi") devName = "WiFi";
        else if (parts[0] == "home") devName = "归家模式";
        else if (parts[0] == "leave") devName = "离家模式";
        else if (parts[0] == "sleep") devName = "睡觉模式";
        else if (parts[0] == "wake") devName = "起床模式";
        else devName = parts[0];

        displayText = QString("[%1] 设备指令：%2 %3")
                          .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                          .arg(devName)
                          .arg(parts[1] == "1" ? "开启" : "关闭");
    }

    tcpRecvText->append(displayText); // 追加到接收区
    // 自动滚动到底部
    QTextCursor cursor = tcpRecvText->textCursor();
    cursor.movePosition(QTextCursor::End);
    tcpRecvText->setTextCursor(cursor);
}

// 全自动字体加载：返回可用字体（自定义字体或默认字体）
QFont SmartHomeWidget::initCustomFont(const QString& fontFilePath, int pointSize, bool applyToGlobal) {
    // 步骤1：尝试加载自定义字体
    int fontId = QFontDatabase::addApplicationFont(fontFilePath);
    QString fontFamily;

    // 步骤2：判断加载结果，获取字体家族名
    if (fontId != -1) {
        QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.isEmpty()) {
            fontFamily = families.first();
            qDebug() << "[自定义字体] 加载成功：" << fontFamily << "（路径：" << fontFilePath << "）";
        }
    }

    // 步骤3：加载失败时，使用系统默认字体（降级方案）
    if (fontFamily.isEmpty()) {
        qWarning() << "[自定义字体] 加载失败，使用系统默认字体（路径：" << fontFilePath << "）";
#ifdef Q_OS_WIN
        fontFamily = "SimHei";  // 默认
#elif defined(Q_OS_MAC)
        fontFamily = "PingFang SC";      // macOS 默认
#else
        fontFamily = "WenQuanYi Micro Hei"; // Linux 默认
#endif
    }

    // 步骤4：创建字体对象（设置字号）
    QFont resultFont(fontFamily, pointSize);

    // 步骤5：如果需要全局应用，设置为应用程序字体
    if (applyToGlobal) {
        QApplication::setFont(resultFont);
        qDebug() << "[全局字体] 已应用：" << fontFamily << "（字号：" << pointSize << "）";
    }

    return resultFont;  // 返回可用字体（自定义或默认）
}


