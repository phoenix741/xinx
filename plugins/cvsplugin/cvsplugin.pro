TEMPLATE = lib
CONFIG += debug \
    plugin \
    static
DESTDIR += ./
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
LIBS = -L../../libxinx \
    -lsharedxinx
TARGET = $$qtLibraryTarget(cvsplugin)
unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
INCLUDEPATH += ../../libxinx
HEADERS = pluginsettings.h \
    customizeplugin.h \
    cvsplugin.h
SOURCES = pluginsettings.cpp \
    customizeplugin.cpp \
    cvsplugin.cpp
FORMS += ui/customize.ui
