set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt4 REQUIRED)

include(UseXinx)

add_definitions(-DIDEALITY_BUILD)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

set(headers 
	dmainwindow.h
	dbuttonbar.h
	dviewbutton.h
	dtoolview.h
	dtabbedmainwindow.h
	dmainwindowabstractsettings.h
	dmainwindowfactory.h
	dideality.h
	dstackedmainwindow.h
	dworkspacemainwindow.h
	dtabbedmainwindow_p.h)
set(sources 
	dmainwindow.cpp
	dbuttonbar.cpp
	dviewbutton.cpp
	dtoolview.cpp
	dtabbedmainwindow.cpp
	dmainwindowfactory.cpp
	dstackedmainwindow.cpp
	dworkspacemainwindow.cpp)

add_library(ideality SHARED ${sources})
target_link_libraries(ideality ${QT_LIBRARIES})
set_target_properties(ideality PROPERTIES VERSION 0.1.0.0)
if(UNIX)
	set_target_properties(ideality PROPERTIES LINK_FLAGS "-rdynamic")
endif(UNIX)

install(TARGETS ideality DESTINATION bin)