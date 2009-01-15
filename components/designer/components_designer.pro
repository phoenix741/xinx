TEMPLATE = lib
CONFIG += designer plugin

DESTDIR += ./
TARGET = $$qtLibraryTarget(xinxcmp_designer)
INCLUDEPATH += ../

LIBS = -L../ -lxinxcmp
HEADERS = xinxdesignerplugin.h \
    directoryeditplugin.h \
    directoryeditwidgetplugin.h \
    xinxcolorcomboplugin.h \
    pluginselectorplugin.h \
    xinxlistwidgetplugin.h \
    customschemeplugin.h
SOURCES = xinxdesignerplugin.cpp \
	directoryeditplugin.cpp \
    directoryeditwidgetplugin.cpp \
    xinxcolorcomboplugin.cpp \
    pluginselectorplugin.cpp \
    xinxlistwidgetplugin.cpp \
    customschemeplugin.cpp

# install
target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
include(../../project_mode.pro)
CONFIG += release
CONFIG -= debug
