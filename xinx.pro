CONFIG += debug \
 exceptions \
 qdbus \
 qt \
 thread \
 warn_on \
 x86
DESTDIR += bin
DISTFILES = CHANGELOG \
 COPYING \
 Doxyfile \
 INSTALL \
 TODO \
 application.qrc \
 rc/*.rc \
 setup/*.bat \
 setup/*.iss \
 src/*.bat \
 src/*.cpp \
 src/*.h \
 src/*.sh \
 src/*.xml \
 translations/*.ts \
 ui/*.ui \
 xml/*.xnx
FORMS += ui/about.ui \
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
 ui/snipetproperty.ui
HEADERS += src/aboutdialogimpl.h \
 src/commitmessagedialogimpl.h \
 src/connectionwebservicesdialogimpl.h \
 src/customdialogimpl.h \
 src/dirrcsmodel.h \
 src/editor.h \
 src/editorcompletion.h \
 src/filecontentdockwidget.h \
 src/filecontentitemmodel.h \
 src/fileeditor.h \
 src/flattreeview.h \
 src/globals.h \
 src/iconprojectprovider.h \
 src/javascriptfilecontent.h \
 src/javascriptmodelcompleter.h \
 src/javascriptparser.h \
 src/jseditor.h \
 src/jsfileeditor.h \
 src/jshighlighter.h \
 src/kcolorcombo.h \
 src/mainformimpl.h \
 src/numberbar.h \
 src/objectview.h \
 src/p_rcs_cvs.h \
 src/private/p_mainformimpl.h \
 src/private/p_projectdirectorydockwidget.h \
 src/projectdirectorydockwidget.h \
 src/projectpropertyimpl.h \
 src/rcs.h \
 src/rcs_cvs.h \
 src/rcslogdialogimpl.h \
 src/replacedialogimpl.h \
 src/runsnipetdialogimpl.h \
 src/serviceresultdialogimpl.h \
 src/snipet.h \
 src/snipetdialog.h \
 src/snipetlist.h \
 src/snipetmodel.h \
 src/soap.h \
 src/stable_header.h \
 src/syntaxhighlighter.h \
 src/tabeditor.h \
 src/texteditor.h \
 src/uniqueapplication.h \
 src/webservices.h \
 src/webserviceseditor.h \
 src/wsdl.h \
 src/xinxconfig.h \
 src/xmleditor.h \
 src/xmlfileeditor.h \
 src/xmlhighlighter.h \
 src/xmlvisualstudio.h \
 src/xsleditor.h \
 src/xsllistview.h \
 src/xslproject.h
MOC_DIR += build
OBJECTS_DIR += build
QT += network xml
RCC_DIR += build
RESOURCES += application.qrc
SOURCES += src/aboutdialogimpl.cpp \
 src/commitmessagedialogimpl.cpp \
 src/connectionwebservicesdialogimpl.cpp \
 src/customdialogimpl.cpp \
 src/dirrcsmodel.cpp \
 src/editor.cpp \
 src/editorcompletion.cpp \
 src/filecontentdockwidget.cpp \
 src/filecontentitemmodel.cpp \
 src/fileeditor.cpp \
 src/flattreeview.cpp \
 src/globals.cpp \
 src/iconprojectprovider.cpp \
 src/javascriptfilecontent.cpp \
 src/javascriptmodelcompleter.cpp \
 src/javascriptparser.cpp \
 src/jseditor.cpp \
 src/jsfileeditor.cpp \
 src/jshighlighter.cpp \
 src/kcolorcombo.cpp \
 src/main.cpp \
 src/mainformimpl.cpp \
 src/numberbar.cpp \
 src/objectview.cpp \
 src/p_rcs_cvs.cpp \
 src/projectdirectorydockwidget.cpp \
 src/projectpropertyimpl.cpp \
 src/rcs.cpp \
 src/rcs_cvs.cpp \
 src/rcslogdialogimpl.cpp \
 src/replacedialogimpl.cpp \
 src/runsnipetdialogimpl.cpp \
 src/serviceresultdialogimpl.cpp \
 src/snipet.cpp \
 src/snipetdialog.cpp \
 src/snipetlist.cpp \
 src/snipetmodel.cpp \
 src/soap.cpp \
 src/syntaxhighlighter.cpp \
 src/tabeditor.cpp \
 src/texteditor.cpp \
 src/uniqueapplication.cpp \
 src/webservices.cpp \
 src/webserviceseditor.cpp \
 src/wsdl.cpp \
 src/xinxconfig.cpp \
 src/xmleditor.cpp \
 src/xmlfileeditor.cpp \
 src/xmlhighlighter.cpp \
 src/xmlvisualstudio.cpp \
 src/xsleditor.cpp \
 src/xsllistview.cpp \
 src/xslproject.cpp
TARGET = xinx
TEMPLATE = app
TRANSLATIONS += translations/xinx_fr.ts
UI_DIR += build
contains( CONFIG, qdbus ) {
 HEADERS +=  src/studioadaptor.h  src/studiointerface.h
 SOURCES +=  src/studioadaptor.cpp  src/studiointerface.cpp
}
unix {
 QMAKE_CC =  "ccache gcc"
 QMAKE_CXX =  "ccache gcc"
}
win32 {
 RC_FILE +=  rc/xinx.rc
}
