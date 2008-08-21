TEMPLATE = app
CONFIG += qdbus warn_on
QT += xml script xmlpatterns
DESTDIR += ./
INCLUDEPATH += ../components ../libxinx ../xinx/
LIBS = -L../libxinx -L../plugins -L../components -lwebplugin -lsharedxinx -lxinxcmp
PRE_TARGETDEPS = ../components/libxinxcmp.a ../plugins/libwebplugin.a
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
    ui/replace.ui \
    ui/runsnipet.ui \
    ui/snipetproperty.ui \
    ui/xmlpresentationwidget.ui \
    ui/newprojectwizard_project.ui \
    ui/newprojectwizard_specifique.ui \
 ui/logform.ui
HEADERS += scriptmanager.h \
    snipetdockwidget.h \
    specifiquedlgimpl.h \
    aboutdialogimpl.h \
    commitmessagedialogimpl.h \
    configurationfile.h \
    customdialogimpl.h \
    dirrcsmodel.h \
    filecontentdockwidget.h \
    flattreeview.h \
    iconprojectprovider.h \
    mainformimpl.h \
    newprojectwizard.h \
    private/p_filecontentdockwidget.h \
    private/p_mainformimpl.h \
    private/p_projectdirectorydockwidget.h \
    private/p_xmlpresentationdockwidget.h \
    private/p_flattreeview.h \
    projectdirectorydockwidget.h \
    projectpropertyimpl.h \
    replacedialogimpl.h \
    runsnipetdialogimpl.h \
    snipet.h \
    snipetdialog.h \
    snipetlist.h \
    tabeditor.h \
    threadedconfigurationfile.h \
    uniqueapplication.h \
    xmlpresentationdockwidget.h \
    xmlpresentationitem.h \
    private/p_customdialogimpl.h \
	studioadaptor.h \
	studiointerface.h \
 searchfilethread.h \
 logdialogimpl.h
SOURCES += snipetdockwidget.cpp \
    specifiquedlgimpl.cpp \
    aboutdialogimpl.cpp \
    commitmessagedialogimpl.cpp \
    configurationfile.cpp \
    customdialogimpl.cpp \
    dirrcsmodel.cpp \
    filecontentdockwidget.cpp \
    flattreeview.cpp \
    iconprojectprovider.cpp \
    main.cpp \
    mainformimpl.cpp \
    projectdirectorydockwidget.cpp \
    projectpropertyimpl.cpp \
    replacedialogimpl.cpp \
    runsnipetdialogimpl.cpp \
    snipet.cpp \
    snipetdialog.cpp \
    snipetlist.cpp \
    tabeditor.cpp \
    threadedconfigurationfile.cpp \
    uniqueapplication.cpp \
    xmlpresentationdockwidget.cpp \
    xmlpresentationitem.cpp \
    newprojectwizard.cpp \
    scriptmanager.cpp \
    studioadaptor.cpp \
    studiointerface.cpp \
 searchfilethread.cpp \
 logdialogimpl.cpp
TRANSLATIONS += translations/xinx_fr.ts

include(../project_mode.pro)

xinx.path = /usr/local/xinx
INSTALLS += xinx
