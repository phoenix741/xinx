TEMPLATE = app
TARGET = xinx
xinx.path = /usr/local/xinx
INSTALLS += xinx
DESTDIR += ./
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
INCLUDEPATH += ../xinx/ \
    ../libxinx \
    ../components
CONFIG += debug \
    exceptions \
    qdbus \
    qt \
    thread \
    warn_on \
    x86
QT += network \
    xml
LIBS = -L../plugins/cvsplugin \
    -L../plugins/webplugin \
    -L../libxinx \
    -L../components
win32:CONFIG(debug) { 
    LIBS += -lsharedxinxd \
        -lxinxcmpd \
        -lwebplugind \
        -lcvsplugind
    POST_TARGETDEPS = ../plugins/cvsplugin/libcvsplugind.a \
        ../plugins/webplugin/libwebplugind.a \
        ../libxinx/libsharedxinxd.a \
        ../components/libxinxcmpd.a
}
else { 
    LIBS += -lsharedxinx \
        -lxinxcmp \
        -lwebplugin \
        -lcvsplugin
    POST_TARGETDEPS = ../plugins/cvsplugin/libcvsplugin.a \
        ../plugins/webplugin/libwebplugin.a \
        ../libxinx/libsharedxinx.a \
        ../components/libxinxcmp.a
}
win32:RC_FILE += rc/xinx.rc
DISTFILES = ../CHANGELOG \
    ../COPYING \
    ../Doxyfile \
    ../INSTALL \
    ../TODO \
    application.qrc \
    rc/*.rc \
    setup/*.bat \
    setup/*.iss \
    *.bat \
    *.cpp \
    *.h \
    *.sh \
    *.xml \
    private/*.h \
    translations/*.ts \
    ui/*.ui \
    xml/*.xnx
RESOURCES += application.qrc
FORMS += ui/specifiquedlg.ui \
    ui/about.ui \
    ui/commitmessages.ui \
    ui/custom.ui \
    ui/mainform.ui \
    ui/projectdirectorywidget.ui \
    ui/projectproperty.ui \
    ui/rcslogform.ui \
    ui/replace.ui \
    ui/resultatServices.ui \
    ui/runsnipet.ui \
    ui/servicesconnection.ui \
    ui/snipetproperty.ui \
    ui/xmlpresentationwidget.ui \
    ui/newprojectwizard_project.ui \
    ui/newprojectwizard_specifique.ui \
    ui/newprojectwizard_services.ui \
    ui/newprojectwizard_serviceslist.ui
HEADERS += xinxcore.h \
    xinxpluginsloader.h \
    specifiquedlgimpl.h \
    xinxthread.h \
    aboutdialogimpl.h \
    appsettings.h \
    commitmessagedialogimpl.h \
    configurationfile.h \
    connectionwebservicesdialogimpl.h \
    customdialogimpl.h \
    cvsfiles.h \
    cvsthread.h \
    dirrcsmodel.h \
    editor.h \
    filecontentdockwidget.h \
    filecontentitemmodel.h \
    fileeditor.h \
    filewatcher.h \
    flattreeview.h \
    iconprojectprovider.h \
    mainformimpl.h \
    numberbar.h \
    newprojectwizard.h \
    private/p_configurationfile.h \
    private/p_connectionwebservicesdialogimpl.h \
    private/p_filecontentdockwidget.h \
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
    xmlpresentationdockwidget.h \
    xmlpresentationitem.h \
    xslproject.h \
    private/p_customdialogimpl.h
SOURCES += xinxcore.cpp \
    xinxpluginsloader.cpp \
    specifiquedlgimpl.cpp \
    aboutdialogimpl.cpp \
    appsettings.cpp \
    commitmessagedialogimpl.cpp \
    configurationfile.cpp \
    connectionwebservicesdialogimpl.cpp \
    customdialogimpl.cpp \
    cvsfiles.cpp \
    cvsthread.cpp \
    dirrcsmodel.cpp \
    editor.cpp \
    filecontentdockwidget.cpp \
    filecontentitemmodel.cpp \
    fileeditor.cpp \
    filewatcher.cpp \
    flattreeview.cpp \
    iconprojectprovider.cpp \
    main.cpp \
    mainformimpl.cpp \
    numberbar.cpp \
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
    xmlpresentationdockwidget.cpp \
    xmlpresentationitem.cpp \
    xslproject.cpp \
    xinxthread.cpp \
    newprojectwizard.cpp
contains( CONFIG, qdbus ) { 
    HEADERS += studioadaptor.h \
        studiointerface.h
    SOURCES += studioadaptor.cpp \
        studiointerface.cpp
}
TRANSLATIONS += translations/xinx_fr.ts
