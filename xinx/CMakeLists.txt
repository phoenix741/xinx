find_package(Qt4 REQUIRED)
find_package(QCodeEdit REQUIRED)
find_package(Ideality REQUIRED)

set(USE_FRAMEWORK True)

include(UseUniqueApplication)
include(UseXinx)

add_xinx_executable(xinx "coreplugin")
