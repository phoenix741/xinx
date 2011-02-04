find_package(Qt4 REQUIRED)
find_package(QCodeEdit REQUIRED)

set(USE_FRAMEWORK True)
set(USE_COMPONENTS True)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTXMLPATTERNS TRUE)
set(QT_USE_QTWEBKIT TRUE)

#add_definitions(-fPIC)

include(UseXinx)

add_xinx_plugins(coreplugin False "" 0.9.0.0)


