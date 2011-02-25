find_package(Qt4 REQUIRED)

set(USE_COMPONENTS True)
set(QT_USE_QTWEBKIT TRUE)

include(UseXinx)

add_xinx_executable(xinxcrash "")
