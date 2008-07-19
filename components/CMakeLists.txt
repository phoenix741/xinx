include(${QT_USE_FILE})

set(xinxcmp_SRCS
	directoryedit.cpp
	kcolorcombo.cpp
	xinxlistwidgetimpl.cpp
	xinxpluginselector.cpp
)

SET(xinxcmp_MOC_HDRS
	directoryedit.h
	kcolorcombo.h
	p_xinxpluginselector.h
	xinxlistwidgetimpl.h
	xinxpluginselector.h
)

include_directories( . ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} )

QT4_WRAP_CPP(xinxcmp_MOC_SRCS ${xinxcmp_MOC_HDRS})

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

add_library(xinxcmp STATIC ${xinxcmpx_SRCS} ${xinxcmp_MOC_SRCS})
target_link_libraries(xinxcmp ${QT_LIBRARIES})




