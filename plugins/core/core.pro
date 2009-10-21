include(../../project_mode.pri)
TEMPLATE = lib
VERSION = 0.9.0.0
CONFIG += plugin \
    staticlib
QT += xml \
    xmlpatterns \
    webkit
DESTDIR += ../
RESOURCES += plugindefinition/coreplugin.qrc
TARGET = coreplugin
INCLUDEPATH += ./ \
    ../../framework \
    ../../components \
    ../../ext/qcodeedit/lib \
    ../../ext/qcodeedit/lib/document \
    ../../ext/qcodeedit/lib/language \
    ../../ext/qcodeedit/lib/qnfa \
    ../../ext/qcodeedit/lib/widgets
HEADERS = docks/datastream/xmlpresentationdockwidget.h \
    docks/datastream/xquerydialogimpl.h \
    docks/datastream/xmlpresentationdockthread.h \
    docks/datastream/xmlpresentationitem.h \
    pluginproperty/webpluginprojectpropertyformimpl.h \
    config/webpluginsettings.h \
    config/editorsconfigformimpl.h \
    config/selfwebpluginsettings.h \
    editors/scheme/xmlformatscheme.h \
    editors/scheme/jsformatscheme.h \
    editors/scheme/cssformatscheme.h \
    editors/scheme/xqformatscheme.h \
    editors/models/xq/xqmodelcompleter.h \
    editors/models/xq/xquery_keyword.h \
    editors/models/xsl/xslcompletionnodemodel.h \
    editors/models/xsl/xslcontentviewparser.h \
    editors/models/js/javascriptmodelcompleter.h \
    editors/models/js/jscontentviewparser.h \
    editors/models/css/cssmodeldata.h \
    editors/parsing/xsltparser.h \
    editors/prettyprint/xmlprettyprinter.h \
    editors/widgeteditor/xq/xqtexteditor.h \
    editors/widgeteditor/xq/xqfileeditor.h \
    editors/widgeteditor/xml/xmltexteditor.h \
    editors/widgeteditor/xml/xmlfileeditor.h \
    editors/widgeteditor/html/htmlfileeditor.h \
    editors/widgeteditor/xsl/xslfileeditor.h \
    editors/widgeteditor/xsl/xsltexteditor.h \
    editors/widgeteditor/js/jsfileeditor.h \
    editors/widgeteditor/js/jstexteditor.h \
    editors/widgeteditor/css/cssfileeditor.h \
    editors/widgeteditor/css/csstexteditor.h \
    plugindefinition/coreplugin.h \
    plugindefinition/filetypeplugin.h \
    actions/stylesheetaction.h \
    actions/commentactions.h
SOURCES = docks/datastream/xmlpresentationdockthread.cpp \
    docks/datastream/xquerydialogimpl.cpp \
    docks/datastream/xmlpresentationdockwidget.cpp \
    docks/datastream/xmlpresentationitem.cpp \
    pluginproperty/webpluginprojectproperyformimpl.cpp \
    config/editorsconfigformimpl.cpp \
    config/selfwebpluginsettings.cpp \
    config/webpluginsettings.cpp \
    editors/scheme/xmlformatscheme.cpp \
    editors/scheme/xqformatscheme.cpp \
    editors/scheme/cssformatscheme.cpp \
    editors/scheme/jsformatscheme.cpp \
    editors/models/xq/xquery_keyword.cpp \
    editors/models/xq/xqmodelcompleter.cpp \
    editors/models/xsl/xslcompletionnodemodel.cpp \
    editors/models/xsl/xslcontentviewparser.cpp \
    editors/models/js/javascriptmodelcompleter.cpp \
    editors/models/js/jscontentviewparser.cpp \
    editors/models/css/cssmodeldata.cpp \
    editors/parsing/xsltparser.cpp \
    editors/prettyprint/xmlprettyprinter.cpp \
    editors/widgeteditor/xq/xqfileeditor.cpp \
    editors/widgeteditor/xq/xqtexteditor.cpp \
    editors/widgeteditor/xml/xmltexteditor.cpp \
    editors/widgeteditor/xml/xmlfileeditor.cpp \
    editors/widgeteditor/html/htmlfileeditor.cpp \
    editors/widgeteditor/xsl/xslfileeditor.cpp \
    editors/widgeteditor/xsl/xsltexteditor.cpp \
    editors/widgeteditor/js/jstexteditor.cpp \
    editors/widgeteditor/js/jsfileeditor.cpp \
    editors/widgeteditor/css/cssfileeditor.cpp \
    editors/widgeteditor/css/csstexteditor.cpp \
    plugindefinition/coreplugin.cpp \
    plugindefinition/filetypeplugin.cpp \
    actions/stylesheetaction.cpp \
    actions/commentactions.cpp
TRANSLATIONS += translations/coreplugin_fr.ts
FORMS += config/editorsconfigform.ui \
    config/webpluginprojectpropertyform.ui \
    config/xmlpresconfigform.ui \
    docks/datastream/xquery.ui \
    docks/datastream/xmlpresentationwidget.ui
unix:INCLUDEPATH += /usr/include/libxml2
