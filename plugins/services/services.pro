TEMPLATE = lib
CONFIG += plugin
QT += network \
	xml
DESTDIR += ../
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
LIBS = -L../../libxinx -L../../components -L../ -lxinxcmp -lwebplugin -lsharedxinx
TARGET = $$qtLibraryTarget(services)
INCLUDEPATH += ../../libxinx ../../components ../webplugin/xsl
RESOURCES = servicesplugin.qrc
TRANSLATIONS += translations/servicesplugin_fr.ts
HEADERS =servicesplugin.h \
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

include(../../project_mode.pro)