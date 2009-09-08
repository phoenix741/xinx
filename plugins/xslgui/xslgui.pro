include(../../project_mode.pri)

TEMPLATE = lib
CONFIG += plugin
DESTDIR += ../
LIBS = -L../../framework -L../../components -lxinxframework -lxinxcmp
TARGET = xslgui
INCLUDEPATH += ../../framework ../../components
RESOURCES = xslgui.qrc
TRANSLATIONS += translations/xslgui_fr.ts
HEADERS = xslgui.h \
 xuieditor.h
SOURCES = xslgui.cpp \
 xuieditor.cpp
win32 : RC_FILE += rc/plugin.rc

