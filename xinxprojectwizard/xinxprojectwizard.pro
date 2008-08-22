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
    ../xinx/dbus/orgshadowarexinxinterface.cpp
HEADERS = projectconverter.h \
    projectwizard.h \
    ../xinx/dbus/orgshadowarexinxinterface.h
RESOURCES = xinxprojectwizard.qrc
INCLUDEPATH += ../components \
    ../libxinx
LIBS = -L../libxinx -L../components -lsharedxinx -lxinxcmp
TRANSLATIONS += translations/xinxprojectwizard_fr.ts
PRE_TARGETDEPS = ../components/libxinxcmp.a
win32 : RC_FILE += rc/xinxprojectwizard.rc
include(../project_mode.pro)