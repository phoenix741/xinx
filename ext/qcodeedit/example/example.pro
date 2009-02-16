TEMPLATE = app
TARGET = example
CONFIG += debug console
CONFIG -= app_bundle

UI_DIR = 
MOC_DIR = .build
OBJECTS_DIR = .build
DESTDIR = .

LIBS += -L.. -lqcodeedit

DEFINES += _QCODE_EDIT_ _QCODE_EDIT_GENERIC_

INCLUDEPATH += ../lib ../lib/language ../lib/document ../lib/widgets ../lib/.build

HEADERS += window.h snippets.h
SOURCES += main.cpp window.cpp snippets.cpp

FORMS += window.ui

RESOURCES += example.qrc
