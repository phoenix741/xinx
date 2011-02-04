find_package(Qt4 REQUIRED)
set(USE_FRAMEWORK True)
set(USE_COMPONENTS True)

include(UseXinx)

add_xinx_plugins(svnpluginwrapper True "" 0.1.0.1)


