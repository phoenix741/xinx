TEMPLATE = lib
CONFIG += dll
QT += xml
TARGET = sharedxinx
DESTDIR += ./
INCLUDEPATH += ../components \
    ../ext/qcodeedit/lib \
    ../ext/qcodeedit/lib/document \
    ../ext/qcodeedit/lib/language \
    ../ext/qcodeedit/lib/qnfa \
    ../ext/qcodeedit/lib/widgets
LIBS = -L../components \
    -L../ext/qcodeedit \
    -lxinxcmp \
    -lqcodeedit
win32:QMAKE_LFLAGS_SHLIB *= -no-undefined \
    -enable-runtime-pseudo-reloc
win32:RC_FILE += rc/libxinx.rc
HEADERS = xinxproject.h \
    xinxformatscheme.h \
    textfileeditor.h \
    filewatcher.h \
    p_filewatcher.h \
    filecontentitemmodel.h \
    appsettings.h \
    xinxconfig.h \
    xinxpluginsloader.h \
    xinxcore.h \
    xinxthread.h \
    rcs.h \
    exceptions.h \
    filecontentstructure.h \
    abstracteditor.h \
    abstractfileeditor.h \
    itemmodelfileeditor.h \
    xinxpluginelement.h \
    plugininterfaces.h \
    xinxcodeedit.h \
    xinxcodeedit_p.h \
    xinxlanguagefactory.h \
    contentviewcache.h \
    contentviewnode.h \
    contentviewmodel.h \
    contentviewparser.h
SOURCES = contentviewnode.cpp \
    xinxproject.cpp \
    xinxformatscheme.cpp \
    textfileeditor.cpp \
    filewatcher.cpp \
    filecontentitemmodel.cpp \
    appsettings.cpp \
    xinxconfig.cpp \
    xinxpluginsloader.cpp \
    xinxcore.cpp \
    xinxthread.cpp \
    rcs.cpp \
    exceptions.cpp \
    filecontentstructure.cpp \
    abstracteditor.cpp \
    abstractfileeditor.cpp \
    itemmodelfileeditor.cpp \
    xinxpluginelement.cpp \
    xinxcodeedit.cpp \
    xinxlanguagefactory.cpp \
    contentviewparser.cpp
TRANSLATIONS += translations/libxinx_fr.ts
include(../project_mode.pro)
SOURCES -= xinxformatfactory.cpp
HEADERS -= xinxformatfactory.h
