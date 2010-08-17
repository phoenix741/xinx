include(../../project_mode.pri)

TEMPLATE = lib
VERSION = 0.1.0.1
CONFIG += plugin
DESTDIR += ../

LIBS = -L../../framework -lxinxframework -lsvncpp
# apr
LIBS += -lapr-1 -laprutil-1 -lapriconv-1 -lxml
# svn
LIBS += -lsvn_client-1 -lsvn_wc-1 -lsvn_ra-1 -lsvn_delta-1 -lsvn_subr-1

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

