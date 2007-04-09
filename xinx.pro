CONFIG +=   exceptions \
  qt \
  thread \
  warn_on \
  debug
DESTDIR +=   bin
FORMS +=   ui/about.ui \
  ui/custom.ui \
  ui/javaobjectfile.ui \
  ui/mainform.ui \
  ui/projectproperty.ui
HEADERS +=   src/aboutdialogimpl.h \
  src/editor.h \
  src/editorcompletion.h \
  src/fileeditor.h \
  src/finddialog.h \
  src/javaobjectfileimpl.h \
  src/jseditor.h \
  src/jshighlighter.h \
  src/objectview.h \
  src/projectpropertyimpl.h \
  src/tabeditor.h \
  src/uniqueapplication.h \
  src/webservices.h \
  src/xmleditor.h \
  src/xmlhighlighter.h \
  src/xmlvisualstudio.h \
  src/xsllistview.h \
  src/xslproject.h \
  src/customdialogimpl.h \
  src/kcolorcombo.h \
  src/xinxconfig.h \
  src/syntaxhighlighter.h
MOC_DIR +=   build
OBJECTS_DIR +=   build
QT +=   gui \
  network \
  xml
RCC_DIR +=   build
RESOURCES +=   images/application.qrc
SOURCES +=   src/aboutdialogimpl.cpp \
  src/editor.cpp \
  src/editorcompletion.cpp \
  src/fileeditor.cpp \
  src/finddialog.cpp \
  src/javaobjectfileimpl.cpp \
  src/jseditor.cpp \
  src/jshighlighter.cpp \
  src/main.cpp \
  src/objectview.cpp \
  src/projectpropertyimpl.cpp \
  src/tabeditor.cpp \
  src/uniqueapplication.cpp \
  src/webservices.cpp \
  src/xmleditor.cpp \
  src/xmlhighlighter.cpp \
  src/xmlvisualstudio.cpp \
  src/xsllistview.cpp \
  src/xslproject.cpp \
  src/customdialogimpl.cpp \
  src/kcolorcombo.cpp \
  src/xinxconfig.cpp \
  src/syntaxhighlighter.cpp
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
