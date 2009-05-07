CONFIG += debug
unix {
    QMAKE_LFLAGS = -rdynamic
}

MOC_DIR = .
OBJECTS_DIR = .
RCC_DIR = .

QMAKE_CXXFLAGS_RELEASE = "-Os"

PRECOMPILED_HEADER = stable_header.h
