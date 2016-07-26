set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt4 REQUIRED)
find_package(QCodeEdit REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)

set(QT_USE_QTXML TRUE)
set(QT_USE_QTXMLPATTERNS TRUE)
set(USE_FRAMEWORK True)
set(USE_COMPONENTS True)

include(UseXinx)

include_directories(${CMAKE_SOURCE_DIR}/plugins/core)

add_xinx_plugins(generix True coreplugin 1.0.0.1)
