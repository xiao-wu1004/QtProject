QT       += core gui network multimedia widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    button_custom.cpp \
    button_home_start.cpp \
    industrywidget.cpp \
    main.cpp \
    messagewidget.cpp \
    motorcontrolwidget.cpp \
    musicsearchdialog.cpp \
    smartdialogwidget.cpp \
    smarthomewidget.cpp \
    visualrecogwidget.cpp \
    weatherwidget.cpp \
    widget_assist_backdrop.cpp \
    widget_assist_menu.cpp \
    widget_main.cpp

HEADERS += \
    button_custom.h \
    button_home_start.h \
    homemodelabel.h \
    industrywidget.h \
    messagewidget.h \
    motorcontrolwidget.h \
    musicsearchdialog.h \
    smartdialogwidget.h \
    smarthomewidget.h \
    visualrecogwidget.h \
    weatherwidget.h \
    widget_assist_backdrop.h \
    widget_assist_menu.h \
    widget_main.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    file.qrc \
    fonts.qrc

DISTFILES +=
