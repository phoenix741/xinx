include(${QT_USE_FILE})

set(libxinx_SRCS
	abstracteditor.cpp
	abstractfileeditor.cpp
	appsettings.cpp
	borderlayout.cpp
	exceptions.cpp
	filecontentitemmodel.cpp
	filecontentstructure.cpp
	filewatcher.cpp
	itemmodelfileeditor.cpp
	numberbar.cpp
	rcs.cpp
	syntaxhighlighter.cpp
	texteditor.cpp
	textfileeditor.cpp
	xinxconfig.cpp
	xinxcore.cpp
	xinxpluginsloader.cpp
	xinxthread.cpp
	xslproject.cpp
)

SET(libxinx_MOC_HDRS
	abstracteditor.h
	abstractfileeditor.h
	appsettings.h
	borderlayout.h
	exceptions.h
	filecontentitemmodel.h
	filecontentstructure.h
	filewatcher.h
	itemmodelfileeditor.h
	numberbar.h
	p_filewatcher.h
	plugininterfaces.h
	rcs.h
	syntaxhighlighter.h
	texteditor.h
	textfileeditor.h
	xinxconfig.h
	xinxcore.h
	xinxpluginsloader.h
	xinxthread.h
	xslproject.h
)

include_directories( . ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} )

QT4_WRAP_CPP(libxinx_MOC_SRCS ${libxinx_MOC_HDRS})

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

add_library(sharedxinx SHARED ${libxinx_SRCS} ${libxinx_MOC_SRCS})
target_link_libraries(sharedxinx ${QT_LIBRARIES})

install(TARGETS sharedxinx DESTINATION bin)

