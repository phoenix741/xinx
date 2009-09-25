include(../project_mode.pri)

TEMPLATE = lib
unix:VERSION = 0.9.0.0
CONFIG += dll
win32 : QMAKE_LFLAGS_SHLIB *= -no-undefined -enable-runtime-pseudo-reloc
DESTDIR += ./
INCLUDEPATH += ../ext/qcodeedit/lib \
    ../ext/qcodeedit/lib/document \
    ../ext/qcodeedit/lib/language \
    ../ext/qcodeedit/lib/qnfa \
    ../ext/qcodeedit/lib/widgets
LIBS = -L../ext/qcodeedit \
    -lqcodeedit
TARGET = xinxcmp
FORMS = ui/xinxlistwidget.ui \
	ui/customscheme.ui
HEADERS = p_pluginselector.h \
    pluginselector.h \
    directoryedit.h \
    xinxcolorcombo.h \
    xinxlistwidgetimpl.h \
    pluginelement.h \
    borderlayout.h \
    customschemeimpl.h \
	qmenuview.h \
	stringcombobox.h \
	xinxlineedit.h
SOURCES = directoryedit.cpp \
    xinxcolorcombo.cpp \
    pluginselector.cpp \
    xinxlistwidgetimpl.cpp \
    pluginelement.cpp \
    borderlayout.cpp \
    customschemeimpl.cpp \
	qmenuview.cpp \
	stringcombobox.cpp \
	xinxlineedit.cpp
TRANSLATIONS += translations/xinxcomponents_fr.ts
RESOURCES = xinxpluginselector.qrc


