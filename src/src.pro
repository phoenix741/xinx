SOURCES +=   main.cpp \
  editor.cpp \
  tabeditor.cpp \
  xmlhighlighter.cpp \
  objectview.cpp \
  javaobjectfileimpl.cpp \
  editorcompletion.cpp \
  texteditor.cpp \
  finddialog.cpp \
  xsllistview.cpp \
  xslproject.cpp \
  uniqueapplication.cpp \
  projectpropertyimpl.cpp \
  xmlvisualstudio.cpp
HEADERS +=   editor.h \
  tabeditor.h \
  xmlhighlighter.h \
  objectview.h \
  javaobjectfileimpl.h \
  editorcompletion.h \
  texteditor.h \
  finddialog.h \
  xsllistview.h \
  xslproject.h \
  uniqueapplication.h \
  projectpropertyimpl.h \
  xmlvisualstudio.h
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
