include(${QT_USE_FILE})

set(empty_SRCS
	empty.cpp
)

SET(empty_MOC_HDRS
	empty.h
)

include_directories(../../libxinx ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})

QT4_WRAP_CPP(empty_MOC_SRCS ${empty_MOC_HDRS})
QT4_ADD_RESOURCES(empty_QRC_H empty.qrc)

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

add_library(empty SHARED ${empty_SRCS} ${empty_MOC_SRCS} ${empty_QRC_H})
target_link_libraries(empty ${QT_LIBRARIES})

