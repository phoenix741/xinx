TEMPLATE = lib
CONFIG += debug plugin static
QT += xml

DESTDIR += ./
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./

LIBS=-L../../libxinx -lsharedxinx

TARGET = $$qtLibraryTarget(cvsplugin)

unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}

INCLUDEPATH += ../../libxinx
HEADERS = cvsplugin.h
SOURCES = cvsplugin.cpp