TEMPLATE = lib
CONFIG += plugin
QT += network xml
DESTDIR += ../
INCLUDEPATH += ../../libxinx ../../components ../webplugin/xsl
LIBS = -L../../libxinx -L../../components -L../ -lxinxcmp -lwebplugin -lsharedxinx
PRE_TARGETDEPS = ../libwebplugin.a ../../components/libxinxcmp.a
TARGET = $$qtLibraryTarget(services)
RESOURCES = servicesplugin.qrc
TRANSLATIONS += translations/servicesplugin_fr.ts
HEADERS = servicesplugin.h \
	servicesprojectpropertyimpl.h \
	servicesprojectwizard.h \
	connectionwebservicesdialogimpl.h \
	serviceresultdialogimpl.h \
	soap.h \
	webservices.h \
	wsdl.h \
	webserviceseditor.h
SOURCES =servicesplugin.cpp \
	servicesprojectpropertyimpl.cpp \
	servicesprojectwizard.cpp \
	connectionwebservicesdialogimpl.cpp \
	serviceresultdialogimpl.cpp \
	soap.cpp \
	webservices.cpp \
	wsdl.cpp \
	webserviceseditor.cpp
FORMS += ui/newprojectwizard_serviceslist.ui \
	ui/newprojectwizard_services.ui \
	ui/servicesprojectproperty.ui \
	ui/resultatServices.ui \
	ui/servicesconnection.ui
win32 : RC_FILE += rc/plugin.rc

include(../../project_mode.pro)