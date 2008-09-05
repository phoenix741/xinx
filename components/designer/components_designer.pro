TEMPLATE = lib
CONFIG += designer plugin 

DESTDIR += ./
TARGET = $$qtLibraryTarget(xinxcmp_designer)

INCLUDEPATH += ../
LIBS = -L../ -lxinxcmp

HEADERS = directoryeditplugin.h
#	directoryeditwidgetplugin.h
SOURCES = directoryeditplugin.cpp
#	directoryeditwidgetplugin.cpp \

# install
target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
 
include(../../project_mode.pro)
CONFIG += release
CONFIG -= debug