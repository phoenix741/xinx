TEMPLATE = lib
CONFIG += plugin \
    static \
    staticlib \
    debug
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
HEADERS =xsl/xmlprettyprinter.h \
    webplugin.h \
    xsl/xmlhighlighter.h \
    js/jshighlighter.h \
    css/csshighlighter.h \
    xsl/xsllistview.h \
    js/javascriptparser.h \
    editorcompletion.h \
    css/cssmodeldata.h \
    xsl/xmltexteditor.h \
	xsl/xslmodelcompleter.h \
	xsl/stylesheeteditor.h \
	xsl/xmlfileeditor.h \
xsl/htmlfileeditor.h \
js/javascriptmodelcompleter.h \
js/jsfileeditor.h \
js/jstexteditor.h \
css/cssfileeditor.h \
css/csstexteditor.h
SOURCES = xsl/xmlprettyprinter.cpp \
    webplugin.cpp \
    xsl/xmlhighlighter.cpp \
    js/jshighlighter.cpp \
    css/csshighlighter.cpp \
    xsl/xsllistview.cpp \
    js/javascriptparser.cpp \
    editorcompletion.cpp \
    css/cssmodeldata.cpp \
    xsl/xmltexteditor.cpp \
	xsl/xslmodelcompleter.cpp \
 	xsl/stylesheeteditor.cpp \
 	xsl/xmlfileeditor.cpp \
 xsl/htmlfileeditor.cpp \
 js/javascriptmodelcompleter.cpp \
 js/jsfileeditor.cpp \
 js/jstexteditor.cpp \
 css/cssfileeditor.cpp \
 css/csstexteditor.cpp
TRANSLATIONS += translations/webplugin_fr.ts

SOURCES -= javascriptmodelcompleter.cpp

