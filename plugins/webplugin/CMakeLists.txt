set(QT_USE_QTXML TRUE)

include(${QT_USE_FILE})

set(webplugin_SRCS
    xmlprettyprinter.cpp 
    webplugin.cpp 
    xmlhighlighter.cpp 
    jshighlighter.cpp 
    csshighlighter.cpp 
    xsllistview.cpp 
    javascriptparser.cpp 
    editorcompletion.cpp 
    xmlcompleter.cpp 
    xslmodelcompleter.cpp 
    javascriptmodelcompleter.cpp 
    jscompleter.cpp 
    csscompleter.cpp
    cssmodeldata.cpp
)

SET(webplugin_MOC_HDRS
    webplugin.h 
    xsllistview.h 
    javascriptparser.h 
    xmlcompleter.h 
    xslmodelcompleter.h 
    javascriptmodelcompleter.h 
    jscompleter.h 
    csscompleter.h
    cssmodeldata.h
)

include_directories(../../include ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})

QT4_WRAP_CPP(webplugin_MOC_SRCS ${webplugin_MOC_HDRS})

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

add_library(webplugin SHARED ${webplugin_SRCS} ${webplugin_MOC_SRCS})
link_directories(bin)
target_link_libraries(webplugin ${QT_LIBRARIES} xinxplugins)

install(TARGETS webplugin DESTINATION bin/plugins)

