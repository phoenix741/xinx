include(../../project_mode.pri)

TEMPLATE = lib
CONFIG += plugin
DESTDIR += ../
LIBS = -L../../libxinx -lsharedxinx
TARGET = empty
INCLUDEPATH += ../../libxinx 
RESOURCES = empty.qrc
TRANSLATIONS += translations/empty_fr.ts
HEADERS = empty.h
SOURCES = empty.cpp
win32 : RC_FILE += rc/plugin.rc
