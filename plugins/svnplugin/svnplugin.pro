include(../../project_mode.pri)

TEMPLATE = lib
VERSION = 0.1.0.1
CONFIG += plugin
DESTDIR += ../
LIBS = -L../../framework -lxinxframework -lsvncpp
TARGET = svnplugin
INCLUDEPATH += ../../framework ../../components
unix : INCLUDEPATH += /usr/include/subversion-1/ /usr/include/apr-1.0/
HEADERS = rcs_svn.h \
    svnplugin.h
SOURCES = rcs_svn.cpp \
    svnplugin.cpp
RESOURCES = svnplugin.qrc
TRANSLATIONS += translations/svnplugin_fr.ts
win32 : RC_FILE += rc/plugin.rc

