SOURCES =   xmlvisualstudio.cpp \
  main.cpp \
  Editor.cpp \
  TabEditor.cpp \
  xmlhighlighter.cpp \
  ObjectView.cpp \
  javaobjectfileimpl.cpp
HEADERS =   xmlvisualstudio.h \
  Editor.h \
  TabEditor.h \
  xmlhighlighter.h \
  ObjectView.h \
  javaobjectfileimpl.h
CONFIG +=   warn_on \
  thread \
  qt \
  exceptions \
  release 
TARGET =   ../bin/xmlvisualstudio
RESOURCES =   application.qrc
FORMS =   JavaObjectFile.ui
TEMPLATE =   app
QT +=   xml \
  core \
  gui
