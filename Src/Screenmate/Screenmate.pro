PROJECT_DIR = $${PWD}/../
BUILD_ESSENTIAL_DIR = $${PROJECT_DIR}/BuildEssential
include($${BUILD_ESSENTIAL_DIR}/environment.pri)
include($${BUILD_ESSENTIAL_DIR}/deploy.pri)

QT += core gui

TARGET = Screenmate
TEMPLATE = app

SOURCES += \
    main.cpp \
    screenmatewidget.cpp

HEADERS += \
    screenmatewidget.h

RESOURCES += \
    resource.qrc

OTHER_FILES += \
    Screenmate.conf

copyFiles($${PWD}, $${DESTDIR}, Screenmate.conf)
