#-------------------------------------------------
#
# Project created by QtCreator 2015-09-04T20:13:07
#
#-------------------------------------------------

# BuildEssentials dir
BUILD_ESSENTIALS_DIR = $$PWD/../BuildEssentials

include($${BUILD_ESSENTIALS_DIR}/deployment.pri)
qtcAddDeployment()

include($${BUILD_ESSENTIALS_DIR}/environment.pri)
include($${BUILD_ESSENTIALS_DIR}/helper.pri)
#

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Screenmate
TEMPLATE = app
VERSION = 0.0.1

SOURCES += \
    main.cpp \
    screenmatewidget.cpp

HEADERS += \
    screenmatewidget.h

FORMS += \
    screenmatewidget.ui

RESOURCES += \
    resource.qrc
