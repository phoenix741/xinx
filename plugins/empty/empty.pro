TEMPLATE = lib
CONFIG += plugin \
 debug
DESTDIR += ../
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
LIBS = -L../../libxinx -lsharedxinx
if(!debug_and_release|build_pass):CONFIG(debug, debug|release){
    mac : LIBS = $$member(LIBS, 0) $$member(LIBS, 1)_debug 
    win32 : LIBS = $$member(LIBS, 0) $$member(LIBS, 1)d
}
TARGET = $$qtLibraryTarget(empty)
unix {
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
INCLUDEPATH += ../../libxinx 
RESOURCES = empty.qrc
TRANSLATIONS += translations/empty_fr.ts
HEADERS = empty.h
SOURCES = empty.cpp
