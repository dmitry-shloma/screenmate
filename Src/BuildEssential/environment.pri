# flags
QMAKE_CXXFLAGS +=-std=c++0x

# dirs
CONFIG(debug, debug|release) {
    BUILD_DIR = $${OUT_PWD}/Debug
    DESTDIR = $${OUT_PWD}/../Bin
}

CONFIG(release, debug|release) {
    BUILD_DIR = $${OUT_PWD}/Release
    DESTDIR = $${OUT_PWD}/../Bin
}

MOC_DIR = $${BUILD_DIR}/Moc
OBJECTS_DIR = $${BUILD_DIR}/Obj
RCC_DIR = $${BUILD_DIR}/Rcc
UI_DIR = $${BUILD_DIR}/Ui
