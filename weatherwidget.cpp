#include "weatherwidget.h"
#include "widget_assist_backdrop.h"
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QPixmap>
#include <QSpacerItem>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QRegExp>
#include <QCompleter>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
// 自定义高低温曲线绘制部件
class TempCurveWidget : public QWidget
{

public:
    // 构造函数：接收七天数据（日期、高温、低温）
    TempCurveWidget(const QStringList& dates, const QList<int>& highTemps, const QList<int>& lowTemps, QWidget* parent = nullptr)
        : QWidget(parent), m_dates(dates), m_highTemps(highTemps), m_lowTemps(lowTemps)
    {
        // 设置部件尺寸（适应 backdrop3 的 120 高度）
        setMinimumSize(0, 150);
        setMaximumSize(INT_MAX, 150);
    }

    // 新增：更新曲线数据的方法
    void updateData(const QStringList& dates, const QList<int>& highTemps, const QList<int>& lowTemps)
    {
        m_dates = dates;
        m_highTemps = highTemps;
        m_lowTemps = lowTemps;
        update(); // 触发重绘
    }

protected:
    // 重写绘图事件：绘制坐标轴、曲线、数据点、标签
    void paintEvent(QPaintEvent* event) override
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿

        // 1. 计算绘图区域（为文本预留空间）
        int marginLeft = 30;
        int marginRight = 20;
        int marginTop = 20;
        int marginBottom = 30; // 底部留更多空间
        int plotWidth = width() - marginLeft - marginRight;
        int plotHeight = height() - marginTop - marginBottom;

        if (plotWidth <= 0 || plotHeight <= 0) return;


        // 2. 确定温度范围（留10%余量避免贴边）
        int maxTemp = *std::max_element(m_highTemps.begin(), m_highTemps.end());
        int minTemp = *std::min_element(m_lowTemps.begin(), m_lowTemps.end());
        int tempRange = maxTemp - minTemp;
        if (tempRange == 0) tempRange = 5;
        maxTemp += tempRange * 0.1;
        minTemp -= tempRange * 0.1;


        // 3. 绘制坐标轴与网格
        painter.save();
        painter.translate(marginLeft, marginTop); // 原点移至绘图区左上角


        // 3.1 水平线（每5度一条）
        painter.setPen(QPen(QColor(220, 220, 220), 1));
        for (int temp = minTemp; temp <= maxTemp; temp += 5)
        {
            int y = plotHeight - (temp - minTemp) * plotHeight / (maxTemp - minTemp);
            painter.drawLine(0, y, plotWidth, y);
        }

        // 3.2 垂直线 + 日期标签（修改后：深色字体 + 清晰样式）
        // 先画竖线（浅灰半透明）
        int dayCount = m_dates.size();
        int xStep = plotWidth / (dayCount - 1);
        painter.setPen(QPen(QColor(200, 200, 200, 150), 1)); // RGBA：浅灰 + 50%透明度
        for (int i = 0; i < dayCount; ++i)
        {
            int x = i * xStep;
            painter.drawLine(x, 0, x, plotHeight);
        }
        // 再画日期文本（深灰色，层级更高）
        painter.setPen(QPen(QColor(50, 50, 50), 1));
        painter.setFont(QFont("SimHei", 10));
        for (int i = 0; i < dayCount; ++i)
        {
            int x = i * xStep;
            painter.drawText(x - 15, plotHeight + 20, m_dates[i]);
        }
        painter.restore();


        // 4. 绘制高温/低温曲线 + 数据点 + 数据点温度
        painter.save();
        painter.translate(marginLeft, marginTop);

        // 高温曲线（珊瑚红）
        QPen highPen(QColor(255, 127, 80), 2);
        painter.setPen(highPen);
        drawCurve(painter, m_highTemps, maxTemp, minTemp, plotWidth, plotHeight);
        drawPoints(painter, m_highTemps, maxTemp, minTemp, plotWidth, plotHeight, QColor(255, 127, 80));

        // 低温曲线（天蓝色）
        QPen lowPen(QColor(135, 206, 235), 2);
        painter.setPen(lowPen);
        drawCurve(painter, m_lowTemps, maxTemp, minTemp, plotWidth, plotHeight);
        drawPoints(painter, m_lowTemps, maxTemp, minTemp, plotWidth, plotHeight, QColor(135, 206, 235));


        // 数据点上方温度文本
        painter.setPen(QPen(Qt::black, 1));
        painter.setFont(QFont("SimHei", 8));
        for (int i = 0; i < m_highTemps.size(); ++i)
        {
            int x = i * xStep;
            int y = plotHeight - (m_highTemps[i] - minTemp) * plotHeight / (maxTemp - minTemp);
            painter.drawText(x - 8, y - 10, QString::number(m_highTemps[i]) + "°");
        }
        for (int i = 0; i < m_lowTemps.size(); ++i)
        {
            int x = i * xStep;
            int y = plotHeight - (m_lowTemps[i] - minTemp) * plotHeight / (maxTemp - minTemp);
            painter.drawText(x - 8, y - 10, QString::number(m_lowTemps[i]) + "°");
        }

