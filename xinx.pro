DESTDIR +=   bin
OBJECTS_DIR +=   build
MOC_DIR +=   build
UI_DIR +=   build
RCC_DIR +=   build
SOURCES +=   src/main.cpp \
  src/xmlhighlighter.cpp \
  src/objectview.cpp \
  src/javaobjectfileimpl.cpp \
  src/editorcompletion.cpp \
  src/finddialog.cpp \
  src/xsllistview.cpp \
  src/xslproject.cpp \
  src/uniqueapplication.cpp \
  src/projectpropertyimpl.cpp \
  src/xmlvisualstudio.cpp \
  src/fileeditor.cpp \
  src/xmleditor.cpp \
  src/tabeditor.cpp \
  src/editor.cpp \
  src/jshighlighter.cpp \
  src/aboutdialogimpl.cpp \
  src/webservices.cpp
HEADERS +=   src/xmlhighlighter.h \
  src/objectview.h \
  src/javaobjectfileimpl.h \
  src/editorcompletion.h \
  src/finddialog.h \
  src/xsllistview.h \
  src/xslproject.h \
  src/uniqueapplication.h \
  src/projectpropertyimpl.h \
  src/xmlvisualstudio.h \
  src/fileeditor.h \
  src/xmleditor.h \
  src/tabeditor.h \
  src/editor.h \
  src/jshighlighter.h \
  src/aboutdialogimpl.h \
  src/webservices.h
FORMS +=   ui/javaobjectfile.ui \
  ui/projectproperty.ui \
  ui/mainform.ui \
  ui/syntaxeditorproperty.ui \
  ui/about.ui
TEMPLATE =   app
CONFIG +=   warn_on \
  thread \
  qt \
  exceptions \
  debug
TARGET =   xinx
RESOURCES +=   images/application.qrc
TRANSLATIONS +=   translations/xinx_fr.ts
QT +=   xml \
  gui \
  network
win32 {
  RC_FILE +=     rc/xinx.rc
}
unix {
  SOURCES +=     src/studioadaptor.cpp \
    src/studiointerface.cpp
  HEADERS +=     src/studioadaptor.h \
    src/studiointerface.h
  CONFIG +=     qdbus
}
