# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./demo6
# Target is an application:  

HEADERS += mainwindow.h \
           abstractmanager.h \
           designermanager.h \
           defs.h \
           assistantmanager.h \
           codemanager.h 
SOURCES += demo6.cpp \
           mainwindow.cpp \
           designermanager.cpp \
           assistantmanager.cpp \
           codemanager.cpp 
KDEV_QTVER = 4

LIBS += -lQtDesignerComponents
IDEALITY_DIR = ../lib
include($$IDEALITY_DIR/ideality.pri)


CONFIG += release \
warn_on designer assistant
TEMPLATE = app

RESOURCES += demo6.qrc
