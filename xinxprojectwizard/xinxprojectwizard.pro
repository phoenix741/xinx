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
