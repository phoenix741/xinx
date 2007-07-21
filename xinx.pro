CONFIG +=   debug \
  exceptions \
  qt \
  thread \
  warn_on \
  x86
DEFINES +=   USING_PCH
DESTDIR +=   bin
DISTFILES +=   CHANGELOG \
  COPYING \
  TODO \
  src/com.generix.xmlstudio.xml \
  src/genxml.sh \
  xml/completion.cpl
FORMS +=   ui/about.ui \
  ui/commitmessages.ui \
  ui/custom.ui \
  ui/mainform.ui \
  ui/projectproperty.ui \
  ui/rcslogform.ui \
  ui/replace.ui \
  ui/resultatServices.ui \
  ui/servicesconnection.ui
HEADERS +=   src/aboutdialogimpl.h \
  src/commitmessagedialogimpl.h \
  src/connectionwebservicesdialogimpl.h \
  src/customdialogimpl.h \
  src/editor.h \
  src/editorcompletion.h \
  src/fileeditor.h \
  src/flattreeview.h \
  src/globals.h \
  src/javascriptparser.h \
  src/jseditor.h \
  src/jsfileeditor.h \
  src/jshighlighter.h \
  src/kcolorcombo.h \
  src/numberbar.h \
  src/objectview.h \
  src/p_rcs_cvs.h \
  src/projectpropertyimpl.h \
  src/rcs.h \
  src/rcs_cvs.h \
  src/rcslogdialogimpl.h \
  src/replacedialogimpl.h \
  src/serviceresultdialogimpl.h \
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
  src/xsddocument.h \
  src/xsleditor.h \
  src/xsllistview.h \
  src/xslproject.h \
  src/filecontentitemmodel.h \
  src/javascriptfilecontent.h
MOC_DIR +=   build
OBJECTS_DIR +=   build
QT +=   network \
  xml
RCC_DIR +=   build
RESOURCES +=   application.qrc
SOURCES +=   src/aboutdialogimpl.cpp \
  src/commitmessagedialogimpl.cpp \
  src/connectionwebservicesdialogimpl.cpp \
  src/customdialogimpl.cpp \
  src/editor.cpp \
  src/editorcompletion.cpp \
  src/fileeditor.cpp \
  src/flattreeview.cpp \
  src/globals.cpp \
  src/javascriptparser.cpp \
  src/jseditor.cpp \
  src/jsfileeditor.cpp \
  src/jshighlighter.cpp \
  src/kcolorcombo.cpp \
  src/main.cpp \
  src/numberbar.cpp \
  src/objectview.cpp \
  src/p_rcs_cvs.cpp \
  src/projectmainform.cpp \
  src/projectpropertyimpl.cpp \
  src/rcs.cpp \
  src/rcs_cvs.cpp \
  src/rcslogdialogimpl.cpp \
  src/replacedialogimpl.cpp \
  src/searchmainform.cpp \
  src/serviceresultdialogimpl.cpp \
  src/soap.cpp \
  src/syntaxhighlighter.cpp \
  src/tabeditor.cpp \
  src/texteditor.cpp \
  src/uniqueapplication.cpp \
  src/webservices.cpp \
  src/webserviceseditor.cpp \
  src/webservicesmainform.cpp \
  src/wsdl.cpp \
  src/xinxconfig.cpp \
  src/xmleditor.cpp \
  src/xmlfileeditor.cpp \
  src/xmlhighlighter.cpp \
  src/xmlvisualstudio.cpp \
  src/xsddocument.cpp \
  src/xsleditor.cpp \
  src/xsllistview.cpp \
  src/xslproject.cpp \
  src/filecontentitemmodel.cpp \
  src/javascriptfilecontent.cpp
TARGET =   xinx
TEMPLATE =   app
TRANSLATIONS +=   translations/xinx_fr.ts
UI_DIR +=   build
unix {
  CONFIG +=     qdbus
  HEADERS +=     src/studioadaptor.h \
    src/studiointerface.h
  SOURCES +=     src/studioadaptor.cpp \
    src/studiointerface.cpp
}
win32 {
  DISTFILES +=     rc/xinx.rc \
    setup/xinx-setup.iss
  RC_FILE +=     rc/xinx.rc
}
