TEMPLATE = lib
CONFIG += plugin \
    static
QT += xml
DESTDIR += ../
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
RESOURCES += webplugin.qrc
TARGET = $$qtLibraryTarget(webplugin)
unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
INCLUDEPATH += ../../libxinx
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
TRANSLATIONS += translations/webplugin_fr.ts
