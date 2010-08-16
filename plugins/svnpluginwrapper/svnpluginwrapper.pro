include(../../project_mode.pri)

TEMPLATE = lib
VERSION = 0.1.0.1
CONFIG += plugin
DESTDIR += ../
LIBS = -L../../framework -lxinxframework
TARGET = svnpluginwrapper
INCLUDEPATH += ../../framework ../../components
HEADERS = rcs_svn.h \
    svnpluginwrapper.h
SOURCES = rcs_svn.cpp \
    svnpluginwrapper.cpp
RESOURCES = svnpluginwrapper.qrc
TRANSLATIONS += translations/svnpluginwrapper_fr.ts
win32 : RC_FILE += rc/plugin.rc

