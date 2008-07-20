include(${QT_USE_FILE})

set(xslgui_SRCS
	xslgui.cpp
	xuieditor.cpp
)

SET(xslgui_MOC_HDRS
	xslgui.h
	xuieditor.h
)

SET(xslgui_MOC_QRCS
	xslgui.qrc
)

include_directories(../../libxinx ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})

QT4_WRAP_CPP(xslgui_MOC_SRCS ${xslgui_MOC_HDRS})
QT4_ADD_RESOURCES(xslgui_QRC_H ${xslgui_MOC_QRCS})

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

add_library(xslgui SHARED ${xslgui_SRCS} ${xslgui_MOC_SRCS} ${xslgui_QRC_H})
link_directories(../../libxinx)
target_link_libraries(xslgui ${QT_LIBRARIES} sharedxinx)

