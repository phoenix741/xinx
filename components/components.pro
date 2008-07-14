TEMPLATE = lib
CONFIG += static staticlib
DESTDIR += ./
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
TARGET = $$qtLibraryTarget(xinxcmp)
INCLUDEPATH += ../libxinx
LIBS =-L../libxinx -lsharedxinx
FORMS = ui/xinxlistwidget.ui
HEADERS = p_xinxpluginselector.h \
    xinxpluginselector.h \
    directoryedit.h \
    kcolorcombo.h \
    xinxlistwidgetimpl.h     
SOURCES = directoryedit.cpp \
    kcolorcombo.cpp \
    xinxpluginselector.cpp \ 
    xinxlistwidgetimpl.cpp 
TRANSLATIONS += translations/xinxcomponents_fr.ts
RESOURCES = xinxpluginselector.qrc
include(../project_mode.pro)