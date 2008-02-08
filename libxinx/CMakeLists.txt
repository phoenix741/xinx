include(${QT_USE_FILE})

set(xinxplugins_SRCS
    exceptions.cpp
    filecontentstructure.cpp
)

SET(xinxplugins_MOC_HDRS
    filecontentstructure.h
)

include_directories( . ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} )

QT4_WRAP_CPP(xinxplugins_MOC_SRCS ${xinxplugins_MOC_HDRS})

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

add_library(sharedxinx STATIC ${xinxplugins_SRCS} ${xinxplugins_MOC_SRCS})
target_link_libraries(sharedxinx ${QT_LIBRARIES})

install(TARGETS sharedxinx DESTINATION bin/plugins)

