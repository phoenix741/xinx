TEMPLATE = app
TARGET = xinx

DESTDIR += ../bin
MOC_DIR += ../build
OBJECTS_DIR += ../build
RCC_DIR += ../build
UI_DIR += ../build
unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
INCLUDEPATH += ../src/ ../include

CONFIG += debug \
    exceptions \
    qdbus \
    qt \
    thread \
    warn_on \
    x86
QT += network \
    xml
    
LIBS = -L../bin/plugins -lxinx_baseplugin

DISTFILES = ../CHANGELOG \
    ../COPYING \
    ../Doxyfile \
    ../INSTALL \
    ../TODO \
    ../application.qrc \
    ../rc/*.rc \
    ../setup/*.bat \
    ../setup/*.iss \
    *.bat \
    *.cpp \
    *.h \
    *.sh \
    *.xml \
    private/*.h \
    ../translations/*.ts \
    ../ui/*.ui \
    ../xml/*.xnx

win32 { 
    QMAKE_LFLAGS = -limagehlp \
        -lpsapi
    RC_FILE += ../rc/xinx.rc
}
RESOURCES += ../application.qrc
FORMS += ../ui/plugindialog.ui \
    ../ui/specifiquedlg.ui \
    ../ui/about.ui \
    ../ui/commitmessages.ui \
    ../ui/custom.ui \
    ../ui/mainform.ui \
    ../ui/projectdirectorywidget.ui \
    ../ui/projectproperty.ui \
    ../ui/rcslogform.ui \
    ../ui/replace.ui \
    ../ui/resultatServices.ui \
    ../ui/runsnipet.ui \
    ../ui/servicesconnection.ui \
    ../ui/snipetproperty.ui \
    ../ui/xmlpresentationwidget.ui
HEADERS += plugindialog.h \
    xinxpluginsloader.h \
    interfaces.h \
    specifiquedlgimpl.h \
    filecontentstructure.h \
    xinxthread.h \
    aboutdialogimpl.h \
    appsettings.h \
    commitmessagedialogimpl.h \
    configurationfile.h \
    connectionwebservicesdialogimpl.h \
    customdialogimpl.h \
    cvsfiles.h \
    cvsthread.h \
    directoryedit.h \
    dirrcsmodel.h \
    editor.h \
    editorcompletion.h \
    exceptions.h \
    filecontentdockwidget.h \
    filecontentitemmodel.h \
    fileeditor.h \
    filetypeinterface.h \
    filetypejs.h \
    filetypexml.h \
    filetypexsl.h \
    filewatcher.h \
    flattreeview.h \
    globals.h \
    iconprojectprovider.h \
    javascriptmodelcompleter.h \
    javascriptparser.h \
    jsfileeditor.h \
    kcolorcombo.h \
    mainformimpl.h \
    numberbar.h \
    objectview.h \
    private/p_configurationfile.h \
    private/p_connectionwebservicesdialogimpl.h \
    private/p_filecontentdockwidget.h \
    private/p_fileeditor.h \
    private/p_filetypeinterface.h \
    private/p_filetypejs.h \
    private/p_filetypexsl.h \
    private/p_filewatcher.h \
    private/p_mainformimpl.h \
    private/p_projectdirectorydockwidget.h \
    private/p_rcs_cvs.h \
    private/p_threadedconfigurationfile.h \
    private/p_uniqueapplication.h \
    private/p_xmlpresentationdockwidget.h \
    private/p_flattreeview.h \
    private/p_runsnipetdialogimpl.h \
    private/p_serviceresultdialogimpl.h \
    private/p_snipetdialog.h \
    private/p_snipetlist.h \
    private/p_webserviceseditor.h \
    projectdirectorydockwidget.h \
    projectpropertyimpl.h \
    rcs.h \
    rcs_cvs.h \
    rcslogdialogimpl.h \
    replacedialogimpl.h \
    runsnipetdialogimpl.h \
    serviceresultdialogimpl.h \
    snipet.h \
    snipetdialog.h \
    snipetlist.h \
    snipetmodel.h \
    soap.h \
    syntaxhighlighter.h \
    tabeditor.h \
    texteditor.h \
    threadedconfigurationfile.h \
    uniqueapplication.h \
    webservices.h \
    webserviceseditor.h \
    wsdl.h \
    xinxconfig.h \
    xmlfileeditor.h \
    xmlpresentationdockwidget.h \
    xmlpresentationitem.h \
    xsllistview.h \
    xslmodelcompleter.h \
    xslproject.h
SOURCES += plugindialog.cpp \
    xinxpluginsloader.cpp \
    specifiquedlgimpl.cpp \
    filecontentstructure.cpp \
    aboutdialogimpl.cpp \
    appsettings.cpp \
    commitmessagedialogimpl.cpp \
    configurationfile.cpp \
    connectionwebservicesdialogimpl.cpp \
    customdialogimpl.cpp \
    cvsfiles.cpp \
    cvsthread.cpp \
    directoryedit.cpp \
    dirrcsmodel.cpp \
    editor.cpp \
    editorcompletion.cpp \
    exceptions.cpp \
    filecontentdockwidget.cpp \
    filecontentitemmodel.cpp \
    fileeditor.cpp \
    filetypeinterface.cpp \
    filetypejs.cpp \
    filetypexml.cpp \
    filetypexsl.cpp \
    filewatcher.cpp \
    flattreeview.cpp \
    globals.cpp \
    iconprojectprovider.cpp \
    javascriptmodelcompleter.cpp \
    javascriptparser.cpp \
    jsfileeditor.cpp \
    kcolorcombo.cpp \
    main.cpp \
    mainformimpl.cpp \
    numberbar.cpp \
    objectview.cpp \
    projectdirectorydockwidget.cpp \
    projectpropertyimpl.cpp \
    rcs.cpp \
    rcs_cvs.cpp \
    rcslogdialogimpl.cpp \
    replacedialogimpl.cpp \
    runsnipetdialogimpl.cpp \
    serviceresultdialogimpl.cpp \
    snipet.cpp \
    snipetdialog.cpp \
    snipetlist.cpp \
    snipetmodel.cpp \
    soap.cpp \
    syntaxhighlighter.cpp \
    tabeditor.cpp \
    texteditor.cpp \
    threadedconfigurationfile.cpp \
    uniqueapplication.cpp \
    webservices.cpp \
    webserviceseditor.cpp \
    wsdl.cpp \
    xinxconfig.cpp \
    xmlfileeditor.cpp \
    xmlpresentationdockwidget.cpp \
    xmlpresentationitem.cpp \
    xsllistview.cpp \
    xslmodelcompleter.cpp \
    xslproject.cpp \
    xinxthread.cpp
TRANSLATIONS += ../translations/xinx_fr.ts
contains( CONFIG, qdbus ) { 
    HEADERS += studioadaptor.h \
        studiointerface.h
    SOURCES += studioadaptor.cpp \
        studiointerface.cpp
}
