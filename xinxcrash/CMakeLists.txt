set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt4 REQUIRED)

set(USE_COMPONENTS True)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTWEBKIT TRUE)

include(UseXinx)

add_xinx_executable(xinxcrash "")
