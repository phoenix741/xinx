include(../project_mode.pri)
TEMPLATE = app

VERSION = 0.9.0.0
DEFINES += XINX_VERSION=$$replace(VERSION,"\.",",")

win32:QMAKE_RC = windres \
    -DXINX_VERSION=$$replace(VERSION,"\.",",")
win32:RC_FILE += rc/xinx.rc
CONFIG += warn_on
QT += xml \
    script \
    xmlpatterns \
    webkit \
	sql
DESTDIR += ./
PRE_TARGETDEPS = ../plugins/libcoreplugin.a ../ext/ideality/lib/libideality.a
INCLUDEPATH += ../framework \
    ../components \
    ../ext/qcodeedit/lib \
    ../xinx \
    ../ext/qcodeedit/lib/document \
    ../ext/qcodeedit/lib/language \
    ../ext/qcodeedit/lib/qnfa \
	../ext/qcodeedit/lib/widgets \
	../ext/ideality/lib
LIBS += -L../framework \
    -L../plugins \
    -L../components \
    -L../ext/qcodeedit \
	-L../framework/ \
	-L../ext/ideality/lib/ \
    -lcoreplugin \
    -lxinxframework \
    -lxinxcmp \
    -lqcodeedit \
	-lideality
RESOURCES += application.qrc
FORMS += ui/about.ui \
    ui/projectdirectorywidget.ui \
    ui/projectproperty.ui \
    ui/replace.ui \
    ui/newprojectwizard_project.ui \
    ui/logform.ui \
    ui/snipetlistwidget.ui \
    ui/welcomdlg.ui \
    customdialog/ui/customdialog.ui \
    customdialog/ui/customgeneral.ui \
    customdialog/ui/customproject.ui \
    customdialog/ui/customeditor.ui \
    customdialog/ui/customfont.ui \
    customdialog/ui/customsyntax.ui \
    customdialog/ui/customsnipet.ui \
    customdialog/ui/customtools.ui \
    customdialog/ui/custommodules.ui \
    ui/templatedialog.ui \
    ui/newversion.ui
HEADERS += customdialog/customgeneralimpl.h \
    customdialog/customprojectimpl.h \
    customdialog/customeditorimpl.h \
    customdialog/customfontimpl.h \
    customdialog/customsyntaximpl.h \
    customdialog/customsnipetimpl.h \
    customdialog/customtoolsimpl.h \
    customdialog/custommodulesimpl.h \
    customdialog/customdialogimpl.h \
    customdialog/toolsmodelindex.h \
    customdialog/directoryeditdelegate.h \
    customdialog/scriptelement.h \
    snipetdockwidget.h \
    aboutdialogimpl.h \
    filecontentdockwidget.h \
    mainformimpl.h \
    newprojectwizard.h \
    projectdirectorydockwidget.h \
    projectpropertyimpl.h \
    replacedialogimpl.h \
    tabeditor.h \
    uniqueapplication.h \
    searchfilethread.h \
    logdialogimpl.h \
    xinxprojectwizard/projectconverter.h \
    xinxprojectwizard/projectwizard.h \
    welcomdlgimpl.h \
    welcomdlgimpl_p.h \
    templatedialogimpl.h \
    newversionwizardimpl.h
SOURCES += customdialog/customgeneralimpl.cpp \
    customdialog/customprojectimpl.cpp \
    customdialog/customeditorimpl.cpp \
    customdialog/customfontimpl.cpp \
    customdialog/customsyntaximpl.cpp \
    customdialog/customsnipetimpl.cpp \
    customdialog/customtoolsimpl.cpp \
    customdialog/custommodulesimpl.cpp \
    customdialog/directoryeditdelegate.cpp \
    customdialog/toolsmodelindex.cpp \
    customdialog/scriptelement.cpp \
    customdialog/customdialogimpl.cpp \
    snipetdockwidget.cpp \
    aboutdialogimpl.cpp \
    filecontentdockwidget.cpp \
    main.cpp \
    mainformimpl.cpp \
    projectdirectorydockwidget.cpp \
    projectpropertyimpl.cpp \
    replacedialogimpl.cpp \
    tabeditor.cpp \
    uniqueapplication.cpp \
    newprojectwizard.cpp \
    searchfilethread.cpp \
    logdialogimpl.cpp \
    xinxprojectwizard/projectconverter.cpp \
    xinxprojectwizard/projectwizard.cpp \
    welcomdlgimpl.cpp \
    templatedialogimpl.cpp \
    newversionwizardimpl.cpp
TRANSLATIONS += translations/xinx_fr.ts
CONFIG(debug, debug|release):exists( $$[QT_INSTALL_DATA]/qtc-debugging-helper/gdbmacros.cpp ) { 
    message( "Add GDB Macro to program..." )
    SOURCES += $$[QT_INSTALL_DATA]/qtc-debugging-helper/gdbmacros.cpp
}
include(../ext/qtsingleapplication/src/qtsingleapplication.pri)
