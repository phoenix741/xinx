include(../../project_mode.pri)

TEMPLATE = lib
VERSION = 0.1.0.1
CONFIG += plugin
QT += network xml
DESTDIR += ../
INCLUDEPATH += ../../libxinx \
	../../components \
	../webplugin/xsl \
	../../ext/qcodeedit/lib \
	../../ext/qcodeedit/lib/document \
	../../ext/qcodeedit/lib/language \
	../../ext/qcodeedit/lib/qnfa \
	../../ext/qcodeedit/lib/widgets
LIBS = -L../../libxinx -L../../components -L../ -L../../ext/qcodeedit -lxinxcmp -lwebplugin -lsharedxinx -lqcodeedit
PRE_TARGETDEPS = ../libwebplugin.a
TARGET = services
RESOURCES = servicesplugin.qrc
TRANSLATIONS += translations/servicesplugin_fr.ts
HEADERS = servicesplugin.h \
	servicesprojectpropertyimpl.h \
	servicesprojectwizard.h \
	serviceresultdialogimpl.h \
	webservices.h \
	wsdl.h \
	webserviceseditor.h \
 webservicesfiletype.h
SOURCES =servicesplugin.cpp \
	servicesprojectpropertyimpl.cpp \
	servicesprojectwizard.cpp \
	serviceresultdialogimpl.cpp \
	webservices.cpp \
	wsdl.cpp \
	webserviceseditor.cpp
FORMS += ui/newprojectwizard_serviceslist.ui \
	ui/newprojectwizard_services.ui \
	ui/servicesprojectproperty.ui
win32 : RC_FILE += rc/plugin.rc

include(../../ext/qtsoap/src/qtsoap.pri)SOURCES -= connectionwebservicesdialogimpl.cpp \
serviceresultdialogimpl.cpp
HEADERS -= connectionwebservicesdialogimpl.h \
serviceresultdialogimpl.h
