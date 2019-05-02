# flags
QMAKE_CXXFLAGS +=-std=c++0x

# dirs
CONFIG(debug, debug|release) {
    BUILD_DIR = $${OUT_PWD}/debug
}

CONFIG(release, debug|release) {
    BUILD_DIR = $${OUT_PWD}/release
}

DESTDIR = $${BUILD_DIR}/bin

MOC_DIR = $${BUILD_DIR}/moc
OBJECTS_DIR = $${BUILD_DIR}/obj
RCC_DIR = $${BUILD_DIR}/rcc
UI_DIR = $${BUILD_DIR}/ui
