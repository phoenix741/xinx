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
SOURCES = projectwizard.cpp \
    main.cpp
HEADERS = projectwizard.h
RESOURCES = xinxprojectwizard.qrc
INCLUDEPATH += ../components
LIBS = -L../components
win32:CONFIG(debug) { 
    LIBS += -lxinxcmpd
    POST_TARGETDEPS = ../components/libxinxcmpd.a
}
else { 
    LIBS += -lxinxcmp
    POST_TARGETDEPS = ../components/libxinxcmp.a
}