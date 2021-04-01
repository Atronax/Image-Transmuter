TARGET = ImageTransmuter
TEMPLATE = app

CONFIG += c++14 c++17

QT += core gui widgets

SOURCES += main.cpp \
    Helpers/imageconversionthread.cpp \
    Helpers/imagepreviewwidget.cpp \
    gui.cpp \
    historylabel.cpp \
    logic.cpp \
    manager.cpp

HEADERS += \
    Helpers/imageconversionthread.h \
    Helpers/imagepreviewwidget.h \
    gui.h \
    historylabel.h \
    logic.h \
    manager.h \
    types.h
