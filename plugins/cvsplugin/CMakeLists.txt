set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt4 REQUIRED)
set(USE_FRAMEWORK True)
set(USE_COMPONENTS True)

include(UseXinx)

add_xinx_plugins(cvsplugin True "" 0.1.0.1)

