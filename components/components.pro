TEMPLATE = lib
CONFIG += dll
win32 : QMAKE_LFLAGS_SHLIB *= -no-undefined -enable-runtime-pseudo-reloc
DESTDIR += ./
TARGET = $$qtLibraryTarget(xinxcmp)
FORMS = ui/xinxlistwidget.ui
HEADERS = p_pluginselector.h \
    pluginselector.h \
    directoryedit.h \
    kcolorcombo.h \
    xinxlistwidgetimpl.h \
    pluginelement.h \
    borderlayout.h
SOURCES = directoryedit.cpp \
    kcolorcombo.cpp \
    pluginselector.cpp \
    xinxlistwidgetimpl.cpp \
    pluginelement.cpp \
    borderlayout.cpp
TRANSLATIONS += translations/xinxcomponents_fr.ts
RESOURCES = xinxpluginselector.qrc

include(../project_mode.pro)
