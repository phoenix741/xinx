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
	-L../ext/qcodeedit
win32:LIBS += -lwsock32 -llibxml2 -llibxslt
unix:LIBS += -lxml2 -lxslt
LIBS += -lxinxcmp \
	-lqcodeedit
win32:RC_FILE += rc/libxinx.rc
unix:INCLUDEPATH += /usr/include/libxml2
DEFINES += _LIB_BUILD_
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
    project/xinxproject.h \
    project/dirrcsmodel.h \
    project/flattreeview.h \
    project/iconprojectprovider.h \
    plugins/plugininterfaces.h \
    editors/bookmarkeditorinterface.h \
    editors/xinxformatscheme.h \
    editors/xinxlanguagefactory.h \
    editors/textfileeditor.h \
    editors/abstracteditor.h \
    editors/xinxcodeedit.h \
    editors/xinxcodeedit_p.h \
    contentview2/contentview2cache.h \
    contentview2/contentview2node.h \
    contentview2/contentview2parser.h \
    contentview2/contentview2treemodel.h \
    contentview2/contentview2completionmodel.h \
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
    snipets/snipetpropertydlgimpl.h \
    snipets/categorypropertydlgimpl.h \
    snipets/basesnipetitemmodel.h \
    snipets/snipetdockitemmodel.h \
    utils/treeproxyitemmodel.h \
    utils/recursivesortfilterproxymodel.h \
    utils/xsltparser.h \
    scripts/scriptmanager.h \
    scripts/documentsearch.h \
    editors/editorfactory.h \
    editors/bookmarktexteditorinterface.h \
    project/externalfileresolver.h \
    rcs/rcsmanager.h \
    rcs/commitmessagedialogimpl.h \
    project/newprojecttemplate.h \
    snipets/snipetcompletionparser.h \
    contentview2/contentview2manager.h \
    contentview2/contentview2parserfactory.h \
    contentview2/contentview2file.h \
    contentview2/contentview2project.h
SOURCES = rcs/rcs.cpp \
    plugins/xinxpluginsloader.cpp \
    plugins/xinxpluginelement.cpp \
    core/filewatcher.cpp \
    core/appsettings.cpp \
    core/xinxconfig.cpp \
    core/xinxcore.cpp \
    core/xinxthread.cpp \
    core/exceptions.cpp \
    project/xinxproject.cpp \
    project/dirrcsmodel.cpp \
    project/flattreeview.cpp \
    project/iconprojectprovider.cpp \
    editors/bookmarkeditorinterface.cpp \
    editors/xinxformatscheme.cpp \
    editors/xinxlanguagefactory.cpp \
    editors/abstracteditor.cpp \
    editors/textfileeditor.cpp \
    editors/xinxcodeedit.cpp \
    contentview2/contentview2cache.cpp \
    contentview2/contentview2node.cpp \
    contentview2/contentview2parser.cpp \
    contentview2/contentview2treemodel.cpp \
    contentview2/contentview2completionmodel.cpp \
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
    snipets/snipetpropertydlgimpl.cpp \
    snipets/categorypropertydlgimpl.cpp \
    snipets/basesnipetitemmodel.cpp \
    snipets/snipetdockitemmodel.cpp \
    utils/treeproxyitemmodel.cpp \
    utils/recursivesortfilterproxymodel.cpp \
    utils/xsltparser.cpp \
    scripts/scriptmanager.cpp \
    scripts/documentsearch.cpp \
    editors/editorfactory.cpp \
    editors/bookmarktexteditorinterface.cpp \
    project/externalfileresolver.cpp \
    rcs/rcsmanager.cpp \
    rcs/commitmessagedialogimpl.cpp \
    project/newprojecttemplate.cpp \
    snipets/snipetcompletionparser.cpp \
    contentview2/contentview2manager.cpp \
    contentview2/contentview2parserfactory.cpp \
    contentview2/contentview2file.cpp \
    contentview2/contentview2project.cpp
FORMS = snipets/callsnipetdlg.ui \
    snipets/snipetpropertydlg.ui \
    snipets/categorypropertydlg.ui \
    rcs/commitmessages.ui
TRANSLATIONS += translations/libxinx_fr.ts
profiling { 
    QMAKE_CXXFLAGS += --coverage \
        -O0
    QMAKE_LFLAGS += --coverage
}
