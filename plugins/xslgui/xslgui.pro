TEMPLATE = lib
CONFIG += plugin
DESTDIR += ../
LIBS = -L../../libxinx -L../../components -lsharedxinx -lxinxcmp
TARGET = $$qtLibraryTarget(xslgui)
INCLUDEPATH += ../../libxinx ../../components
RESOURCES = xslgui.qrc
TRANSLATIONS += translations/xslgui_fr.ts
HEADERS = xslgui.h \
 xuieditor.h
SOURCES = xslgui.cpp \
 xuieditor.cpp
win32 : RC_FILE += rc/plugin.rc

include(../../project_mode.pro)