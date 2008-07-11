TEMPLATE = lib
CONFIG += plugin \
 debug
QT += xml
DESTDIR += ../
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
LIBS = -L../../libxinx -L../../components -lsharedxinx -lxinxcmp
if(!debug_and_release|build_pass):CONFIG(debug, debug|release){
    mac : LIBS = $$member(LIBS, 0) $$member(LIBS, 1) $$member(LIBS, 2)_debug $$member(LIBS, 3)_debug
    win32 : LIBS = $$member(LIBS, 0) $$member(LIBS, 1) $$member(LIBS, 2)d $$member(LIBS, 3)d
}
TARGET = $$qtLibraryTarget(services)
unix {
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
INCLUDEPATH += ../../libxinx ../../components
RESOURCES = servicesplugin.qrc
TRANSLATIONS += translations/servicesplugin_fr.ts
HEADERS =servicesplugin.h \
servicesprojectpropertyimpl.h \
servicesprojectwizard.h
SOURCES =servicesplugin.cpp \
servicesprojectpropertyimpl.cpp \
servicesprojectwizard.cpp
FORMS += ui/newprojectwizard_serviceslist.ui \
ui/newprojectwizard_services.ui \
ui/servicesprojectproperty.ui

