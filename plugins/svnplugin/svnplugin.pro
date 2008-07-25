TEMPLATE = lib
CONFIG += plugin
QT += xml
DESTDIR += ../
LIBS = -L../../libxinx -lsharedxinx
TARGET = $$qtLibraryTarget(svnplugin)
INCLUDEPATH += ../../libxinx
HEADERS = rcs_svn.h \
    svnplugin.h
SOURCES = rcs_svn.cpp \
    svnplugin.cpp
RESOURCES = svnplugin.qrc
TRANSLATIONS += translations/svnplugin_fr.ts
include(../../project_mode.pro)