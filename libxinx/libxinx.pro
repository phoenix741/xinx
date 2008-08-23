TEMPLATE = lib
CONFIG += dll
QT += xml
TARGET = $$qtLibraryTarget(sharedxinx)
DESTDIR += ./
INCLUDEPATH += ../components
LIBS = -L../components -lxinxcmp
PRE_TARGETDEPS = ../components/libxinxcmp.a
win32 : QMAKE_LFLAGS_SHLIB *= -no-undefined -enable-runtime-pseudo-reloc
win32 : RC_FILE += rc/libxinx.rc
HEADERS = textfileeditor.h \
    numberbar.h \
    texteditor.h \
    filewatcher.h \
    p_filewatcher.h \
    filecontentitemmodel.h \
    syntaxhighlighter.h \
    appsettings.h \
    xinxconfig.h \
    xinxpluginsloader.h \
    xinxcore.h \
    xinxthread.h \
    rcs.h \
    exceptions.h \
    filecontentstructure.h \
    abstracteditor.h \
    xslproject.h \
    abstractfileeditor.h \
    itemmodelfileeditor.h \
    xinxpluginelement.h \
 plugininterfaces.h
SOURCES = textfileeditor.cpp \
    numberbar.cpp \
    texteditor.cpp \
    filewatcher.cpp \
    filecontentitemmodel.cpp \
    syntaxhighlighter.cpp \
    appsettings.cpp \
    xinxconfig.cpp \
    xinxpluginsloader.cpp \
    xinxcore.cpp \
    xinxthread.cpp \
    rcs.cpp \
    exceptions.cpp \
    filecontentstructure.cpp \
    abstracteditor.cpp \
    xslproject.cpp \
    abstractfileeditor.cpp \
    itemmodelfileeditor.cpp \
    xinxpluginelement.cpp
TRANSLATIONS += translations/libxinx_fr.ts

include(../project_mode.pro)