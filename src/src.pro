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
FORMS =   JavaObjectFile.ui
