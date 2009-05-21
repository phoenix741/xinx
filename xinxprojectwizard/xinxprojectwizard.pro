include(../project_mode.pri)
TEMPLATE = app
DESTDIR += ./
QT += xml
CONFIG += exceptions \
    qt \
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
LIBS = -L../libxinx -L../components -L../ext/qcodeedit -lsharedxinx -lxinxcmp -lqcodeedit
TRANSLATIONS += translations/xinxprojectwizard_fr.ts
win32 : RC_FILE += rc/xinxprojectwizard.rc
