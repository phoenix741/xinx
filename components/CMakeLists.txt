set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt4 REQUIRED)
find_package(QCodeEdit REQUIRED)

include(UseXinx)

add_definitions(-D_COMPONENTS_BUILD_)

add_xinx_librairie(xinxcmp "" 0.9.0.0)