        painter.restore();


        // 5. 右上角图例（放在折线图外部右上角，水平并排）
        // 5. 图例整体：与折线图垂直布局（放在折线图上方），内部水平排列
        painter.setPen(QPen(Qt::black, 1));
        painter.setFont(QFont("SimHei", 10));

        // 图例整体位置：折线图上方居中（与折线图形成垂直分布）
        int legendTop = 10; // 距离部件顶部10px
        int legendCenterX = width() -100; // 水平居中

        // 高温图例（左侧，内部水平布局）
        // 线条：高温颜色，水平短横线
        painter.setPen(QPen(QColor(255, 127, 80), 3));
        painter.drawLine(legendCenterX - 80, legendTop + 5, legendCenterX - 60, legendTop + 5);
        // 文本：线条右侧，与线条水平对齐
        painter.setPen(QPen(Qt::black, 1));
        painter.drawText(legendCenterX - 55, legendTop + 8, "高温");

        // 低温图例（右侧，与高温水平并排，内部水平布局）
        // 线条：低温颜色，水平短横线
        painter.setPen(QPen(QColor(135, 206, 235), 3));
        painter.drawLine(legendCenterX + 20, legendTop + 5, legendCenterX + 40, legendTop + 5);
        // 文本：线条右侧，与线条水平对齐
        painter.setPen(QPen(Qt::black, 1));
        painter.drawText(legendCenterX + 45, legendTop + 8, "低温");
    }

private:
    // 绘制曲线（根据温度计算坐标）
    void drawCurve(QPainter& painter, const QList<int>& temps, int maxTemp, int minTemp, int plotWidth, int plotHeight)
    {
        int dayCount = temps.size();
        int xStep = plotWidth / (dayCount - 1);

        for (int i = 0; i < dayCount - 1; ++i)
        {
            // 当前点坐标
            int x1 = i * xStep;
            int y1 = plotHeight - (temps[i] - minTemp) * plotHeight / (maxTemp - minTemp);

            // 下一个点坐标
            int x2 = (i + 1) * xStep;
            int y2 = plotHeight - (temps[i + 1] - minTemp) * plotHeight / (maxTemp - minTemp);

            painter.drawLine(x1, y1, x2, y2); // 绘制线段
        }
    }

    // 绘制数据点（圆点）
    void drawPoints(QPainter& painter, const QList<int>& temps, int maxTemp, int minTemp, int plotWidth, int plotHeight, const QColor& color)
    {
        int dayCount = temps.size();
        int xStep = plotWidth / (dayCount - 1);
        painter.setBrush(QBrush(color)); // 填充颜色

        for (int i = 0; i < dayCount; ++i)
        {
            int x = i * xStep;
            int y = plotHeight - (temps[i] - minTemp) * plotHeight / (maxTemp - minTemp);

            // 绘制3x3的圆点
            painter.drawEllipse(QPoint(x, y), 3, 3);
        }
    }

private:
    QStringList m_dates;      // 七天日期（如："今天"、"周二"）
    QList<int> m_highTemps;   // 高温数据
    QList<int> m_lowTemps;    // 低温数据
};

