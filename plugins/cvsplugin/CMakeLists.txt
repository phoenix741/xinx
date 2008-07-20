include(${QT_USE_FILE})

set(cvsplugin_SRCS
	customizeplugin.cpp
	cvsplugin.cpp
	cvsthread.cpp
	entries.cpp
	pluginsettings.cpp
	rcs_cvs.cpp
)

SET(cvsplugin_MOC_HDRS
	customizeplugin.h
	cvsplugin.h
	cvsthread.h
	entries.h
	pluginsettings.h
	rcs_cvs.h
)

SET(cvsplugin_MOC_QRCS
	cvsplugin.qrc
)

SET(cvsplugin_UIS
	ui/customize.ui
)

include_directories(../../libxinx ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})

QT4_WRAP_CPP(cvsplugin_MOC_SRCS ${cvsplugin_MOC_HDRS})
QT4_WRAP_UI(cvsplugin_UIS_H ${cvsplugin_UIS})
QT4_ADD_RESOURCES(cvsplugin_QRC_H ${cvsplugin_MOC_QRCS})

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

add_library(cvsplugin SHARED ${cvsplugin_SRCS} ${cvsplugin_MOC_SRCS} ${cvsplugin_UIS_H} ${cvsplugin_QRC_H})
link_directories(../../libxinx)
target_link_libraries(cvsplugin ${QT_LIBRARIES} sharedxinx)
