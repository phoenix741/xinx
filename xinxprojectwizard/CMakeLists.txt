set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTDBUS TRUE)

include(${QT_USE_FILE})

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(-DQT_NO_DEBUG)
add_definitions(${QT_DEFINITIONS})
include_directories( ./ ../libxinx ../components ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})

set(xinxprojectwizard_SRCS
	projectconverter.cpp
    projectwizard.cpp
    main.cpp
	../xinx/studioadaptor.cpp
	../xinx/studiointerface.cpp
)

SET(xinxprojectwizard_MOC_HDRS
	projectconverter.h 
	projectwizard.h
	../xinx/studioadaptor.h
	../xinx/studiointerface.h
)

QT4_WRAP_CPP(xinxprojectwizard_MOC_SRCS ${xinxprojectwizard_MOC_HDRS})
QT4_ADD_RESOURCES(xinxprojectwizard_QRC_H xinxprojectwizard.qrc)


add_executable(xinxprojectwizard WIN32 ${xinxprojectwizard_SRCS} ${xinxprojectwizard_MOC_SRCS} ${xinxprojectwizard_QRC_H})
link_directories(../libxinxprojectwizard ../components)
target_link_libraries(xinxprojectwizard ${QT_LIBRARIES} sharedxinx xinxcmp)

