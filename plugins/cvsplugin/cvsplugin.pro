TEMPLATE = lib
CONFIG += plugin
DESTDIR += ../
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./

LIBS = -L../../libxinx -lsharedxinx 
if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
   mac:LIBS = $$member(LIBS, 0) $$member(LIBS, 1)_debug
   win32:LIBS = $$member(LIBS, 0) $$member(LIBS, 1)d
}
win32:if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
    POST_TARGETDEPS = ../../libxinx/libsharedxinxd.a
}
else {
    POST_TARGETDEPS = ../../libxinx/libsharedxinx.a
}

TARGET = $$qtLibraryTarget(cvsplugin)
unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
INCLUDEPATH += ../../libxinx
HEADERS = p_rcs_cvs.h \
    cvsfiles.h \
    cvsthread.h \
    rcs_cvs.h \
    pluginsettings.h \
    customizeplugin.h \
    cvsplugin.h
SOURCES = cvsfiles.cpp \
    cvsthread.cpp \
    rcs_cvs.cpp \
    pluginsettings.cpp \
    customizeplugin.cpp \
    cvsplugin.cpp
FORMS += ui/customize.ui
RESOURCES = cvsplugin.qrc
