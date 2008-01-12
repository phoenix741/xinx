TEMPLATE = lib
CONFIG += debug plugin static
QT += xml
LIBS=-L../../bin -lxinxplugins

TARGET = $$qtLibraryTarget(webplugin)

DESTDIR = ../../bin/plugins
MOC_DIR = ../../build/plugins
OBJECTS_DIR += ../../build/plugins
RCC_DIR += ../../build/plugins
UI_DIR += ../../build/plugins

unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}

INCLUDEPATH += ../../include
HEADERS = xmlprettyprinter.h \
    webplugin.h \
    xmlhighlighter.h \
    jshighlighter.h \
    csshighlighter.h \
    xsllistview.h \
    javascriptparser.h \
    editorcompletion.h \
    xmlcompleter.h \
    xslmodelcompleter.h \
    javascriptmodelcompleter.h \
    jscompleter.h \
    csscompleter.h \
    cssmodeldata.h
SOURCES = xmlprettyprinter.cpp \
    webplugin.cpp \
    xmlhighlighter.cpp \
    jshighlighter.cpp \
    csshighlighter.cpp \
    xsllistview.cpp \
    javascriptparser.cpp \
    editorcompletion.cpp \
    xmlcompleter.cpp \
    xslmodelcompleter.cpp \
    javascriptmodelcompleter.cpp \
    jscompleter.cpp \
    csscompleter.cpp \
    cssmodeldata.cpp
