include(../../project_mode.pri)

TEMPLATE = lib
VERSION = 0.1.0.1
CONFIG += plugin
DESTDIR += ../
LIBS = -L../../libxinx -lsharedxinx
TARGET = svnplugin
INCLUDEPATH += ../../libxinx ../../components
HEADERS = rcs_svn.h \
    svnplugin.h
SOURCES = rcs_svn.cpp \
    svnplugin.cpp
RESOURCES = svnplugin.qrc
TRANSLATIONS += translations/svnplugin_fr.ts
win32 : RC_FILE += rc/plugin.rc

