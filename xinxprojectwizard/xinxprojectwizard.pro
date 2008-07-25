TEMPLATE = app
DESTDIR += ./
QT += xml
CONFIG += exceptions \
    qt \
    qdbus \
    thread \
    warn_on 
SOURCES = projectconverter.cpp \
    projectwizard.cpp \
    main.cpp \
	../xinx/studioadaptor.cpp \
    ../xinx/studiointerface.cpp
HEADERS = projectconverter.h \
    projectwizard.h \
	../xinx/studioadaptor.h \
    ../xinx/studiointerface.h
RESOURCES = xinxprojectwizard.qrc
INCLUDEPATH += ../components \
    ../libxinx
LIBS = -L../libxinx -L../components -lsharedxinx -lxinxcmp
TRANSLATIONS += translations/xinxprojectwizard_fr.ts
PRE_TARGETDEPS = ../components/libxinxcmp.a
include(../project_mode.pro)