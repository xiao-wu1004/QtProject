#include "widget_main.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    // 解决 Qt 高分辨率屏幕显示错误问题
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);    // 启用高 DPI 缩放支持
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);       // 启用高 DPI 位图支持

    QApplication a(argc, argv);
    Widget_Main w;
    w.show();
    return a.exec();
}
