CONFIG +=   exceptions \
  qt \
  warn_on \
  debug
DESTDIR +=   bin
FORMS +=   ui/about.ui \
  ui/crudpropertyservicefile.ui \
  ui/custom.ui \
  ui/mainform.ui \
  ui/newservicefile.ui \
  ui/projectproperty.ui \
  ui/replace.ui \
  ui/resultatServices.ui
HEADERS +=   src/aboutdialogimpl.h \
  src/customdialogimpl.h \
  src/editor.h \
  src/editorcompletion.h \
  src/fileeditor.h \
  src/jseditor.h \
  src/jshighlighter.h \
  src/kcolorcombo.h \
  src/newwebservicesdialogimpl.h \
  src/objectview.h \
  src/projectpropertyimpl.h \
  src/replacedialogimpl.h \
  src/serviceresultdialogimpl.h \
  src/syntaxhighlighter.h \
  src/tabeditor.h \
  src/uniqueapplication.h \
  src/webservices.h \
  src/webservicespropertydialogimpl.h \
  src/xinxconfig.h \
  src/xmleditor.h \
  src/xmlhighlighter.h \
  src/xmlvisualstudio.h \
  src/xsddocument.h \
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
  src/customdialogimpl.cpp \
  src/editor.cpp \
  src/editorcompletion.cpp \
  src/fileeditor.cpp \
  src/jseditor.cpp \
  src/jshighlighter.cpp \
  src/kcolorcombo.cpp \
  src/main.cpp \
  src/newwebservicesdialogimpl.cpp \
  src/objectview.cpp \
  src/projectpropertyimpl.cpp \
  src/replacedialogimpl.cpp \
  src/serviceresultdialogimpl.cpp \
  src/syntaxhighlighter.cpp \
  src/tabeditor.cpp \
  src/uniqueapplication.cpp \
  src/webservices.cpp \
  src/webservicespropertydialogimpl.cpp \
  src/xinxconfig.cpp \
  src/xmleditor.cpp \
  src/xmlhighlighter.cpp \
  src/xmlvisualstudio.cpp \
  src/xsddocument.cpp \
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
