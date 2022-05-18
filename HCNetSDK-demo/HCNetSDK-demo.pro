TARGET = HCNetSDK-demo_internal

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    widget.cpp

HEADERS += \
    widget.h

FORMS += \
    widget.ui

INCLUDEPATH += $$PWD/../thirdparty/HCNetSDK/include
LIBS += -L$$PWD/../thirdparty/HCNetSDK/lib
LIBS += -lHCNetSDK -lGdiPlus -lHCCore -lPlayCtrl

DESTDIR = $$PWD/../dist/
