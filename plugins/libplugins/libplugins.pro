TEMPLATE = lib
CONFIG += debug static
TARGET = $$qtLibraryTarget(xinxplugins)
DESTDIR = ../../bin
MOC_DIR = ../../build
OBJECTS_DIR += ../../build
RCC_DIR += ../../build
UI_DIR += ../../build
unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
INCLUDEPATH += ../../include
HEADERS = ../../include/exceptions.h \
    ../../include/filecontentstructure.h
SOURCES = exceptions.cpp \
    filecontentstructure.cpp

