TEMPLATE = lib
CONFIG += plugin 
DESTDIR += ../
LIBS = -L../../libxinx -lsharedxinx
TARGET = $$qtLibraryTarget(cvsplugin)
INCLUDEPATH += ../../libxinx ../../components
HEADERS = entries.h \
    cvsthread.h \
    rcs_cvs.h \
    pluginsettings.h \
    customizeplugin.h \
    cvsplugin.h
SOURCES = entries.cpp \
    cvsthread.cpp \
    rcs_cvs.cpp \
    pluginsettings.cpp \
    customizeplugin.cpp \
    cvsplugin.cpp
FORMS += ui/customize.ui
RESOURCES = cvsplugin.qrc
TRANSLATIONS += translations/cvsplugin_fr.ts
win32 : RC_FILE += rc/plugin.rc

include(../../project_mode.pro)