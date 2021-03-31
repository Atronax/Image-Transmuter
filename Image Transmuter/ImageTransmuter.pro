TARGET = ImageTransmuter
TEMPLATE = app

CONFIG += c++14 c++17

QT += core gui widgets

SOURCES += main.cpp \
    Helpers/imageconversionthread.cpp \
    Helpers/imagepreviewwidget.cpp \
    controller.cpp \
    gui.cpp \
    logic.cpp

HEADERS += \
    Helpers/imageconversionthread.h \
    Helpers/imagepreviewwidget.h \
    controller.h \
    gui.h \
    logic.h \
    types.h
