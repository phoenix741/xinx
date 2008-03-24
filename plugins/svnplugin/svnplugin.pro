TEMPLATE = lib
CONFIG += debug plugin shared
QT += xml

DESTDIR += ../
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./

LIBS=-L../../libxinx -lsharedxinx

TARGET = $$qtLibraryTarget(svnplugin)

unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}

INCLUDEPATH += ../../libxinx
HEADERS = svnplugin.h
SOURCES = svnplugin.cpp