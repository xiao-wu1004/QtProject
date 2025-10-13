#ifndef WEATHERWIDGET_H
#define WEATHERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QMap>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonArray>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include "widget_assist_backdrop.h"
// 前置声明自定义曲线部件（若在 .cpp 中定义）
class TempCurveWidget;

namespace Ui
{
    class WeatherWidget;
}
class WeatherWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WeatherWidget(QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;
signals:
private slots:
    void searchWeather(); // 触发天气搜索
    void onWeatherReplyFinished(QNetworkReply* reply); // 处理API返回
    void updateTodayWeather(const QJsonObject& data); // 更新今日天气
    void updateSevenDayWeather(const QJsonArray& forecast); // 更新未来七天
    void animateSearchBtn(); // 按钮动画 + 触发搜索的槽
private:
    void loadCityData(); // 加载 city.txt 城市数据
    void initSearchCompleter(); // 初始化搜索框模糊补全
    void initWeatherIconMap(); // 初始化天气图标映射
private:
    // 垂直布局（对应 XML 中的 verticalLayout）
    QVBoxLayout* verticalLayout;
    // 4个自定义背景控件（对应 XML 中的 widget、widget_2、widget_3、widget_4）
//    Widget_Assist_Backdrop* backdrop1;
    QWidget* backdrop1; // 新增：声明为类成员
    Widget_Assist_Backdrop* backdrop2;
    Widget_Assist_Backdrop* backdrop3;
    Widget_Assist_Backdrop* backdrop4;
    // 底部垂直 spacer（对应 XML 中的 verticalSpacer）
    QSpacerItem* verticalSpacer;


    // 搜索相关控件
    QLineEdit* searchEdit;
    QPushButton* searchBtn;
    QLabel* locationLab;

    // 天气显示控件（用于后续更新）
    QLabel* m_tempLab;       // 今日温度
    QLabel* m_statusLab;     // 天气状态（晴/多云）
    QLabel* m_humidityLab;   // 湿度
    QLabel* m_windLab;       // 风向风速
    QLabel* m_weatherIcon;   // 今日天气图标

    // 七天数据相关
    TempCurveWidget* m_curveWidget; // 七天曲线部件
    QHBoxLayout* m_sevenDayLayout;  // 七天卡片布局

    // 核心数据存储
    QMap<QString, QString> m_cityMap;       // 城市名→城市ID
    QMap<QString, QString> m_weatherIconMap;// 天气类型→图标路径
    QNetworkAccessManager* m_netManager;    // 网络请求管理器
    QLabel* m_dateLab; // 日期显示标签
    QLabel *m_noticeLab;
    QLabel* m_humidityIcon; // 湿度图标
    QLabel* m_windIcon;     // 风力图标
    QLabel* m_dateIcon; // 日期图标
    QLabel* m_weatherTextLab;// 新增：天气文字标签（如“晴”“多云”）
    QLabel* searchDefaultIcon; // 搜索按钮默认图标
    QLabel* searchClickIcon;   // 点击时切换的图标
    QSize m_originalIconSize; // 存储图标原始尺寸
};

#endif // WEATHERWIDGET_H
