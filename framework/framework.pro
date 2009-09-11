include(../project_mode.pri)
TEMPLATE = lib
unix:VERSION = 0.9.0.0
CONFIG += dll
QT += xml \
    xmlpatterns \
    sql \
    script
TARGET = xinxframework
DESTDIR += ./
INCLUDEPATH += ./ \
    ../components \
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
HEADERS = rcs/rcs.h \
    plugins/xinxpluginsloader.h \
    plugins/xinxpluginelement.h \
    core/exceptions.h \
    core/filewatcher.h \
    core/p_filewatcher.h \
    core/appsettings.h \
    core/xinxconfig.h \
    core/xinxcore.h \
    core/xinxthread.h \
    core/configurationfile.h \
    project/xinxproject.h \
    project/dirrcsmodel.h \
    project/flattreeview.h \
    project/iconprojectprovider.h \
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
    editors/editormanager.h \
    snipets/snipet.h \
    snipets/snipetlist.h \
    snipets/snipetmanager.h \
    snipets/snipetitemmodel.h \
    snipets/callsnipetdlg.h \
    snipets/snipetmenu.h \
    snipets/categoryitemmodel.h \
    snipets/categorytreeview.h \
    scripts/scriptmanager.h \
    scripts/documentsearch.h \
    snipets/snipetpropertydlgimpl.h
SOURCES = rcs/rcs.cpp \
    plugins/xinxpluginsloader.cpp \
    plugins/xinxpluginelement.cpp \
    core/filewatcher.cpp \
    core/appsettings.cpp \
    core/xinxconfig.cpp \
    core/xinxcore.cpp \
    core/xinxthread.cpp \
    core/exceptions.cpp \
    core/configurationfile.cpp \
    project/xinxproject.cpp \
    project/dirrcsmodel.cpp \
    project/flattreeview.cpp \
    project/iconprojectprovider.cpp \
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
    editors/editormanager.cpp \
    snipets/snipet.cpp \
    snipets/snipetlist.cpp \
    snipets/snipetmanager.cpp \
    snipets/snipetitemmodel.cpp \
    snipets/callsnipetdlg.cpp \
    snipets/snipetmenu.cpp \
    snipets/categoryitemmodel.cpp \
    snipets/categorytreeview.cpp \
    scripts/scriptmanager.cpp \
    scripts/documentsearch.cpp \
    snipets/snipetpropertydlgimpl.cpp
FORMS = snipets/callsnipetdlg.ui \
    snipets/snipetpropertydlg.ui
TRANSLATIONS += translations/libxinx_fr.ts
OTHER_FILES += 
