#ifndef SMARTHOMEWIDGET_H
#define SMARTHOMEWIDGET_H

#include <QWidget>
#include <QLabel>       // 用于标签（图标、歌曲名、歌词）
#include <QPushButton>  // 用于控制按钮（上一曲、播放/暂停、下一曲）
#include <QSlider>      // 用于进度条
#include <QVBoxLayout>  // 垂直布局
#include <QHBoxLayout>  // 水平布局
#include <QPixmap>      // 图片资源
#include <QMediaPlayer>
#include <QPropertyAnimation>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QTcpServer>    // TCP服务端
#include <QTcpSocket>    // TCP客户端/连接
#include <QHostAddress>  // IP地址
#include <QMap>          // 设备状态管理
#include <QTextEdit>
#include <QFontDatabase>
#include <QDebug>
#include <QApplication>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTransform>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QParallelAnimationGroup>
#include "widget_assist_backdrop.h"
#include "button_home_start.h"
#include "homemodelabel.h"
#include "musicsearchdialog.h"
// 自定义支持旋转属性的 Label
class RotatableLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)       // 声明“缩放”属性
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation) // 声明“旋转”属性

public:
    explicit RotatableLabel(QWidget *parent = nullptr)
        : QLabel(parent), m_rotation(0.0),m_scale(1.0)
    {
//        // 确保图片缩放平滑
//        setTransformationMode(Qt::SmoothTransformation);
    }
    // ---- scale 属性 ----
    qreal scale() const { return m_scale; }
    void setScale(qreal s) {
        m_scale = s;
        update(); // 触发重绘，使缩放生效
    }
    qreal rotation() const { return m_rotation; }

    // 设置旋转角度（仅记录角度，由 paintEvent 处理绘制）
    void setRotation(qreal angle)
    {
        if (qFuzzyCompare(m_rotation, angle)) return; // 避免重复更新
        m_rotation = angle;
        update(); // 触发重绘
        emit rotationChanged(angle);
    }

protected:
    // 重写绘制事件：手动绘制旋转后的图片（核心修正）
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);
        if (!pixmap() || pixmap()->isNull()) {
            // 无图片时，调用父类绘制（显示文字或空白）
            QLabel::paintEvent(event);
            return;
        }

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿
        painter.setRenderHint(QPainter::SmoothPixmapTransform); // 图片平滑缩放

        // 1. 计算旋转原点（控件中心）
        QPointF center(width() / 2.0, height() / 2.0);

        // 2. 移动坐标系原点到中心（关键：确保绕中心旋转）
        painter.translate(center);

        // 3. 应用旋转（使用当前角度）
        painter.rotate(m_rotation);

        // 4. 还原坐标系原点（绘制时以中心为基准）
        painter.translate(-center);

        // 5. 绘制图片（居中显示，适应控件大小）
        QRectF targetRect(QPointF(0, 0), size()); // 控件区域
        QRectF sourceRect(pixmap()->rect());      // 图片原始区域
        painter.drawPixmap(targetRect, *pixmap(), sourceRect);
    }

signals:
    void rotationChanged(qreal angle);

private:
    qreal m_rotation; // 旋转角度（度）
    qreal m_scale;    // 缩放比例

};

class SmartHomeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SmartHomeWidget(QWidget* parent = nullptr);
    ~SmartHomeWidget();
    void bindDeviceButtons();

private:

    QFont initCustomFont(
        const QString& fontFilePath,  // 字体文件路径（资源或本地）
        int pointSize = 10,           // 字号（默认10）
        bool applyToGlobal = false    // 是否应用到全局（默认否）
        );

