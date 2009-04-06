CONFIG += debug
unix {
    QMAKE_LFLAGS = -rdynamic
}

MOC_DIR = .
OBJECTS_DIR = .
RCC_DIR = .

QMAKE_CXXFLAGS_RELEASE = "-Os"
QMAKE_CXXFLAGS_DEBUG = "-O0"

#CONFIG += precompile_header
PRECOMPILED_HEADER = stable_header.h
