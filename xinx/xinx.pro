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
INCLUDEPATH += ../components \
  ../libxinx \
  ../xinx/
CONFIG += exceptions \
    qdbus \
    qt \
    thread \
    warn_on \
    x86 \
 debug
QT += network \
    xml \
    script

LIBS = -L../libxinx \
	-L../plugins \
    -L../components \
    -lwebplugin \
    -lsharedxinx \
    -lxinxcmp
if(!debug_and_release|build_pass):CONFIG(debug, debug|release){
    mac : LIBS = $$member(LIBS, 0) \
        $$member(LIBS, 1) \
        $$member(LIBS, 2) \
        $$member(LIBS, 3)_debug \
        $$member(LIBS, 4)_debug \
        $$member(LIBS, 5)_debug
    win32 : LIBS = $$member(LIBS, 0) \
        $$member(LIBS, 1) \
        $$member(LIBS, 2) \
        $$member(LIBS, 3)d \
        $$member(LIBS, 4)d \
        $$member(LIBS, 5)d
}
win32 : if(!debug_and_release|build_pass):CONFIG(debug, debug|release): POST_TARGETDEPS = ../plugins/libwebplugind.a \
    ../components/libxinxcmpd.a
else : POST_TARGETDEPS = ../plugins/libwebplugin.a \
    ../components/libxinxcmp.a
win32 : RC_FILE += rc/xinx.rc
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
    ui/newprojectwizard_specifique.ui
HEADERS += scriptmanager.h \
    snipetdockwidget.h \
    specifiquedlgimpl.h \
    aboutdialogimpl.h \
    commitmessagedialogimpl.h \
    configurationfile.h \
    connectionwebservicesdialogimpl.h \
    customdialogimpl.h \
    dirrcsmodel.h \
    filecontentdockwidget.h \
    flattreeview.h \
    iconprojectprovider.h \
    mainformimpl.h \
    newprojectwizard.h \
    private/p_configurationfile.h \
    private/p_connectionwebservicesdialogimpl.h \
    private/p_filecontentdockwidget.h \
    private/p_mainformimpl.h \
    private/p_projectdirectorydockwidget.h \
    private/p_threadedconfigurationfile.h \
    private/p_uniqueapplication.h \
    private/p_xmlpresentationdockwidget.h \
    private/p_flattreeview.h \
    private/p_serviceresultdialogimpl.h \
    projectdirectorydockwidget.h \
    projectpropertyimpl.h \
    rcslogdialogimpl.h \
    replacedialogimpl.h \
    runsnipetdialogimpl.h \
    serviceresultdialogimpl.h \
    snipet.h \
    snipetdialog.h \
    snipetlist.h \
    soap.h \
    tabeditor.h \
    threadedconfigurationfile.h \
    uniqueapplication.h \
    xmlpresentationdockwidget.h \
    xmlpresentationitem.h \
    private/p_customdialogimpl.h \
    webservices.h \
    wsdl.h
SOURCES += snipetdockwidget.cpp \
    specifiquedlgimpl.cpp \
    aboutdialogimpl.cpp \
    commitmessagedialogimpl.cpp \
    configurationfile.cpp \
    connectionwebservicesdialogimpl.cpp \
    customdialogimpl.cpp \
    dirrcsmodel.cpp \
    filecontentdockwidget.cpp \
    flattreeview.cpp \
    iconprojectprovider.cpp \
    main.cpp \
    mainformimpl.cpp \
    projectdirectorydockwidget.cpp \
    projectpropertyimpl.cpp \
    rcslogdialogimpl.cpp \
    replacedialogimpl.cpp \
    runsnipetdialogimpl.cpp \
    serviceresultdialogimpl.cpp \
    snipet.cpp \
    snipetdialog.cpp \
    snipetlist.cpp \
    soap.cpp \
    tabeditor.cpp \
    threadedconfigurationfile.cpp \
    uniqueapplication.cpp \
    xmlpresentationdockwidget.cpp \
    xmlpresentationitem.cpp \
    newprojectwizard.cpp \
    scriptmanager.cpp \
    webservices.cpp \
    wsdl.cpp
contains( CONFIG, qdbus ){
    HEADERS += studioadaptor.h \
        studiointerface.h
    SOURCES += studioadaptor.cpp \
        studiointerface.cpp
}
TRANSLATIONS += translations/xinx_fr.ts
