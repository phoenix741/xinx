TEMPLATE      = lib
TARGET        = $$qtLibraryTarget(xinx_baseplugin)

DESTDIR     = ../../bin/plugins
MOC_DIR     = ../../build/plugins
OBJECTS_DIR += ../../build/plugins
RCC_DIR += ../../build/plugins
UI_DIR += ../../build/plugins

unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
INCLUDEPATH += ../../src/

CONFIG       += plugin

HEADERS       = baseplugin.h 
SOURCES       = baseplugin.cpp
