include(../project_mode.pri)

TEMPLATE = lib
VERSION = 0.9.0.0
CONFIG += dll
QT += xml \
    xmlpatterns
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
win32 : QMAKE_LFLAGS_SHLIB *= -no-undefined \
    -enable-runtime-pseudo-reloc
win32 : RC_FILE += rc/libxinx.rc
HEADERS = rcs/rcs.h \
    plugins/xinxpluginsloader.h \
    plugins/xinxpluginelement.h \
    core/exceptions.h \
    core/xinxproject.h \
    core/filewatcher.h \
    core/p_filewatcher.h \
    core/appsettings.h \
    core/xinxconfig.h \
    core/xinxcore.h \
    core/xinxthread.h \
    core/configurationfile.h \
    plugins/plugininterfaces.h \
    editors/xinxformatscheme.h \
    editors/xinxlanguagefactory.h \
    editors/textfileeditor.h \
    editors/abstracteditor.h \
    editors/xinxcodeedit.h \
    editors/xinxcodeedit_p.h \
    editors/contentviewtexteditor.h \
    contentview/contentviewcache.h \
    contentview/contentviewnode.h \
    contentview/contentviewmodel.h \
    contentview/contentviewparser.h \
    contentview/completionnodemodel.h \
    contentview/abstractcontentviewmodel.h \
    actions/actioninterface.h \
 editors/editormanager.h
SOURCES = rcs/rcs.cpp \
    plugins/xinxpluginsloader.cpp \
    plugins/xinxpluginelement.cpp \
    core/xinxproject.cpp \
    core/filewatcher.cpp \
    core/appsettings.cpp \
    core/xinxconfig.cpp \
    core/xinxcore.cpp \
    core/xinxthread.cpp \
    core/exceptions.cpp \
    core/configurationfile.cpp \
    editors/xinxformatscheme.cpp \
    editors/xinxlanguagefactory.cpp \
    editors/abstracteditor.cpp \
    editors/textfileeditor.cpp \
    editors/xinxcodeedit.cpp \
    contentview/contentviewmodel.cpp \
    contentview/contentviewnode.cpp \
    contentview/contentviewparser.cpp \
    contentview/contentviewcache.cpp \
    contentview/abstractcontentviewmodel.cpp \
    contentview/completionnodemodel.cpp \
    editors/contentviewtexteditor.cpp \
 actions/actioninterface.cpp \
 editors/editormanager.cpp
TRANSLATIONS += translations/libxinx_fr.ts

