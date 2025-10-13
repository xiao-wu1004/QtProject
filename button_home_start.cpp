#include "button_home_start.h"
#include <QDebug>  // 用于容错打印

Button_home_start::Button_home_start(QWidget* parent) : Button_Cust_Base(parent)
{
    // 关键：关闭鼠标事件透传，确保按钮能接收点击
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setMouseTracking(true);      // 启用鼠标跟踪
    setFocusPolicy(Qt::ClickFocus); // 允许点击获取焦点
    setCursor(Qt::PointingHandCursor); // 鼠标悬浮为“手型”，提示可点击


    // 原有代码（尺寸、图片等）保持不变
    setFixedSize(40, 40);
    setScaledContents(true);
    QPixmap initPixmap(":/状态开关_开");
    if (initPixmap.isNull())
    {
        qWarning() << "警告：未找到资源图片 :/状态开关_关，请检查资源文件！";
        // 若图片缺失，用红色背景占位，确保视觉可见
        setStyleSheet("background-color: red;");
    }
    else
    {
        setPixmap(initPixmap);
    }
    m_start = false;

}


void Button_home_start::mousePressEvent(QMouseEvent* event)
{
    // 1. 打印事件来源和按钮状态，确认事件到达
    qDebug() << "[Button] 事件触发！按钮地址：" << this
             << "，当前状态：" << m_start
             << "，鼠标按钮：" << event->button();

    // 2. 校验点击位置是否在按钮内
    if (!rect().contains(event->pos()))
    {
        qDebug() << "[Button] 点击超出按钮范围（按钮尺寸：" << rect() << "，点击坐标：" << event->pos() << "）";
        return;
    }

    // 3. 仅处理左键点击
    if (event->button() != Qt::LeftButton)
    {
        qDebug() << "[Button] 非左键点击（按钮：" << event->button() << "），忽略";
        return;
    }

    // 4. 切换状态+加载图片+发射信号（原有逻辑不变）
    m_start = !m_start;
    set_start(m_start);
    emit signals_start(m_start);
}

void Button_home_start::set_start(bool isStart)
{
    QPixmap pixmap;
    if (isStart)
    {
        pixmap.load(":/状态开关_开");
        if (pixmap.isNull())
        {
            qWarning() << "错误：未找到资源 :/状态开关_开，请检查.qrc文件！";
            setStyleSheet("background-color: green;"); // 临时占位，方便视觉排查
            return;
        }
    }
    else
    {
        pixmap.load(":/状态开关_关");
        if (pixmap.isNull())
        {
            qWarning() << "错误：未找到资源 :/状态开关_关，请检查.qrc文件！";
            setStyleSheet("background-color: red;"); // 临时占位
            return;
        }
    }
    setPixmap(pixmap);
    m_start = isStart;
}
