TEMPLATE = lib
CONFIG += plugin
DESTDIR += ../
LIBS = -L../../libxinx -lsharedxinx
TARGET = $$qtLibraryTarget(empty)
INCLUDEPATH += ../../libxinx 
RESOURCES = empty.qrc
TRANSLATIONS += translations/empty_fr.ts
HEADERS = empty.h
SOURCES = empty.cpp
include(../../project_mode.pro)