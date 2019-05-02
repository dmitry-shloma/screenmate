include($$PWD/../environment.pri)

QT += core gui

TARGET = screenmate
TEMPLATE = app

HEADERS += \
    screenmatewidget.h

SOURCES += \
    main.cpp \
    screenmatewidget.cpp

RESOURCES += \
    resource.qrc

OTHER_FILES += \
    screenmate.conf

#copyFiles($${PWD}, $${DESTDIR}, Screenmate.conf)
