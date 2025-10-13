#include "motorcontrolwidget.h"
#include <QLabel>
#include <QVBoxLayout>

MotorControlWidget::MotorControlWidget(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("电机控制功能（待完善）", this);
    label->setStyleSheet("font-size: 24px; color: #666;");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}
