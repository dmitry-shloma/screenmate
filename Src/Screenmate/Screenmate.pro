#
PROJECT_DIR = $${PWD}/../
LIB_DIR = $${OUT_PWD}/../Bin

BUILD_ESSENTIAL_DIR = $${PROJECT_DIR}/BuildEssential
include($${BUILD_ESSENTIAL_DIR}/environment.pri)
#

QT += core gui

TARGET = Screenmate
TEMPLATE = app

SOURCES += \
    main.cpp \
    screenmatewidget.cpp

HEADERS += \
    screenmatewidget.h

FORMS += \
    screenmatewidget.ui

RESOURCES += \
    resource.qrc
