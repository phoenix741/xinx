TEMPLATE = lib
CONFIG += plugin \
    staticlib
QT += xml
DESTDIR += ../
RESOURCES += webplugin.qrc
TARGET = webplugin
INCLUDEPATH += ./ \
    ../../libxinx \
    ../../components \
    ../../ext/qcodeedit/lib \
    ../../ext/qcodeedit/lib/document \
    ../../ext/qcodeedit/lib/language \
    ../../ext/qcodeedit/lib/qnfa \
    ../../ext/qcodeedit/lib/widgets
HEADERS = webplugin.h \
    filetypeplugin.h \
    config/webpluginformimpl.h \
    config/selfwebpluginsettings.h \
	config/webpluginsettings.h \
    xq/xquery_keyword.h \
    xq/xqtexteditor.h \
    xq/xqfileeditor.h \
    xq/xqmodelcompleter.h \
    xq/xqformatscheme.h \
    css/cssmodeldata.h \
    css/cssfileeditor.h \
    css/csstexteditor.h \
    css/cssformatscheme.h \
	js/jsfileeditor.h \
    js/jstexteditor.h \
    js/jsformatscheme.h \
    js/jscontentviewparser.h \
	js/javascriptmodelcompleter.h \
    xsl/xmlformatscheme.h \
	xsl/xmlprettyprinter.h \
    xsl/xmltexteditor.h \
    xsl/htmlfileeditor.h \
    xsl/xslcontentviewparser.h \
    xsl/xslcompletionnodemodel.h
SOURCES = webplugin.cpp \
    filetypeplugin.cpp \
    config/selfwebpluginsettings.cpp \
    config/webpluginsettings.cpp \
    config/webpluginformimpl.cpp \
    css/cssmodeldata.cpp \
    css/cssfileeditor.cpp \
    css/csstexteditor.cpp \
    css/cssformatscheme.cpp \
	xq/xquery_keyword.cpp \
    xq/xqtexteditor.cpp \
    xq/xqfileeditor.cpp \
    xq/xqmodelcompleter.cpp \
    xq/xqformatscheme.cpp \
    js/jsfileeditor.cpp \
    js/jstexteditor.cpp \
    js/jsformatscheme.cpp \
    js/jscontentviewparser.cpp \
	js/javascriptmodelcompleter.cpp \
    xsl/xslcontentviewparser.cpp \
    xsl/xmlformatscheme.cpp \
    xsl/xmlprettyprinter.cpp \
    xsl/xmltexteditor.cpp \
    xsl/htmlfileeditor.cpp \
    xsl/xslcompletionnodemodel.cpp 
TRANSLATIONS += translations/webplugin_fr.ts
FORMS += ui/webpluginform.ui
include(../../project_mode.pro)
