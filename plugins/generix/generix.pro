include(../../project_mode.pri)

TEMPLATE = lib
VERSION = 1.0.0.1
CONFIG += plugin
DESTDIR += ../
LIBS = -L../../framework \
		-lxinxframework
TARGET = generix
INCLUDEPATH += ../../framework
RESOURCES = plugindefinition/generix.qrc
TRANSLATIONS += translations/dictionary_fr.ts
HEADERS = plugindefinition/generix.h \
		docks/dictionary/dictionarydockwidgetimpl.h \
		docks/dictionary/dictionaryparser.h
SOURCES = plugindefinition/generix.cpp \
		docks/dictionary/dictionarydockwidgetimpl.cpp \
		docks/dictionary/dictionaryparser.cpp
win32:RC_FILE += rc/plugin.rc
FORMS += ui/dictionarydockwidget.ui
