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
    dictionarydockwidget.h \
    dictionaryparser.h
SOURCES = dictionary.cpp \
    dictionarydockwidget.cpp \
    dictionaryparser.cpp
win32:RC_FILE += rc/plugin.rc
include(../../project_mode.pri)
