include(../../project_mode.pri)
TEMPLATE = lib
VERSION = 1.0.0.0
CONFIG += plugin
QT += network \
    xml \
	sql
DESTDIR += ../
INCLUDEPATH += ../../framework \
    ../../components \
    ../core \
    ../../ext/qcodeedit/lib \
    ../../ext/qcodeedit/lib/document \
    ../../ext/qcodeedit/lib/language \
    ../../ext/qcodeedit/lib/qnfa \
    ../../ext/qcodeedit/lib/widgets
LIBS = -L../../framework \
    -L../../components \
    -L../ \
    -L../../ext/qcodeedit \
    -lxinxcmp \
    -lcoreplugin \
    -lxinxframework \
    -lqcodeedit
PRE_TARGETDEPS = ../libcoreplugin.a
TARGET = services
RESOURCES = servicesplugin.qrc
TRANSLATIONS += translations/servicesplugin_fr.ts
HEADERS = servicesplugin.h \
    servicesprojectpropertyimpl.h \
    servicesprojectwizard.h \
    webservices.h \
    wsdl.h \
    webserviceseditor.h \
    webservicesfiletype.h \
    webservicesaction.h \
    servicesbatchdialogimpl.h
SOURCES = servicesplugin.cpp \
    servicesprojectpropertyimpl.cpp \
    servicesprojectwizard.cpp \
    webservices.cpp \
    wsdl.cpp \
    webserviceseditor.cpp \
    webservicesaction.cpp \
    servicesbatchdialogimpl.cpp
FORMS += ui/newprojectwizard_serviceslist.ui \
    ui/servicesprojectproperty.ui \
    ui/servicesbatch.ui
win32:RC_FILE += rc/plugin.rc
include(../../ext/qtsoap/src/qtsoap.pri)
