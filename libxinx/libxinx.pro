TEMPLATE = lib
CONFIG += static
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
HEADERS = appsettings.h \
    xinxconfig.h \
    xinxpluginsloader.h \
    xinxcore.h \
    xinxthread.h \
    rcs.h \
    exceptions.h \
    filecontentstructure.h
SOURCES = appsettings.cpp \
    xinxconfig.cpp \
    xinxpluginsloader.cpp \
    xinxcore.cpp \
    xinxthread.cpp \
    rcs.cpp \
    exceptions.cpp \
    filecontentstructure.cpp
TRANSLATIONS += translations/libxinx_fr.ts
