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
HEADERS = xsl/xmlformatscheme.h \
    xq/xquery_keyword.h \
    xsl/xsltexteditor.h \
    config/webpluginsettings.h \
    xsl/xmlprettyprinter.h \
    webplugin.h \
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
    css/csstexteditor.h \
    config/webpluginformimpl.h \
    config/selfwebpluginsettings.h \
    xq/xqtexteditor.h \
    xq/xqfileeditor.h \
    xq/xqmodelcompleter.h \
    filetypeplugin.h \
    js/jsformatscheme.h \
    xq/xqformatscheme.h \
    css/cssformatscheme.h \
    xsl/xslcontentviewparser.h \
    js/jscontentviewparser.h
SOURCES = xsl/xmlformatscheme.cpp \
    xq/xquery_keyword.cpp \
    xsl/xsltexteditor.cpp \
    config/selfwebpluginsettings.cpp \
    config/webpluginsettings.cpp \
    xsl/xmlprettyprinter.cpp \
    webplugin.cpp \
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
    css/csstexteditor.cpp \
    xq/xqtexteditor.cpp \
    xq/xqfileeditor.cpp \
    xq/xqmodelcompleter.cpp \
    config/webpluginformimpl.cpp \
    filetypeplugin.cpp \
    js/jsformatscheme.cpp \
    xq/xqformatscheme.cpp \
    css/cssformatscheme.cpp \
    xsl/xslcontentviewparser.cpp \
    js/jscontentviewparser.cpp
TRANSLATIONS += translations/webplugin_fr.ts
FORMS += ui/webpluginform.ui
include(../../project_mode.pro)
