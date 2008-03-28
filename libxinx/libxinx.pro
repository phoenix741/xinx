TEMPLATE = lib
CONFIG += debug \
    static
DESTDIR += ./
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
TARGET = $$qtLibraryTarget(sharedxinx)
unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
HEADERS = xinxthread.h \
    rcs.h \
    exceptions.h \
    filecontentstructure.h
SOURCES = xinxthread.cpp \
    rcs.cpp \
    exceptions.cpp \
    filecontentstructure.cpp
TRANSLATIONS += translations/libxinx_fr.ts
