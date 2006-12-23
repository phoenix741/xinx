SOURCES =   xmlvisualstudio.cpp \
  main.cpp \
  editor.cpp \
  tabeditor.cpp \
  xmlhighlighter.cpp \
  objectview.cpp \
  javaobjectfileimpl.cpp \
  editorcompletion.cpp \
  texteditor.cpp \
  finddialog.cpp
HEADERS =   xmlvisualstudio.h \
  editor.h \
  tabeditor.h \
  xmlhighlighter.h \
  objectview.h \
  javaobjectfileimpl.h \
  editorcompletion.h \
  texteditor.h \
  finddialog.h
TEMPLATE =   app
CONFIG +=   warn_on \
  thread \
  qt \
  exceptions \
  debug
TARGET =   ../bin/xmlvisualstudio
RESOURCES =   application.qrc
QT +=   xml \
  gui
FORMS =   javaobjectfile.ui
