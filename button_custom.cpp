#include "button_custom.h"
#include <QApplication> // 用于 exit(0) 函数

// ========================== 按钮基类实现（原 button_cust_base.cpp）==========================
Button_Cust_Base::Button_Cust_Base(QWidget* parent) : QLabel(parent)
{
}

void Button_Cust_Base::Set_Image(QString Image_path)
{
    // 设定图像（原逻辑：保存 QImage 并触发重绘）
    this->M_Image = QImage(Image_path);
    this->update();
}

void Button_Cust_Base::Set_Test(QString Text)
{
    // 设定文本（原逻辑：保存字符串并触发重绘）
    this->M_Text = Text;
    this->update();
}

void Button_Cust_Base::update_start(int Mode_Code)
{
    // 更新状态码（原逻辑：保存状态并触发重绘）
    this->Mode_Code = Mode_Code;
    this->update();
}

void Button_Cust_Base::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        // 处理控件内的鼠标移动事件，不让事件传递给父窗口（原逻辑）
        //event->accept();
        event->ignore();
    }
}

// ========================== 关闭按钮实现（原 button_main_close.cpp）==========================
Button_Main_Close::Button_Main_Close(QWidget* parent) : Button_Cust_Base(parent)
{
    // 原初始化逻辑：固定15x15尺寸、缩放图片、初始蓝色图标
    this->setFixedSize(15, 15);
    this->setScaledContents(true);
    this->setPixmap(QPixmap(":/关闭按钮_蓝色"));
}

void Button_Main_Close::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
    // 原逻辑：鼠标进入切换红色图标
    this->setPixmap(QPixmap(":/关闭按钮_红色"));
}

void Button_Main_Close::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    // 原逻辑：鼠标离开切换蓝色图标
    this->setPixmap(QPixmap(":/关闭按钮_蓝色"));
}

void Button_Main_Close::mousePressEvent(QMouseEvent* event)
{
    // 原逻辑：左键按下切换灰色图标（忽略右键/中键）
    Qt::MouseButton button = event->button();
    if (button == Qt::LeftButton)
    {
        this->setPixmap(QPixmap(":/关闭按钮_灰色"));
        event->accept(); // 拦截事件，不传递给父窗口
    }
}

void Button_Main_Close::mouseReleaseEvent(QMouseEvent* event)
{
    Qt::MouseButton button = event->button();
    // 原逻辑：获取全局坐标，判断鼠标是否在控件内
    QPoint mousePos = event->globalPos();
    QRect widgetRect = this->rect();

    if (widgetRect.contains(this->mapFromGlobal(mousePos)))
    {
        // 左键释放且在控件内：切换红色图标并退出应用
        if (button == Qt::LeftButton)
        {
            this->setPixmap(QPixmap(":/关闭按钮_红色"));
            exit(0);
        }
    }
}

// ========================== 菜单按钮实现（原 button_main_menu.cpp）==========================
Button_Main_Menu::Button_Main_Menu(QWidget* parent) : Button_Cust_Base(parent)
{
    // 原初始化逻辑：固定120x30尺寸、默认图标/文本
    this->setFixedSize(QSize(120, 30));
    this->M_Image = QImage(":/未定义菜单");
    this->M_Text = "还未设置";
}

void Button_Main_Menu::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 原逻辑：开启抗锯齿

    QPen pen;
    QBrush Brush;

    /***************************** 按状态绘制背景（原逻辑完全保留）*****************************/
    switch (this->Mode_Code)
    {
        case Mode_enter:    // 鼠标进入
            {
                pen.setColor(QColor(107, 194, 252));
                pen.setWidth(0);
                Brush.setColor(QColor(107, 194, 252));
                Brush.setStyle(Qt::SolidPattern);

                painter.setPen(pen);
                painter.setBrush(Brush);

                QRect rect = this->rect();
                painter.drawRoundedRect(rect, 8, 8); // 圆角8px
                rect.setX(rect.x() + rect.width() - 30); // 右侧30px无圆角
                rect.setWidth(30);
                painter.drawRect(rect);
            }
            break;

        case Mode_leave:    // 鼠标离开（原逻辑：空实现）
            {
            }
            break;

        case Mode_Press:    // 鼠标按下
            {
                pen.setColor(QColor(163, 216, 251));
                pen.setWidth(0);
                Brush.setColor(QColor(163, 216, 251));
                Brush.setStyle(Qt::SolidPattern);

                painter.setPen(pen);
                painter.setBrush(Brush);

                QRect rect = this->rect();
                painter.drawRoundedRect(rect, 8, 8);
                rect.setX(rect.x() + rect.width() - 30);
                rect.setWidth(30);
                painter.drawRect(rect);
            }
            break;

        case Mode_Release:  // 鼠标释放（原逻辑：空实现）
            break;
    }

    /***************************** 绘制文本和图标（原逻辑完全保留）*****************************/
    pen.setColor(Qt::white);
    painter.setPen(pen);
    painter.setFont(QFont("隶书", 13));
    painter.drawText(40, 22, this->M_Text); // 文本位置：左40，下22

    QImage image = this->M_Image.scaled(27, 27); // 图标缩放27x27
    painter.drawImage(3, 1, image); // 图标位置：左3，上1
}

void Button_Main_Menu::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
    // 原逻辑：按下状态不更新，否则设为 Mode_enter
    if (this->Mode_Code == Mode_Press)
    {
        return;
    }
    this->Mode_Code = Mode_enter;
    this->update();
}

void Button_Main_Menu::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    // 原逻辑：按下状态不更新，否则设为 Mode_leave
    if (this->Mode_Code == Mode_Press)
    {
        return;
    }
    this->Mode_Code = Mode_leave;
    this->update();
}

void Button_Main_Menu::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    // 原逻辑：设为 Mode_Press，发射点击信号，触发重绘
    this->Mode_Code = Mode_Press;
    emit signals_click();

    this->update();

    // 2. 拦截事件，不传递给父窗口（关键！）
    event->accept();
}

void Button_Main_Menu::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    event->accept(); // 关键：拦截移动事件，不传递给父窗口
}
void Button_Main_Menu::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    // 原逻辑：按下状态不更新，否则设为 Mode_Press
    if (this->Mode_Code == Mode_Press)
    {
        return;
    }
    this->Mode_Code = Mode_Press;
    this->update();
    event->accept(); // 关键：拦截释放事件，不传递给父窗口
}

