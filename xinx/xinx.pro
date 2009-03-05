TEMPLATE = app
CONFIG += qdbus \
    warn_on
QT += xml \
    script \
    xmlpatterns
DESTDIR += ./
INCLUDEPATH += ../components \
    ../libxinx \
    ../xinx \
    ../ext/qcodeedit/lib \
    ../ext/qcodeedit/lib/document \
    ../ext/qcodeedit/lib/language \
    ../ext/qcodeedit/lib/qnfa \
    ../ext/qcodeedit/lib/widgets
LIBS = -L../libxinx \
    -L../plugins \
    -L../components \
    -L../ext/qcodeedit \
    -lwebplugin \
    -lsharedxinx \
    -lxinxcmp \
    -lqcodeedit
PRE_TARGETDEPS = ../plugins/libwebplugin.a
win32 : RC_FILE += rc/xinx.rc
RESOURCES += application.qrc
FORMS += ui/xquery.ui \
    ui/specifiquedlg.ui \
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
HEADERS += xmlpresentationdockthread.h \
    customdialogmodeles.h \
    xquerydialogimpl.h \
    scriptmanager.h \
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
    dbus/orgshadowarexinxadaptor.h \
    dbus/orgshadowarexinxinterface.h \
    searchfilethread.h \
    logdialogimpl.h \
 script/documentsearch.h
SOURCES += xmlpresentationdockthread.cpp \
    customdialogmodeles.cpp \
    xquerydialogimpl.cpp \
    snipetdockwidget.cpp \
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
    dbus/orgshadowarexinxadaptor.cpp \
    dbus/orgshadowarexinxinterface.cpp \
    searchfilethread.cpp \
    logdialogimpl.cpp \
 script/documentsearch.cpp
TRANSLATIONS += translations/xinx_fr.ts
include(../project_mode.pro)
