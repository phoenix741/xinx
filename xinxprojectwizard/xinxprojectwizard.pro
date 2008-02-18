TEMPLATE = app
TARGET = xinxprojectwizard
DESTDIR += ./
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
QT += xml
CONFIG += debug \
    exceptions \
    qt \
    thread \
    warn_on \
    x86
SOURCES = projectconverter.cpp \
    projectwizard.cpp \
    main.cpp
HEADERS = projectconverter.h \
    projectwizard.h
RESOURCES = xinxprojectwizard.qrc
INCLUDEPATH += ../components \
    ../libxinx
LIBS = -L../components \
    -L../libxinx
win32:CONFIG(debug) { 
    LIBS += -lxinxcmpd -lsharedxinxd
    POST_TARGETDEPS = ../components/libxinxcmpd.a ../libxinx/libsharedxinxd.a
}
else { 
    LIBS += -lxinxcmp -lsharedxinx
    POST_TARGETDEPS = ../components/libxinxcmp.a ../libxinx/libsharedxinx.a
}