WeatherWidget::WeatherWidget(QWidget* parent) : QWidget(parent)
{
    // ---------- 1. 初始化核心成员 ----------
    m_netManager = new QNetworkAccessManager(this); // 初始化网络管理器
    loadCityData();         // 加载城市ID映射
    initWeatherIconMap();   // 加载天气图标映射

    // ---------- 2. 窗口基础设置 ----------
    setFixedSize(696, 532);
    setMinimumSize(696, 532);
    setMaximumSize(696, 532);
    setWindowTitle("天气查询");

    // ---------- 3. 主垂直布局 ----------
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout->setSpacing(10);

    // ---------- 4. backdrop1：搜索+定位区 ----------
//    backdrop1 = new Widget_Assist_Backdrop(this);
//    backdrop1->setMinimumSize(0, 50);
//    backdrop1->setMaximumSize(INT_MAX, 50);
//    // 在 backdrop1 初始化后添加
//    backdrop1->setMinimumWidth(600); // 根据窗口总宽度（696px）调整，确保有足够空间

//    backdrop1->setLabelText("backdrop1");

//    QHBoxLayout* topLayout = new QHBoxLayout(backdrop1);
//    topLayout->setContentsMargins(15, 5, 15, 5);
//    topLayout->setSpacing(10);

//    this->searchEdit = new QLineEdit(backdrop1);
//    this->searchEdit->setEnabled(true); // 显式启用
//    this->searchEdit->setAttribute(Qt::WA_InputMethodEnabled); // 启用输入法支持
//    this->searchEdit->setFocus(); // 初始获得焦点
//    this->searchEdit->setFocusPolicy(Qt::StrongFocus); // 👉 强制支持鼠标点击获取焦点
//    this->searchEdit->setPlaceholderText("输入城市名搜索天气");
//    this->searchEdit->setStyleSheet("border: 1px solid #ccc; border-radius: 4px; padding: 5px;");
//    this->searchEdit->setFixedHeight(30);
//    // 在初始化 topLayout 时，调整搜索框的尺寸策略，避免挤压
//    this->searchEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 搜索框占满剩余空间
//    this->searchEdit->setMinimumWidth(200); // 确保搜索框有足够宽度

//    this->searchBtn = new QPushButton(backdrop1);
//    this->searchBtn->setEnabled(true); // 显式启用
//    this->searchBtn->setIcon(QIcon(":/搜索")); // 替换为实际搜索图标路径
//    this->searchBtn->setIconSize(QSize(20, 20));
//    this->searchBtn->setFixedSize(30, 30);
//    this->searchBtn->setStyleSheet("border: 1px solid #ccc; border-radius: 4px;");

//    locationLab = new QLabel("当前城市：北京", backdrop1);
//    locationLab->setFont(QFont("SimHei", 10));
//    locationLab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

//    topLayout->addWidget(this->searchEdit);
//    topLayout->addWidget(this->searchBtn);
//    topLayout->addSpacing(20); // 增加按钮和定位标签的间距
//    topLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

//    topLayout->addWidget(locationLab);
//    topLayout->addStretch(); // 右侧拉伸，确保定位标签靠右

//    backdrop1->setLayout(topLayout); // 布局生效的核心代码
//    // 在 backdrop1->setLayout(topLayout); 后添加：
//    backdrop1->setFocusPolicy(Qt::ClickFocus);
//    backdrop1->setFocusProxy(this->searchEdit); // 点击背景聚焦到搜索框


//    // 在连接搜索信号的代码后添加：
//    bool btnConnected = connect(this->searchBtn, &QPushButton::clicked, this, &WeatherWidget::searchWeather);
//    bool editConnected = connect(this->searchEdit, &QLineEdit::returnPressed, this, &WeatherWidget::searchWeather);
//    qDebug() << "搜索按钮信号连接成功？" << btnConnected;
//    qDebug() << "搜索框回车信号连接成功？" << editConnected;

//    verticalLayout->addWidget(backdrop1);

    backdrop1 = new QWidget(this);
    backdrop1->setMinimumHeight(50);
    backdrop1->setStyleSheet("background-color: #D5EEFF;"); // 临时背景色，方便调试

    QHBoxLayout* searchLayout = new QHBoxLayout(backdrop1);
    searchLayout->setContentsMargins(15, 5, 15, 5);
    searchLayout->setSpacing(10);

    // 搜索框
    searchEdit = new QLineEdit(backdrop1);
    searchEdit->setPlaceholderText("输入城市名搜索天气");
    searchEdit->setStyleSheet("border: 1px solid #ccc; border-radius: 10px; padding: 5px;");
    searchEdit->setFixedHeight(30);
    searchEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    searchEdit->setFocus(); // 初始聚焦，打开窗口就可输入

    // 搜索按钮
    searchBtn = new QPushButton(backdrop1);
    searchBtn->setIcon(QIcon(":/搜索")); // 替换为实际搜索图标路径
    searchBtn->setIconSize(QSize(20, 20));
     m_originalIconSize = searchBtn->iconSize(); // 保存原始尺寸（关键：固定缩放基准）
    searchBtn->setFixedSize(30, 30);
    searchBtn->setStyleSheet("border: 1px solid #ccc; border-radius: 15px;");

    // 定位标签
    locationLab = new QLabel("当前城市：北京", backdrop1);
    locationLab->setFont(QFont("SimHei", 10));
    locationLab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // 加入布局
    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchBtn);
    searchLayout->addSpacing(20);
    searchLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    searchLayout->addWidget(locationLab);
    searchLayout->addStretch();

    verticalLayout->addWidget(backdrop1);

    // ---------- 连接搜索信号（确保点击/回车都触发） ----------
    // 搜索按钮点击 → 先执行动画，再搜索
    connect(searchBtn, &QPushButton::clicked, this, &WeatherWidget::animateSearchBtn);
    connect(searchEdit, &QLineEdit::returnPressed, this, &WeatherWidget::searchWeather);
    qDebug() << "按钮点击连接：" << connect(searchBtn, &QPushButton::clicked, this, &WeatherWidget::searchWeather);
    qDebug() << "回车事件连接：" << connect(searchEdit, &QLineEdit::returnPressed, this, &WeatherWidget::searchWeather);



    initSearchCompleter(); // 初始化搜索补全
    // ---------- 5. backdrop2：今日天气区 ----------
    backdrop2 = new Widget_Assist_Backdrop(this);
    backdrop2->setMinimumSize(0, 120);
    backdrop2->setMaximumSize(INT_MAX, 120);
    //backdrop2->setLabelText("backdrop2");

    // 中间容器（统一半透明、圆角风格）
    QWidget* todayContainer = new QWidget(backdrop2);
    todayContainer->setStyleSheet(R"(
        background-color: rgba(213,238,255,1);
        border-radius: 10px;
        padding: 10px;
    )");

    // 主水平布局：容纳三个卡片
    QHBoxLayout* todayMainLayout = new QHBoxLayout(todayContainer);
    todayMainLayout->setContentsMargins(0, 0, 0, 0);
    todayMainLayout->setSpacing(20); // 卡片间距

    // ========== 卡片1：天气图标 + 温度 ==========
    QWidget* card1 = new QWidget(todayContainer);
    card1->setStyleSheet(R"(
        background-color: rgba(255, 255, 255, 0.5);
        border-radius: 10px;
        padding: 8px;
    )");
    QHBoxLayout* card1Layout = new QHBoxLayout(card1);
    card1Layout->setContentsMargins(10, 10, 10, 10);
    card1Layout->setSpacing(5);

    m_weatherIcon = new QLabel(card1);
    QPixmap initIcon(":/weatherIcon/sunny.png"); // 替换为实际晴天图标路径
    m_weatherIcon->setPixmap(initIcon.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_weatherIcon->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    card1Layout->addWidget(m_weatherIcon);

    // 右侧：垂直布局（天气文字 + 温度）
    QVBoxLayout* rightVLayout = new QVBoxLayout();
    rightVLayout->setContentsMargins(0, 0, 0, 0);
    rightVLayout->setSpacing(5); // 天气文字与温度的间距

    // 新增：天气文字标签（默认显示“晴”）
    m_weatherTextLab = new QLabel("晴", card1);
    m_weatherTextLab->setFont(QFont("SimHei", 14, QFont::Medium)); // 字体比温度小，清晰易读
    m_weatherTextLab->setAlignment(Qt::AlignCenter);
    rightVLayout->addWidget(m_weatherTextLab);

    m_tempLab = new QLabel("33°C", card1);
    m_tempLab->setFont(QFont("SimHei", 16, QFont::Bold));
    m_tempLab->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    rightVLayout->addWidget(m_tempLab);
    card1Layout->addLayout(rightVLayout);

    // ========== 卡片2：湿度 + 风力 ==========
    QWidget* card2 = new QWidget(todayContainer);
    card2->setStyleSheet(R"(
        background-color: rgba(255, 255, 255, 0.5);
        border-radius: 10px;
        padding: 8px;
    )");
    QVBoxLayout* card2Layout = new QVBoxLayout(card2);
    card2Layout->setContentsMargins(10, 10, 10, 10);
    card2Layout->setSpacing(5);

    // ---------- 湿度行（图标 + 文字） ----------
    QWidget* humidityRow = new QWidget(card2);
    QHBoxLayout* humidityRowLayout = new QHBoxLayout(humidityRow);
    humidityRowLayout->setContentsMargins(0, 0, 0, 0);
    humidityRowLayout->setSpacing(5);

    m_humidityIcon = new QLabel(humidityRow);
    QPixmap humidityIcon(":/湿度"); // 替换为「湿度」图标路径
    m_humidityIcon->setPixmap(humidityIcon.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_humidityIcon->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    m_humidityLab = new QLabel("湿度：86%", humidityRow);
    m_humidityLab->setFont(QFont("SimHei", 11));
    m_humidityLab->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    humidityRowLayout->addWidget(m_humidityIcon);
    humidityRowLayout->addWidget(m_humidityLab);

    // ---------- 风力行（图标 + 文字） ----------
    QWidget* windRow = new QWidget(card2);
    QHBoxLayout* windRowLayout = new QHBoxLayout(windRow);
    windRowLayout->setContentsMargins(0, 0, 0, 0);
    windRowLayout->setSpacing(5);

    m_windIcon = new QLabel(windRow);
    QPixmap windIcon(":/风力"); // 替换为「风力」图标路径
    m_windIcon->setPixmap(windIcon.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_windIcon->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    m_windLab = new QLabel("风力：1级", windRow);
    m_windLab->setFont(QFont("SimHei", 11));
    m_windLab->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    windRowLayout->addWidget(m_windIcon);
    windRowLayout->addWidget(m_windLab);

    card2Layout->addWidget(humidityRow);
    card2Layout->addWidget(windRow);
    // ========== 卡片3：日期 + 提示语 ==========
    QWidget* card3 = new QWidget(todayContainer);
    card3->setStyleSheet(R"(
        background-color: rgba(255, 255, 255, 0.5);
        border-radius: 10px;
        padding: 8px;
    )");
    QVBoxLayout* card3Layout = new QVBoxLayout(card3);
    card3Layout->setContentsMargins(10, 10, 10, 10);
    card3Layout->setSpacing(10);

    // ---------- 日期行（图标 + 文字） ----------
    QWidget* dateRow = new QWidget(card3);
    QHBoxLayout* dateRowLayout = new QHBoxLayout(dateRow);
    dateRowLayout->setContentsMargins(0, 0, 0, 0);
    dateRowLayout->setSpacing(5);

    m_dateIcon = new QLabel(dateRow);
    QPixmap dateIcon(":/日期"); // 替换为「日期」图标路径
    m_dateIcon->setPixmap(dateIcon.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_dateIcon->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    m_dateLab = new QLabel("2025-08-01", dateRow);
    m_dateLab->setFont(QFont("SimHei", 11));
    m_dateLab->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    dateRowLayout->addWidget(m_dateIcon);
    dateRowLayout->addWidget(m_dateLab);

    m_noticeLab = new QLabel("愿你拥有比阳光明媚的心情", card3);
    m_noticeLab->setFont(QFont("SimHei", 11));
    m_noticeLab->setAlignment(Qt::AlignCenter | Qt::AlignBottom);

    card3Layout->addWidget(dateRow);
    card3Layout->addWidget(m_noticeLab);

    // 将三个卡片加入主布局
    todayMainLayout->addWidget(card1);
    todayMainLayout->addWidget(card2);
    todayMainLayout->addWidget(card3);

    // 将中间容器加入 backdrop2
    QVBoxLayout* backdrop2Layout = new QVBoxLayout(backdrop2);
    backdrop2Layout->setContentsMargins(10, 10, 10, 10);
    backdrop2Layout->addWidget(todayContainer);

    verticalLayout->addWidget(backdrop2);

    // ---------- 6. backdrop3：七天曲线区 ----------
    backdrop3 = new Widget_Assist_Backdrop(this);
    backdrop3->setMinimumSize(0, 150);
    backdrop3->setMaximumSize(INT_MAX, 150);
    //backdrop3->setLabelText("backdrop3");
    backdrop3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);


    QStringList defaultDays = {"今天", "周二", "周三", "周四", "周五", "周六", "周日"};
    QList<int> defaultHigh = {30, 32, 28, 26, 25, 29, 31};
    QList<int> defaultLow = {15, 17, 13, 12, 10, 14, 16};

    m_curveWidget = new TempCurveWidget(defaultDays, defaultHigh, defaultLow, backdrop3);
    QVBoxLayout* curveLayout = new QVBoxLayout(backdrop3);
    curveLayout->setContentsMargins(0, 0, 0, 0);
    curveLayout->addWidget(m_curveWidget);
    m_curveWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    verticalLayout->addWidget(backdrop3);
    // 插入间隔器（高度 25 表示额外增加 25px 间距）
    QSpacerItem* spacer = new QSpacerItem(35, 35, QSizePolicy::Minimum, QSizePolicy::Fixed);
    verticalLayout->addItem(spacer);

    // ---------- 7. backdrop4：七天卡片区 ----------
    backdrop4 = new Widget_Assist_Backdrop(this);
    backdrop4->setMinimumSize(0, 180);
    backdrop4->setMaximumSize(INT_MAX, 180);
    //backdrop4->setLabelText("backdrop4");

    // 初始化七天卡片布局（成员变量赋值）
    m_sevenDayLayout = new QHBoxLayout(backdrop4);
    m_sevenDayLayout->setContentsMargins(10, 10, 10, 10);
    m_sevenDayLayout->setSpacing(10);

    QList<QVariantList> defaultCardData =
    {
        {"今天", ":/weatherIcon/sunny.png", "26°", "18°"},
        {"周二", ":/weatherIcon/cloudy.png", "27°", "19°"},
        {"周三", ":/weatherIcon/rainy.png", "25°", "17°"},
        {"周四", ":/weatherIcon/rainy.png", "23°", "16°"},
        {"周五", ":/weatherIcon/cloudy.png", "22°", "15°"},
        {"周六", ":/weatherIcon/sunny.png", "24°", "16°"},
        {"周日", ":/weatherIcon/sunny.png", "26°", "17°"}
    };

    for (const auto& dayData : defaultCardData)
    {
        QWidget* dayCard = new QWidget(backdrop4);
        dayCard->setStyleSheet("background-color: rgba(255, 255, 255, 0.7); border-radius: 8px; padding: 5px;");
        dayCard->setMinimumWidth(85);

        QVBoxLayout* cardLayout = new QVBoxLayout(dayCard);
        cardLayout->setContentsMargins(2, 2, 2, 2);
        cardLayout->setSpacing(8);

        QLabel* weekLab = new QLabel(dayData[0].toString(), dayCard);
        weekLab->setFont(QFont("SimHei", 10, QFont::Medium));
        weekLab->setAlignment(Qt::AlignCenter);
        cardLayout->addWidget(weekLab);

        QLabel* iconLab = new QLabel(dayCard);
        QPixmap icon(dayData[1].toString());
        iconLab->setPixmap(icon.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        iconLab->setAlignment(Qt::AlignCenter);
        cardLayout->addWidget(iconLab);

        QWidget* tempWidget = new QWidget(dayCard);
        QHBoxLayout* tempHLay = new QHBoxLayout(tempWidget);
        tempHLay->setContentsMargins(0, 0, 0, 0);
        tempHLay->setSpacing(5);
        // 增加左侧拉伸项（确保标签居中，且不被挤压）
        tempHLay->addStretch();
        QLabel* highTempLab = new QLabel(dayData[2].toString(), tempWidget);
        highTempLab->setStyleSheet("color: #FF6347;");
        highTempLab->setFont(QFont("SimHei", 9));
        highTempLab->setAlignment(Qt::AlignCenter); // 👉 标签文本居中，避免偏移

        QLabel* lowTempLab = new QLabel(dayData[3].toString(), tempWidget);
        lowTempLab->setStyleSheet("color: #4682B4;");
        lowTempLab->setFont(QFont("SimHei", 9));
        lowTempLab->setAlignment(Qt::AlignCenter);

        tempHLay->addWidget(highTempLab);
        tempHLay->addWidget(lowTempLab);
        // 增加右侧拉伸项（与左侧配合，让标签水平居中）
        tempHLay->addStretch();
        tempWidget->setLayout(tempHLay);

        cardLayout->addWidget(tempWidget, 0, Qt::AlignCenter);
        m_sevenDayLayout->addWidget(dayCard);
    }
    m_sevenDayLayout->addStretch();

    verticalLayout->addWidget(backdrop4);

    // ---------- 8. 底部间隔器 ----------
    verticalSpacer = new QSpacerItem(20, 65, QSizePolicy::Minimum, QSizePolicy::Fixed);
    verticalLayout->addItem(verticalSpacer);

    // ---------- 9. 设置主布局 ----------
    setLayout(verticalLayout);

    // ---------- 10. 连接网络信号 ----------
    connect(m_netManager, &QNetworkAccessManager::finished, this, &WeatherWidget::onWeatherReplyFinished);

}


// 加载 city.txt 城市数据
void WeatherWidget::loadCityData()
{
    QFile file(":/citydata/city.txt"); // 确保 city.txt 在资源文件中
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "[Weather] 城市文件打开失败！路径：:/city.txt";
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() >= 2)
        {
            QString cityId = parts[0];
            QString cityName = parts.mid(1).join(" ");
            m_cityMap[cityName] = cityId;
        }
    }

    file.close();
    qDebug() << "[Weather] 加载城市数：" << m_cityMap.size();
}

// 初始化搜索框模糊补全
void WeatherWidget::initSearchCompleter()
{
    QStringList cityNames = m_cityMap.keys();
    QCompleter* completer = new QCompleter(cityNames, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    this->searchEdit->setCompleter(completer);

    connect(completer, QOverload<const QString&>::of(&QCompleter::activated), this, [ = ](const QString & city)
    {
        this->searchEdit->setText(city);
        this->searchEdit->setFocus(); // 手动恢复焦点
        this->searchWeather();
    });
}

// 初始化天气图标映射
void WeatherWidget::initWeatherIconMap()
{
    m_weatherIconMap["晴"] = ":/weatherIcon/sunny.png";
    m_weatherIconMap["多云"] = ":/weatherIcon/cloudy.png";
    m_weatherIconMap["阴"] = ":/weatherIcon/overcast.png";
    m_weatherIconMap["雨"] = ":/weatherIcon/rainy.png";
    m_weatherIconMap["阵雨"] = ":/weatherIcon/shower.png";
    m_weatherIconMap["雷阵雨"] = ":/weatherIcon/thunder.png";
    m_weatherIconMap["雪"] = ":/weatherIcon/snow.png";
    m_weatherIconMap["雾"] = ":/weatherIcon/fog.png";
    m_weatherIconMap["霾"] = ":/weatherIcon/haze.png";
}

// 触发天气搜索
void WeatherWidget::searchWeather()
{
    QString cityName = this->searchEdit->text().trimmed();
    if (cityName.isEmpty())
    {
        qWarning() << "[Weather] 搜索城市为空！";
        return;
    }

    if (!m_cityMap.contains(cityName))
    {
        qWarning() << "[Weather] 未找到城市：" << cityName;
        locationLab->setText("未找到该城市！");
        return;
    }

    QString cityId = m_cityMap[cityName];
    QString apiUrl = QString("http://t.weather.itboy.net/api/weather/city/%1").arg(cityId);
    qDebug() << "[Weather] 请求API：" << apiUrl;

    QNetworkRequest request;
    request.setUrl(QUrl(apiUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_netManager->get(request);
}

// 处理API返回
void WeatherWidget::onWeatherReplyFinished(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning() << "[Weather] 网络错误：" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError)
    {
        qWarning() << "[Weather] JSON解析错误：" << err.errorString();
        return;
    }

    QJsonObject root = doc.object();
    if (root["status"].toInt() != 200)
    {
        qWarning() << "[Weather] API错误：" << root["message"].toString();
        return;
    }

    QJsonObject weatherData = root["data"].toObject();
    updateTodayWeather(weatherData);
    updateSevenDayWeather(weatherData["forecast"].toArray());

    locationLab->setText(QString("当前城市：%1").arg(this->searchEdit->text().trimmed()));
}

// 更新今日天气
void WeatherWidget::updateTodayWeather(const QJsonObject& data)
{
    // 从 forecast[0] 提取今日天气
    QJsonObject todayForecast = data["forecast"].toArray()[0].toObject();

    //天气文字
    QString weatherType = todayForecast["type"].toString(); // 从JSON获取天气类型
    m_weatherTextLab->setText(weatherType); // 设置到天气文字标签
    // 1. 温度
    QString highTempStr = todayForecast["high"].toString();
    int highTemp = highTempStr.remove("高温 ").remove("℃").toInt();
    m_tempLab->setText(QString("%1°C").arg(highTemp));

    // 2. 风力
    QString fengli = todayForecast["fl"].toString();
    m_windLab->setText(QString("风力：%1").arg(fengli));

    // 3. 湿度
    QString shidu = data["shidu"].toString();
    m_humidityLab->setText(QString("湿度：%1").arg(shidu));

    // 4. 日期
    QString date = todayForecast["ymd"].toString();
    m_dateLab->setText(date);

    // 5. 提示语
    QString notice = todayForecast["notice"].toString();
    m_noticeLab->setText(notice);

    // 6. 天气图标
    QString type = todayForecast["type"].toString();
    QString iconPath = m_weatherIconMap.value(type, ":/weatherIcon/sunny.png");
    m_weatherIcon->setPixmap(QPixmap(iconPath).scaled(60, 60, Qt::KeepAspectRatio));
}

// 更新未来七天
void WeatherWidget::updateSevenDayWeather(const QJsonArray& forecast)
{
    if (forecast.size() < 7)
    {
        qWarning() << "[Weather] 七天数据不足：" << forecast.size() << "天";
        return;
    }

    QStringList days;      // 曲线用：日期（供 TempCurveWidget 显示）
    QList<int> highTemps;
    QList<int> lowTemps;
    QList<QVariantList> cardData; // 卡片用：星期、图标、高温、低温

    for (int i = 0; i < 7; ++i)
    {
        QJsonObject day = forecast[i].toObject();

        // 1. 获取「星期」（从 JSON 的 "week" 字段）
        QString week = day["week"].toString();

//        // 2. 获取「日期」（供曲线显示，如“今天”“10-10”）
//        QString date = (i == 0) ? "今天" : day["date"].toString().split("-").mid(1).join("-");
//        days << date;
        // 2. 曲线 X 轴标签：第1天显示“今天”，其余显示星期
        QString curveLabel = (i == 0) ? "今天" : week;
        days << curveLabel;  // 👉 关键：days 现在存储星期信息

        // 3. 提取高低温
        int high = day["high"].toString().remove("高温 ").remove("℃").toInt();
        int low = day["low"].toString().remove("低温 ").remove("℃").toInt();
        highTemps << high;
        lowTemps << low;

        // 4. 提取天气类型与图标
        QString type = day["type"].toString();
        QString iconPath = m_weatherIconMap.value(type, ":/weatherIcon/sunny.png");

        // 5. 生成卡片数据：【星期】、图标、高温、低温
        cardData << QVariantList{week, iconPath, QString("%1°").arg(high), QString("%1°").arg(low)};
    }

    // 更新曲线（曲线用「日期」）
    m_curveWidget->updateData(days, highTemps, lowTemps);

    // 清空旧卡片，生成新卡片
    while (m_sevenDayLayout->count() > 0)
    {
        QLayoutItem* item = m_sevenDayLayout->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    // -------------------------- 动态生成卡片（与默认卡片布局完全一致） --------------------------
    for (const auto& data : cardData)
    {
        // 1. 卡片基础设置（宽度、样式与默认一致）
        QWidget* dayCard = new QWidget(backdrop4);
        dayCard->setStyleSheet("background-color: rgba(255, 255, 255, 0.7); border-radius: 8px; padding: 5px;");
        dayCard->setMinimumWidth(85); // 对齐默认卡片的 85px 宽度

        // 2. 卡片内部垂直布局（内边距 2px，与默认一致）
        QVBoxLayout* cardLayout = new QVBoxLayout(dayCard);
        cardLayout->setContentsMargins(2, 2, 2, 2); // 对齐默认卡片的 2px 内边距
        cardLayout->setSpacing(8); // 对齐默认卡片的间距

        // 3. 星期标签（字体加粗、对齐与默认一致）
        QLabel* weekLab = new QLabel(data[0].toString(), dayCard);
        weekLab->setFont(QFont("SimHei", 10, QFont::Medium)); // 加 Medium 加粗
        weekLab->setAlignment(Qt::AlignCenter);
        cardLayout->addWidget(weekLab);

        // 4. 天气图标（64x64 + 平滑变换，与默认一致）
        QLabel* iconLab = new QLabel(dayCard);
        QPixmap icon(data[1].toString());
        // 对齐默认卡片的图标缩放：64x64 + 平滑变换（SmoothTransformation）
        iconLab->setPixmap(icon.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        iconLab->setAlignment(Qt::AlignCenter);
        cardLayout->addWidget(iconLab);

        // 5. 温度容器（左右拉伸、标签居中，与默认一致）
        QWidget* tempWidget = new QWidget(dayCard);
        QHBoxLayout* tempHLay = new QHBoxLayout(tempWidget);
        tempHLay->setContentsMargins(0, 0, 0, 0);
        tempHLay->setSpacing(5); // 对齐默认卡片的温度间距

        // 左右拉伸项（确保温度标签居中，与默认一致）
        tempHLay->addStretch();

        // 高温标签（字体、颜色、对齐与默认一致）
        QLabel* highTempLab = new QLabel(data[2].toString(), tempWidget);
        highTempLab->setStyleSheet("color: #FF6347;");
        highTempLab->setFont(QFont("SimHei", 9));
        highTempLab->setAlignment(Qt::AlignCenter);

        // 低温标签（字体、颜色、对齐与默认一致）
        QLabel* lowTempLab = new QLabel(data[3].toString(), tempWidget);
        lowTempLab->setStyleSheet("color: #4682B4;");
        lowTempLab->setFont(QFont("SimHei", 9));
        lowTempLab->setAlignment(Qt::AlignCenter);

        tempHLay->addWidget(highTempLab);
        tempHLay->addWidget(lowTempLab);
        tempHLay->addStretch(); // 右拉伸项
        tempWidget->setLayout(tempHLay);

        cardLayout->addWidget(tempWidget, 0, Qt::AlignCenter);
        m_sevenDayLayout->addWidget(dayCard);
    }
    m_sevenDayLayout->addStretch();
}

void WeatherWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event); // 先执行父类逻辑
    // 延迟100ms打印（确保布局完全计算完成）
    QTimer::singleShot(100, this, [=]() {
        qDebug() << "实际显示：搜索框位置" << searchEdit->geometry();
        qDebug() << "实际显示：搜索按钮位置" << searchBtn->geometry();
        qDebug() << "实际显示：backdrop1大小" << backdrop1->size();
    });
}

void WeatherWidget::animateSearchBtn()
{
    // 1. 停止所有正在运行的图标尺寸动画（避免叠加）
    QList<QPropertyAnimation*> animations = searchBtn->findChildren<QPropertyAnimation*>();
    for (auto anim : animations) {
        if (anim->propertyName() == "iconSize") {
            anim->stop();
        }
    }

    // 2. 强制重置图标尺寸为原始值（核心：避免多次点击后尺寸叠加）
    searchBtn->setIconSize(m_originalIconSize);

    // 3. 创建基于原始尺寸的缩放动画
    QPropertyAnimation* iconSizeAnim = new QPropertyAnimation(searchBtn, "iconSize", this);
    iconSizeAnim->setDuration(400); // 动画时长
    iconSizeAnim->setEasingCurve(QEasingCurve::OutBounce); // 弹性曲线增强视觉感

    // 关键帧：原始 → 缩小60% → 放大150% → 恢复原始
    iconSizeAnim->setKeyValueAt(0.0, m_originalIconSize);
    iconSizeAnim->setKeyValueAt(0.3, QSize(
                                         m_originalIconSize.width() * 0.6,
        m_originalIconSize.height() * 0.6
                                         ));
    iconSizeAnim->setKeyValueAt(0.7, QSize(
                                         m_originalIconSize.width() * 1.5,
        m_originalIconSize.height() * 1.5
                                         ));
    iconSizeAnim->setKeyValueAt(1.0, m_originalIconSize);

    // 4. 启动动画（结束后自动销毁）
    iconSizeAnim->start(QAbstractAnimation::DeleteWhenStopped);

    // 5. 动画结束后触发搜索
    QTimer::singleShot(400, this, &WeatherWidget::searchWeather);
}
