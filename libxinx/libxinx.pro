TEMPLATE = lib
DESTDIR += ./
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
TARGET = $$qtLibraryTarget(sharedxinx)
unix {
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
win32 : QMAKE_LFLAGS_SHLIB *= -no-undefined \
    -enable-runtime-pseudo-reloc
HEADERS = serialization.h \
    textfileeditor.h \
    numberbar.h \
    texteditor.h \
    borderlayout.h \
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
 itemmodelfileeditor.h
SOURCES = serialization.cpp \
    textfileeditor.cpp \
    numberbar.cpp \
    texteditor.cpp \
    borderlayout.cpp \
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
 itemmodelfileeditor.cpp
TRANSLATIONS += translations/libxinx_fr.ts
CONFIG += dll \
 debug
QT += xml

SOURCES -= xinxcontainer.cpp \
 serialization.cpp

HEADERS -= xinxcontainer.h \
 serialization.h

