include(../../project_mode.pri)

DESTDIR += ./
SOURCES = testsnipets.cpp
CONFIG  += qtestlib
QT += sql
INCLUDEPATH += ../../framework
LIBS = -L ../../components -L../../framework -L../../ext/qcodeedit -lxinxframework -lxinxcmp -lqcodeedit

include(../../ext/qmodeltest/modeltest.pri)
