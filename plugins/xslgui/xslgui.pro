TEMPLATE = lib
CONFIG += plugin
DESTDIR += ../
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
LIBS = -L../../libxinx -lsharedxinx
TARGET = $$qtLibraryTarget(xslgui)
unix {
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
INCLUDEPATH += ../../libxinx 
RESOURCES = xslgui.qrc
TRANSLATIONS += translations/xslgui_fr.ts
HEADERS = xslgui.h \
 xuieditor.h
SOURCES = xslgui.cpp \
 xuieditor.cpp
include(../../project_mode.pro)