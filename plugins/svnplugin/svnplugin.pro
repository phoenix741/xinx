TEMPLATE = lib
CONFIG += plugin \
 debug
QT += xml
DESTDIR += ../
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
LIBS = -L../../libxinx -lsharedxinx
if(!debug_and_release|build_pass):CONFIG(debug, debug|release){
    mac : LIBS = $$member(LIBS, 0) $$member(LIBS, 1)_debug
    win32 : LIBS = $$member(LIBS, 0) $$member(LIBS, 1)d
}
TARGET = $$qtLibraryTarget(svnplugin)
unix {
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
INCLUDEPATH += ../../libxinx
HEADERS = rcs_svn.h \
    svnplugin.h
SOURCES = rcs_svn.cpp \
    svnplugin.cpp
RESOURCES = svnplugin.qrc
TRANSLATIONS += translations/svnplugin_fr.ts
