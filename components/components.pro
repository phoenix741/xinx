TEMPLATE = lib
CONFIG += debug \
    static
DESTDIR += ./
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
TARGET = $$qtLibraryTarget(xinxcmp)
INCLUDEPATH += ../libxinx
LIBS=-L../libxinx -lsharedxinx
unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
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