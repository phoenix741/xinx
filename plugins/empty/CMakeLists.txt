set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt4 REQUIRED)
set(USE_FRAMEWORK True)

include(UseXinx)

add_xinx_plugins(empty True "" 1.0.0.0)


