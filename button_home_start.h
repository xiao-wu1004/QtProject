#ifndef BUTTON_HOME_START_H  // 修正宏名，与类名保持一致（更规范）
#define BUTTON_HOME_START_H

#include <QWidget>
#include <QMouseEvent>
#include "button_custom.h"  // 确保路径正确

class Button_home_start : public Button_Cust_Base
{
    Q_OBJECT
public:
    explicit Button_home_start(QWidget* parent = nullptr);

    // 重写鼠标按下事件（明确声明 override，增强可读性）
    void mousePressEvent(QMouseEvent* event) override;

    // 设置状态（参数名更清晰，避免与成员变量混淆）
    void set_start(bool isStart);

signals:
    // 信号：状态变化（true=开，false=关）
    void signals_start(bool start);

protected:
    bool m_start = false;  // ① 用 bool 类型替代 int，更符合布尔状态语义

};

#endif // BUTTON_HOME_START_H
