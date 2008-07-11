TEMPLATE = lib
CONFIG += plugin \
 debug
QT += network \
	xml
DESTDIR += ../
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
LIBS = -L../../libxinx -L../../components -L../ -lsharedxinx -lxinxcmp -lwebplugin
if(!debug_and_release|build_pass):CONFIG(debug, debug|release){
    mac : LIBS = $$member(LIBS, 0) $$member(LIBS, 1) $$member(LIBS, 2) $$member(LIBS, 3)_debug $$member(LIBS, 4)_debug $$member(LIBS, 5)_debug
    win32 : LIBS = $$member(LIBS, 0) $$member(LIBS, 1) $$member(LIBS, 2) $$member(LIBS, 3)d $$member(LIBS, 4)d $$member(LIBS, 5)d
}
TARGET = $$qtLibraryTarget(services)
unix {
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
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

