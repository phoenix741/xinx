include(../../project_mode.pri)

TEMPLATE = lib
CONFIG += plugin
DESTDIR += ../
LIBS = -L../../framework \
    -L../../components \
    -lxinxframework \
    -lxinxcmp
TARGET = xsd
INCLUDEPATH += ../../framework \
    ../../components
RESOURCES = xsdedit.qrc
TRANSLATIONS += translations/xsledit_fr.ts
HEADERS = xmlschema.h \
    p_xsdeditor.h \
    xsdedit.h \
    xsdeditor.h
SOURCES = xmlschema.cpp \
    xsdedit.cpp \
    xsdeditor.cpp
win32:RC_FILE += rc/plugin.rc
QT += xml
