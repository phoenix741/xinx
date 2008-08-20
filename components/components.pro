TEMPLATE = lib
CONFIG += staticlib
DESTDIR += ./
TARGET = $$qtLibraryTarget(xinxcmp)
INCLUDEPATH += ../libxinx
LIBS =-L../libxinx -lsharedxinx
FORMS = ui/xinxlistwidget.ui
HEADERS = p_pluginselector.h \
    pluginselector.h \
    directoryedit.h \
    kcolorcombo.h \
    xinxlistwidgetimpl.h \
	pluginelement.h
SOURCES = directoryedit.cpp \
    kcolorcombo.cpp \
    pluginselector.cpp \
    xinxlistwidgetimpl.cpp \
	pluginelement.cpp
TRANSLATIONS += translations/xinxcomponents_fr.ts
RESOURCES = xinxpluginselector.qrc

include(../project_mode.pro)