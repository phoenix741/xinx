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
CONFIG += exceptions \
    qdbus \
    qt \
    thread \
    warn_on \
    x86
QT += network \
    xml
LIBS = -L../plugins/webplugin \
    -L../libxinx \
    -L../components \
    -lwebplugin \
    -lsharedxinx \
    -lxinxcmp
if(!debug_and_release|build_pass):CONFIG(debug, debug|release) { 
    mac:LIBS = $$member(LIBS, 0) \
        $$member(LIBS, 1) \
        $$member(LIBS, 2) \
        $$member(LIBS, 3)_debug \
        $$member(LIBS, 4)_debug \
        $$member(LIBS, 5)_debug
    win32:LIBS = $$member(LIBS, 0) \
        $$member(LIBS, 1) \
        $$member(LIBS, 2) \
        $$member(LIBS, 3)d \
        $$member(LIBS, 4)d \
        $$member(LIBS, 5)d
}
win32:if(!debug_and_release|build_pass):CONFIG(debug, debug|release):POST_TARGETDEPS = ../plugins/webplugin/libwebplugind.a \
    ../libxinx/libsharedxinxd.a \
    ../components/libxinxcmpd.a
else:POST_TARGETDEPS = ../plugins/webplugin/libwebplugin.a \
    ../libxinx/libsharedxinx.a \
    ../components/libxinxcmp.a
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
HEADERS += specifiquedlgimpl.h \
    aboutdialogimpl.h \
    commitmessagedialogimpl.h \
    configurationfile.h \
    connectionwebservicesdialogimpl.h \
    customdialogimpl.h \
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
    private/p_threadedconfigurationfile.h \
    private/p_uniqueapplication.h \
    private/p_xmlpresentationdockwidget.h \
    private/p_flattreeview.h \
    private/p_runsnipetdialogimpl.h \
    private/p_serviceresultdialogimpl.h \
    private/p_snipetdialog.h \
    private/p_webserviceseditor.h \
    projectdirectorydockwidget.h \
    projectpropertyimpl.h \
    rcslogdialogimpl.h \
    replacedialogimpl.h \
    runsnipetdialogimpl.h \
    serviceresultdialogimpl.h \
    snipet.h \
    snipetdialog.h \
    snipetlist.h \
    snipetmodel.h \
    soap.h \
    tabeditor.h \
    texteditor.h \
    threadedconfigurationfile.h \
    uniqueapplication.h \
    webservices.h \
    webserviceseditor.h \
    wsdl.h \
    xmlpresentationdockwidget.h \
    xmlpresentationitem.h \
    xslproject.h \
    private/p_customdialogimpl.h
SOURCES += specifiquedlgimpl.cpp \
    aboutdialogimpl.cpp \
    commitmessagedialogimpl.cpp \
    configurationfile.cpp \
    connectionwebservicesdialogimpl.cpp \
    customdialogimpl.cpp \
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
    rcslogdialogimpl.cpp \
    replacedialogimpl.cpp \
    runsnipetdialogimpl.cpp \
    serviceresultdialogimpl.cpp \
    snipet.cpp \
    snipetdialog.cpp \
    snipetlist.cpp \
    snipetmodel.cpp \
    soap.cpp \
    tabeditor.cpp \
    texteditor.cpp \
    threadedconfigurationfile.cpp \
    uniqueapplication.cpp \
    webservices.cpp \
    webserviceseditor.cpp \
    wsdl.cpp \
    xmlpresentationdockwidget.cpp \
    xmlpresentationitem.cpp \
    xslproject.cpp \
    newprojectwizard.cpp
contains( CONFIG, qdbus ) { 
    HEADERS += studioadaptor.h \
        studiointerface.h
    SOURCES += studioadaptor.cpp \
        studiointerface.cpp
}
TRANSLATIONS += translations/xinx_fr.ts
