TEMPLATE = lib
CONFIG += plugin 
DESTDIR += ../
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
LIBS = -L../../libxinx -lsharedxinx
TARGET = $$qtLibraryTarget(cvsplugin)
INCLUDEPATH += ../../libxinx
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

include(../../project_mode.pro)