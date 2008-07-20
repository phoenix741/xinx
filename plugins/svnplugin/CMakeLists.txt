include(${QT_USE_FILE})

set(svnplugin_SRCS
	rcs_svn.cpp
	svnplugin.cpp
)

SET(svnplugin_MOC_HDRS
	rcs_svn.h
	svnplugin.h
)

SET(svnplugin_MOC_QRCS
	svnplugin.qrc
)

include_directories(../../libxinx ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})

QT4_WRAP_CPP(svnplugin_MOC_SRCS ${svnplugin_MOC_HDRS})
QT4_ADD_RESOURCES(svnplugin_QRC_H ${svnplugin_MOC_QRCS})

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

add_library(svnplugin SHARED ${svnplugin_SRCS} ${svnplugin_MOC_SRCS} ${svnplugin_QRC_H})
link_directories(../../libxinx)
target_link_libraries(svnplugin ${QT_LIBRARIES} sharedxinx)

