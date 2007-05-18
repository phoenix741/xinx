CONFIG +=   debug \
  exceptions \
  qt \
  warn_on
DESTDIR +=   bin
FORMS +=   ui/about.ui \
  ui/crudpropertyservicefile.ui \
  ui/custom.ui \
  ui/mainform.ui \
  ui/newservicefile.ui \
  ui/projectproperty.ui \
  ui/replace.ui \
  ui/resultatServices.ui \
  ui/servicesconnection.ui
HEADERS +=   src/aboutdialogimpl.h \
  src/connectionwebservicesdialogimpl.h \
  src/customdialogimpl.h \
  src/editor.h \
  src/editorcompletion.h \
  src/fileeditor.h \
  src/jseditor.h \
  src/jsfileeditor.h \
  src/jshighlighter.h \
  src/kcolorcombo.h \
  src/newwebservicesdialogimpl.h \
  src/numberbar.h \
  src/objectview.h \
  src/projectpropertyimpl.h \
  src/replacedialogimpl.h \
  src/serviceresultdialogimpl.h \
  src/soap.h \
  src/syntaxhighlighter.h \
  src/tabeditor.h \
  src/texteditor.h \
  src/uniqueapplication.h \
  src/webservices.h \
  src/webserviceseditor.h \
  src/webservicespropertydialogimpl.h \
  src/wsdl.h \
  src/xinxconfig.h \
  src/xmleditor.h \
  src/xmlfileeditor.h \
  src/xmlhighlighter.h \
  src/xmlvisualstudio.h \
  src/xsddocument.h \
  src/xsleditor.h \
  src/xsllistview.h \
  src/xslproject.h
MOC_DIR +=   build
OBJECTS_DIR +=   build
QT +=   gui \
  network \
  xml
RCC_DIR +=   build
RESOURCES +=   images/application.qrc
SOURCES +=   src/aboutdialogimpl.cpp \
  src/connectionwebservicesdialogimpl.cpp \
  src/customdialogimpl.cpp \
  src/editor.cpp \
  src/editorcompletion.cpp \
  src/fileeditor.cpp \
  src/jseditor.cpp \
  src/jsfileeditor.cpp \
  src/jshighlighter.cpp \
  src/kcolorcombo.cpp \
  src/main.cpp \
  src/newwebservicesdialogimpl.cpp \
  src/numberbar.cpp \
  src/objectview.cpp \
  src/projectmainform.cpp \
  src/projectpropertyimpl.cpp \
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
  src/webservicespropertydialogimpl.cpp \
  src/wsdl.cpp \
  src/xinxconfig.cpp \
  src/xmleditor.cpp \
  src/xmlfileeditor.cpp \
  src/xmlhighlighter.cpp \
  src/xmlvisualstudio.cpp \
  src/xsddocument.cpp \
  src/xsleditor.cpp \
  src/xsllistview.cpp \
  src/xslproject.cpp
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
  RC_FILE +=     rc/xinx.rc
}
