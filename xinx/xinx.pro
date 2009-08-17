include(../project_mode.pri)

TEMPLATE = app
VERSION = 0.9.0.0

DEFINES += XINX_VERSION=$$replace(VERSION,"\.",",")
win32 : QMAKE_RC = windres -DXINX_VERSION=$$replace(VERSION,"\.",",")
win32 : RC_FILE += rc/xinx.rc

CONFIG += warn_on
QT += xml \
		script \
		xmlpatterns \
		webkit \
		sql
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
	-lxmlpres \
    -lwebplugin \
		-lsharedxinx \
		-lxinxcmp \
    -lqcodeedit
RESOURCES += application.qrc
FORMS += ui/specifiquedlg.ui \
    ui/about.ui \
    ui/commitmessages.ui \
    ui/custom.ui \
    ui/projectdirectorywidget.ui \
    ui/projectproperty.ui \
    ui/replace.ui \
    ui/runsnipet.ui \
    ui/snipetproperty.ui \
    ui/newprojectwizard_project.ui \
    ui/newprojectwizard_specifique.ui \
    ui/logform.ui
HEADERS += customdialogmodeles.h \
    scriptmanager.h \
    snipetdockwidget.h \
    specifiquedlgimpl.h \
    aboutdialogimpl.h \
    commitmessagedialogimpl.h \
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
    snipetdialog.h \
    tabeditor.h \
    threadedconfigurationfile.h \
    uniqueapplication.h \
    searchfilethread.h \
    logdialogimpl.h \
 script/documentsearch.h \
 xinxprojectwizard/projectconverter.h \
 xinxprojectwizard/projectwizard.h
SOURCES += customdialogmodeles.cpp \
    snipetdockwidget.cpp \
    specifiquedlgimpl.cpp \
    aboutdialogimpl.cpp \
    commitmessagedialogimpl.cpp \
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
    snipetdialog.cpp \
    tabeditor.cpp \
    threadedconfigurationfile.cpp \
    uniqueapplication.cpp \
    newprojectwizard.cpp \
    scriptmanager.cpp \
    searchfilethread.cpp \
    logdialogimpl.cpp \
 script/documentsearch.cpp  \
 xinxprojectwizard/projectconverter.cpp \
 xinxprojectwizard/projectwizard.cpp
TRANSLATIONS += translations/xinx_fr.ts
CONFIG(debug, debug|release) {
	exists( $$[QT_INSTALL_DATA]/qtc-debugging-helper/gdbmacros.cpp ) {
		message( "Add GDB Macro to program..." )
		SOURCES += $$[QT_INSTALL_DATA]/qtc-debugging-helper/gdbmacros.cpp
	}
}

include(../ext/qmodeltest/modeltest.pri)
include(../ext/qtsingleapplication/src/qtsingleapplication.pri)
