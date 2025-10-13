#include "visualrecogwidget.h"
#include <QLabel>
#include <QVBoxLayout>

VisualRecogWidget::VisualRecogWidget(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("视觉识别功能（待完善）", this);
    label->setStyleSheet("font-size: 24px; color: #666;");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}