private slots:
    void slots_button_start(bool start); // 保持按钮互斥逻辑
    void slots_sleepWakeClicked(bool isOn);
    // 可选：添加音乐播放器交互槽函数（如播放/暂停切换）
     void on_playPauseClicked();

     // 新增：播放器同步槽函数
     void onPlayMedia(const QUrl &url);          // 播放指定媒体
     void onUpdateSongInfo(const QString &name, const QString &lyric); // 更新歌名、歌词
     void onUpdatePlayState(bool isPlaying);     // 更新播放/暂停状态
     void onNextMedia();                         // 下一首
     void onPrevMedia();
     void animatePrevButton(); // 上一首按钮动画
     void animateNextButton(); // 下一首按钮动画
     // TCP服务端
     void initTcpServer();                // 初始化服务端
     void onNewClientConnected();         // 新客户端连接
     void readTcpClientData();            // 读取客户端数据
     void onClientDisconnected();         // 客户端断开连接
     void sendToAllClients(const QString &cmd);  // 向所有客户端广播指令

     // TCP客户端
     void initTcpClient();                // 初始化客户端
     void connectToServer();              // 连接到服务端
     void readTcpServerData();            // 读取服务端数据
     void onServerDisconnected();         // 与服务端断开连接
     void sendToServer(const QString &cmd);     // 向服务端发送指令

     // 设备状态管理
     void updateDeviceState(const QString &devType, bool isOn);  // 更新状态（UI+TCP）

     // TCP相关槽函数
     void onTcpConnectBtnClicked(); // 连接/断开按钮点击
     void onTcpSendBtnClicked();    // 发送按钮点击
     void updateTcpStatus(bool isConnected); // 更新连接状态
     void appendTcpRecvData(const QString& data); // 追加接收的消息
 signals:
     void signalPlayMedia(const QUrl &url);          // 发射“播放媒体”信号
     void signalUpdateSongInfo(const QString &name, const QString &lyric); // 发射“更新歌曲信息”信号
     void signalUpdatePlayState(bool isPlaying);     // 发射“更新播放状态”信号
     void signalNextMedia();                         // 发射“下一首”信号
     void signalPrevMedia();                         // 发射“上一首”信号
