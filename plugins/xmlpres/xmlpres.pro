include(../../project_mode.pri)

TEMPLATE = lib
CONFIG += plugin staticlib
QT += xml \
		xmlpatterns
DESTDIR += ../
LIBS = -L../../libxinx -lsharedxinx
TARGET = xmlpres
INCLUDEPATH += ../../libxinx  \
		../../components \
		../../ext/qcodeedit/lib \
		../../ext/qcodeedit/lib/document \
		../../ext/qcodeedit/lib/language \
		../../ext/qcodeedit/lib/qnfa \
		../../ext/qcodeedit/lib/widgets
RESOURCES = xmlpres.qrc
TRANSLATIONS += translations/xmlpres_fr.ts
HEADERS = xmlpres.h xmlpresentationdockthread.h xquerydialogimpl.h xmlpresentationdockwidget.h xmlpresentationitem.h 
SOURCES = xmlpres.cpp xmlpresentationdockthread.cpp xquerydialogimpl.cpp xmlpresentationdockwidget.cpp xmlpresentationitem.cpp
FORMS += ui/xquery.ui \
    ui/xmlpresentationwidget.ui \
	ui/config.ui

