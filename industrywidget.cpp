#include "industrywidget.h"
#include <QLabel>
#include <QVBoxLayout>

IndustryWidget::IndustryWidget(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("工控大屏功能（待完善）", this);
    label->setStyleSheet("font-size: 24px; color: #666;");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}
