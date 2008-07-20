TEMPLATE = lib
CONFIG += plugin
DESTDIR += ../
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
LIBS = -L../../libxinx -lsharedxinx
TARGET = $$qtLibraryTarget(xslgui)
INCLUDEPATH += ../../libxinx 
RESOURCES = xslgui.qrc
TRANSLATIONS += translations/xslgui_fr.ts
HEADERS = xslgui.h \
 xuieditor.h
SOURCES = xslgui.cpp \
 xuieditor.cpp
include(../../project_mode.pro)