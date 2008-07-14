TEMPLATE = app
TARGET = xinxprojectwizard
DESTDIR += ./
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
QT += xml
CONFIG += exceptions \
    qt \
    qdbus \
    thread \
    warn_on 
SOURCES = projectconverter.cpp \
    projectwizard.cpp \
    main.cpp
HEADERS = projectconverter.h \
    projectwizard.h
RESOURCES = xinxprojectwizard.qrc
INCLUDEPATH += ../components \
    ../libxinx
LIBS = -L../libxinx -L../components -lsharedxinx -lxinxcmp
TRANSLATIONS += translations/xinxprojectwizard_fr.ts
contains( CONFIG, qdbus ){
    HEADERS += ../xinx/studioadaptor.h \
        ../xinx/studiointerface.h
    SOURCES += ../xinx/studioadaptor.cpp \
        ../xinx/studiointerface.cpp
}
include(../project_mode.pro)