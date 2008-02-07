project(xinx)
cmake_minimum_required(VERSION 2.4.0)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Qt4 REQUIRED) # find and setup Qt4 for this project

add_subdirectory(libxinx)
add_subdirectory(plugins)
add_subdirectory(xinx)
add_subdirectory(doc)
