set(QT_USE_QTXML TRUE)

include(${QT_USE_FILE})

set(baseplugin_SRCS
    xmlprettyprinter.cpp 
    baseplugin.cpp 
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
)

SET(baseplugin_MOC_HDRS
    baseplugin.h 
    xsllistview.h 
    javascriptparser.h 
    xmlcompleter.h 
    xslmodelcompleter.h 
    javascriptmodelcompleter.h 
    jscompleter.h 
)

include_directories(../../include ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})

QT4_WRAP_CPP(baseplugin_MOC_SRCS ${baseplugin_MOC_HDRS})

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

add_library(xinx_baseplugin SHARED ${baseplugin_SRCS} ${baseplugin_MOC_SRCS})
target_link_libraries(xinx_baseplugin ${QT_LIBRARIES})

install(TARGETS xinx_baseplugin DESTINATION bin/plugins)

