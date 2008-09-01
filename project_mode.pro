CONFIG += release
if(!debug_and_release|build_pass):CONFIG(debug, debug|release) { 
    win32:PRE_TARGETDEPS ~= s/\.a/d.a/g
    mac:PRE_TARGETDEPS ~= s/\.a/_debug.a/g
    MYLIBS = $$find(LIBS, -l.*)
    for( LIBRARY, MYLIBS ) { 
        LIBS -= $$LIBRARY
        mac:LIBS += $$join(LIBRARY,,,_debug)
        win32:LIBS += $$join(LIBRARY,,,d)
        unix:LIBS += $$LIBRARY
    }
}
unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
QMAKE_CXXFLAGS_RELEASE = "-Os"
DEFINES += NO_QT_PLAINTEXT
