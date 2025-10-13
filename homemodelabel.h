#ifndef HOMEMODELABEL_H
#define HOMEMODELABEL_H

#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include <QMouseEvent>  // 👉 新增：包含 QMouseEvent 的头文件
class HomeModeLabel : public QLabel
{
    Q_OBJECT
public:
    // 构造函数：传入“关”“开”图片路径，父部件
    HomeModeLabel(const QString& offImgPath, const QString& onImgPath, QWidget* parent = nullptr)
        : QLabel(parent), m_offImg(offImgPath), m_onImg(onImgPath), m_isOn(false)
    {
        // 基础配置（和你原来的按钮尺寸/样式保持一致）
        setFixedSize(40, 40);       // 尺寸 40x40（和原按钮一致）
        setScaledContents(true);    // 图片自适应标签
        setCursor(Qt::PointingHandCursor); // 鼠标悬浮显“手型”
        setAttribute(Qt::WA_TransparentForMouseEvents, false); // 不透传事件

        // 初始加载“关”状态图片
        loadImage(m_offImg);

        raise(); // 确保标签在所有子部件之上
     }

    // 外部设置状态（用于互斥逻辑）
    void setOn(bool isOn)
    {
        m_isOn = isOn;
        loadImage(m_isOn ? m_onImg : m_offImg);
    }

signals:
    // 点击后发射当前状态（供互斥使用）
    void clicked(bool isOn);

protected:
    // 鼠标点击事件（核心：切换图片+发射信号）
    void mousePressEvent(QMouseEvent* event) override
    {
        // 👉 新增：打印事件触发，确认是否收到点击
        qDebug() << "[HomeModeLabel] 鼠标按下事件触发！标签地址：" << this

                 <<         "，鼠标位置（标签内）：" << event->pos()

                 <<       "，鼠标按钮：" << event->button();
        if (event->button() == Qt::LeftButton)
        {
            m_isOn = !m_isOn;
            loadImage(m_isOn ? m_onImg : m_offImg);
            emit clicked(m_isOn);
            qDebug() << "[HomeModeLabel] 点击处理完成，当前状态：" << m_isOn;
        }
        QLabel::mousePressEvent(event);
    }
private:
    // 加载图片（处理加载失败）
    void loadImage(const QString& path)
    {
        QPixmap pix(path);
        if (pix.isNull())
        {
            qWarning() << "[HomeModeLabel] 图片加载失败：" << path;
            setStyleSheet("background-color: red;"); // 红色占位提示
            clear();
        }
        else
        {
            setStyleSheet(""); // 清空背景色
            setPixmap(pix);
            qDebug() << "[HomeModeLabel] 图片加载成功：" << path;
        }
    }

    QString m_offImg; // “关”状态图片路径
    QString m_onImg;  // “开”状态图片路径
    bool m_isOn;      // 当前状态（false=关，true=开）
};

#endif // HOMEMODELABEL_H
