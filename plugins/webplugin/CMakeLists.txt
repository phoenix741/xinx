set(QT_USE_QTXML TRUE)
set(QT_USE_QTNETWORK TRUE)

include(${QT_USE_FILE})

set(webplugin_SRCS
	config/selfwebpluginsettings.cpp
	config/webpluginformimpl.cpp
	config/webpluginsettings.cpp
	css/cssfileeditor.cpp
	css/csshighlighter.cpp
	css/cssmodeldata.cpp
	css/csstexteditor.cpp
	editorcompletion.cpp
	js/javascriptmodelcompleter.cpp
	js/javascriptparser.cpp
	js/jsfileeditor.cpp
	js/jshighlighter.cpp
	js/jstexteditor.cpp
	webplugin.cpp
	xsl/htmlfileeditor.cpp
	xsl/stylesheeteditor.cpp
	xsl/xmlfileeditor.cpp
	xsl/xmlhighlighter.cpp
	xsl/xmlprettyprinter.cpp
	xsl/xmltexteditor.cpp
	xsl/xsllistview.cpp
	xsl/xslmodelcompleter.cpp
)

SET(webplugin_MOC_HDRS
	config/selfwebpluginsettings.h
	config/webpluginformimpl.h
	config/webpluginsettings.h
	css/cssfileeditor.h
	css/csshighlighter.h
	css/cssmodeldata.h
	css/csstexteditor.h
	editorcompletion.h
	js/javascriptmodelcompleter.h
	js/javascriptparser.h
	js/jsfileeditor.h
	js/jshighlighter.h
	js/jstexteditor.h
	webplugin.h
	xsl/htmlfileeditor.h
	xsl/stylesheeteditor.h
	xsl/xmlfileeditor.h
	xsl/xmlhighlighter.h
	xsl/xmlprettyprinter.h
	xsl/xmltexteditor.h
	xsl/xsllistview.h
	xsl/xslmodelcompleter.h
)

SET(webplugin_UIS
	ui/webpluginform.ui
)

include_directories(../../libxinx ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})

QT4_WRAP_CPP(webplugin_MOC_SRCS ${webplugin_MOC_HDRS})
QT4_WRAP_UI(webplugin_UIS_H ${webplugin_UIS})
QT4_ADD_RESOURCES(webplugin_QRC_H webplugin.qrc)

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)

add_library(webplugin STATIC ${webplugin_SRCS} ${webplugin_MOC_SRCS} ${webplugin_QRC_H} ${webplugin_UIS_H})
target_link_libraries(webplugin ${QT_LIBRARIES})

