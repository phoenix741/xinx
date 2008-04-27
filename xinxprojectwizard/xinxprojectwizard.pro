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
CONFIG += exceptions \
    qt \
    qdbus \
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
LIBS = -L../libxinx -L../components -lsharedxinx -lxinxcmp
if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
   mac:LIBS = $$member(LIBS, 0) $$member(LIBS, 1) $$member(LIBS, 2)_debug $$member(LIBS, 3)_debug 
   win32:LIBS = $$member(LIBS, 0) $$member(LIBS, 1) $$member(LIBS, 2)d $$member(LIBS, 3)d
}
TRANSLATIONS += translations/xinxprojectwizard_fr.ts
contains( CONFIG, qdbus ) { 
    HEADERS += ../xinx/studioadaptor.h \
        ../xinx/studiointerface.h
    SOURCES += ../xinx/studioadaptor.cpp \
        ../xinx/studiointerface.cpp
}