private:
    // ========== 主布局（对应 XML: verticalLayout_3） ==========
    QVBoxLayout* mainVLayout;

    // ========== 第一行（对应 XML: widget_12） ==========
    QWidget* widget12;
    QHBoxLayout* widget12HLayout;
    Widget_Assist_Backdrop* backdropReturnHome; // XML: widget_return_home
    Widget_Assist_Backdrop* backdropRunHome;    // XML: widget_run_home
    Widget_Assist_Backdrop* backdrop14;         // XML: widget_14
    Widget_Assist_Backdrop* backdrop16;         // XML: widget_16
    QSpacerItem* topHorizontalSpacer;           // XML: horizontalSpacer
    HomeModeLabel* btnReturnHome; // 归家模式标签
    HomeModeLabel* btnRunHome;    // 离家模式标签

    // ========== 第二行（对应 XML: widget_11） ==========
    QWidget* widget11;
    QHBoxLayout* widget11HLayout;

    // ---------- 第二行左侧（对应 XML: widget_10） ----------
    QWidget* widget10;
    QVBoxLayout* widget10VLayout; // XML: verticalLayout_2
    QWidget* backdrop9; // XML: widget_9
    QWidget* widgetInner;
    QVBoxLayout* widgetInnerVLayout; // XML: verticalLayout_4

    // ~ widgetInner 内第一行（对应 XML: widget_2） ~
    QWidget* widget2;
    QHBoxLayout* widget2HLayout; // XML: horizontalLayout_3
    QWidget* backdrop4;
    QWidget* backdrop5;
    QWidget* backdrop6;
    QSpacerItem* widget2Spacer1; // XML: horizontalSpacer_5
    QSpacerItem* widget2Spacer2; // XML: horizontalSpacer_6

    // ~ widgetInner 内第二行（对应 XML: widget_3） ~
    QWidget* widget3;
    QHBoxLayout* widget3HLayout; // XML: horizontalLayout_4
    QWidget* backdrop17;
    QWidget* backdrop18;
    QWidget* backdrop19;
    QSpacerItem* widget3Spacer1; // XML: horizontalSpacer_7
    QSpacerItem* widget3Spacer2; // XML: horizontalSpacer_8

    // ---------- 第二行右侧（对应 XML: widget_7） ----------
    QWidget* widget7;
    QVBoxLayout* widget7VLayout; // XML: verticalLayout
    QWidget* backdrop8; // XML: widget_8
    //Widget_Assist_Backdrop* backdrop20; // XML: widget_20
    QWidget *backdrop20;


    // 播放器控件
    QPushButton *musicButton;       // 音乐按钮（顶部）
    QLabel *songNameLabel;       // 歌曲名标签
    QLabel *lyricLabel;          // 歌词标签
    QSlider *progressBar;        // 播放进度条
    QPushButton *prevBtn;        // 上一曲按钮
    QPushButton *playPauseBtn;   // 播放/暂停按钮
    QPushButton *nextBtn;        // 下一曲按钮

    // 播放器图标资源（可选，若需在槽函数中切换图标）
    QIcon playIcon;    // 播放图标（类成员，生命周期与窗口一致）
    QIcon pauseIcon;   // 暂停图标
    QIcon prevIcon;    // 上一曲图标
    QIcon nextIcon;    // 下一曲图标
    QIcon musicIcon;   // 顶部音乐图标

    HomeModeLabel* btnSleep;   // 睡觉模式按钮
    HomeModeLabel* btnWakeUp;  // 起床模式按钮

    // 图片显示标签
    // 场景图片显示标签
    QLabel *returnHomePic; // 归家场景图
    QLabel *leaveHomePic;  // 离家场景图
    QLabel *sleepPic;      // 睡觉场景图
    QLabel *wakeUpPic;     // 起床场景图

    // 状态标记（true=对应状态激活）
    bool isReturnHome = false;
    bool isRunHome = false;
    bool isSleep = false;
    bool isWakeUp = false;

    // ---- 图片资源（提前加载）----
    QPixmap returnHomeOff, returnHomeOn;
    QPixmap leaveHomeOff, leaveHomeOn;
    QPixmap sleepOff, sleepOn;
    QPixmap wakeOff, wakeOn;

    QPixmap wifiOff,wifiOn;
    QPixmap airOff,airOn;
    QPixmap lightOff,lightOn;
    QPixmap tvOff,tvOn;
    QPixmap curtainOff,curtainOn;
    QPixmap humidifierOff,humidifierOn;


    // 新增：共享媒体播放器
    QMediaPlayer* player;

    QSize prevBtnOriginalSize; // 上一首按钮原始尺寸
    QSize nextBtnOriginalSize; // 下一首按钮原始尺寸
    QSlider *volumeSlider;       // 新增：音量调节滑块
    QLabel *volumeIconLabel;     // 音量图标标签（用于视觉区分）
    QIcon volumeIcon;            // 音量图标资源
    QLabel *currentTimeLabel;    // 新增：当前播放时间标签
    QLabel *volumeValueLabel;    // 新增：音量数值标签（显示百分比）
    MusicSearchDialog *searchDialog; // 新增：音乐搜索对话框指针（控制单例）

    // 按钮
    HomeModeLabel *lightBtn;       // 灯光按钮
    HomeModeLabel *airBtn;         // 空调按钮
    HomeModeLabel *tvBtn;          // 电视按钮
    HomeModeLabel *curtainBtn;     // 窗帘按钮
    HomeModeLabel *humidifierBtn;  // 加湿器按钮
    HomeModeLabel *wifiBtn;        // WiFi按钮

    // 声明设备状态图片的成员变量
    QLabel *lightPic;       // 灯光状态图
    QLabel *airPic;         // 空调状态图
    QLabel *tvPic;          // 电视状态图
    QLabel *curtainPic;     // 窗帘状态图
    QLabel *humidifierPic;  // 加湿器状态图
    QLabel *wifiPic;        // WiFi状态图

    QSerialPort *serial;          // 串口对象
    bool isSerialOpen;            // 串口是否已打开的状态标记


    // 2. TCP服务端相关
    QTcpServer *tcpServer;               // 服务端对象
    QList<QTcpSocket*> tcpClientList;    // 已连接的客户端列表
    quint16 tcpServerPort = 8888;        // 服务端端口（默认8888）

    // 3. TCP客户端相关
    QTcpSocket *tcpClient;               // 客户端对象
    QHostAddress tcpServerIP;            // 服务端IP（默认本地）
    quint16 tcpClientPort = 8888;        // 服务端端口（默认8888）

    // 4. 设备状态中枢（记录所有设备当前状态：key=设备标识，value=是否开启）
    QMap<QString, bool> deviceStates;
    //quint16 tcpClientPort=8888 ; // 直接初始化默认值

    // TCP连接界面控件
    QLineEdit* tcpServerIPEdit;   // IP输入框
    QLineEdit* tcpPortEdit;       // 端口输入框
    QPushButton* tcpConnectBtn;   // 连接/断开按钮
    QLabel* tcpStatusLabel;       // 状态显示
    QLineEdit* tcpSendEdit;       // 发送消息输入
    QPushButton* tcpSendBtn;      // 发送按钮
    QTextEdit* tcpRecvText;       // 接收消息显示

    RotatableLabel* cdLabel; // 替换原 QLabel*
    QPropertyAnimation* cdAnimation;

    int cdRotationCycles = 5; // 自定义：整首歌时长内旋转的圈数（默认3圈，可改5、7等）
    qint64 songTotalDuration = 0; // 歌曲总时长（ms）
    // 新增：记录光碟动画的暂停状态
    bool isCdAnimationPaused = false;
    QParallelAnimationGroup *cdGroupAnim; // <-- 添加这行（动画组成员变量）

    // ---- 辅助：带动画切换图片（增强视觉）----
    void switchPixmapWithFade(QLabel *label, const QPixmap &newPix);
};

#endif // SMARTHOMEWIDGET_H
