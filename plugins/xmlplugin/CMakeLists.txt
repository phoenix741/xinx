include(${QT_USE_FILE})

set(xmlplugin_SRCS
	xmlplugin.cpp
)

SET(xmlplugin_MOC_HDRS
	xmlplugin.h
)

include_directories(../../src ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})

QT4_WRAP_CPP(xmlplugin_MOC_SRCS ${xmlplugin_MOC_HDRS})

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

add_library(xinx_xmlplugin SHARED ${xmlplugin_SRCS} ${xmlplugin_MOC_SRCS})
target_link_libraries(xinx_xmlplugin ${QT_LIBRARIES})

install(TARGETS xinx_xmlplugin DESTINATION bin/plugins)

