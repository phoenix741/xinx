SOURCES +=   main.cpp \
  xmlhighlighter.cpp \
  objectview.cpp \
  javaobjectfileimpl.cpp \
  editorcompletion.cpp \
  finddialog.cpp \
  xsllistview.cpp \
  xslproject.cpp \
  uniqueapplication.cpp \
  projectpropertyimpl.cpp \
  xmlvisualstudio.cpp \
  fileeditor.cpp \
  xmleditor.cpp \
  tabeditor.cpp \
  editor.cpp
HEADERS +=   xmlhighlighter.h \
  objectview.h \
  javaobjectfileimpl.h \
  editorcompletion.h \
  finddialog.h \
  xsllistview.h \
  xslproject.h \
  uniqueapplication.h \
  projectpropertyimpl.h \
  xmlvisualstudio.h \
  fileeditor.h \
  xmleditor.h \
  tabeditor.h \
  editor.h
TEMPLATE =   app
CONFIG +=   warn_on \
  thread \
  qt \
  exceptions
TARGET =   ../bin/xmlvisualstudio
RESOURCES +=   application.qrc
QT +=   xml \
  gui
FORMS +=   javaobjectfile.ui \
  projectproperty.ui \
  mainform.ui \
  syntaxeditorproperty.ui
win32 {
  CONFIG +=     release
}
unix {
  SOURCES +=     studioadaptor.cpp \
    studiointerface.cpp
  HEADERS +=     studioadaptor.h \
    studiointerface.h
  CONFIG +=     debug \
    qdbus
}
