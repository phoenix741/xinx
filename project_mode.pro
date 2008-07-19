CONFIG += debug

if(!debug_and_release|build_pass):CONFIG(debug, debug|release){
	MYLIBS = $$find(LIBS, -l.*)
	for( LIBRARY, MYLIBS ) {
		LIBS -= $$LIBRARY
		mac : LIBS += $$join(LIBRARY,,,_debug)
		win32 : LIBS += $$join(LIBRARY,,,d)
		unix : LIBS += $$LIBRARY
	}
}

unix {
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
