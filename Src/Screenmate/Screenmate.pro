#
PROJECT_DIR = $${PWD}/../

BUILD_ESSENTIAL_DIR = $${PROJECT_DIR}/BuildEssential
include($${BUILD_ESSENTIAL_DIR}/environment.pri)
include($${BUILD_ESSENTIAL_DIR}/fileoperationhelper.pri)
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

OTHER_FILES += \
    Screenmate.conf

srcFile = $${PWD}/Screenmate.conf
dstFile = $${DESTDIR}/Screenmate.conf
copyFile($${srcFile}, $${dstFile})
