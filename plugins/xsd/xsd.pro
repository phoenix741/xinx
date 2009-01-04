TEMPLATE = lib
CONFIG += plugin
DESTDIR += ../
LIBS = -L../../libxinx \
    -L../../components \
    -lsharedxinx \
    -lxinxcmp
TARGET = $$qtLibraryTarget(xsd)
INCLUDEPATH += ../../libxinx \
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
include(../../project_mode.pro)
QT += xml
