include(../../project_mode.pri)

TEMPLATE = lib
CONFIG += plugin
DESTDIR += ../
LIBS = -L../../libxinx \
    -lsharedxinx
TARGET = dictionary
INCLUDEPATH += ../../libxinx
RESOURCES = dictionary.qrc
TRANSLATIONS += translations/dictionary_fr.ts
HEADERS = dictionary.h \
		dictionarydockwidgetimpl.h \
    dictionaryparser.h
SOURCES = dictionary.cpp \
		dictionarydockwidgetimpl.cpp \
    dictionaryparser.cpp
win32:RC_FILE += rc/plugin.rc
FORMS += ui/dictionarydockwidget.ui
