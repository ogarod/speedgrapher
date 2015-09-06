QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = speegrapher
TEMPLATE = app

HEADERS += \
    speed-grapher.h \
    demo.h

SOURCES += \
    speed-grapher.cc \
    main.cc \
    demo.cc

CONFIG += c++11
