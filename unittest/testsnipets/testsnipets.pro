include(../../project_mode.pri)

DESTDIR += ./
SOURCES = testsnipets.cpp
CONFIG += qtestlib
QT += sql
INCLUDEPATH += ../../framework

include(../../ext/qmodeltest/modeltest.pri)


profiling {
    QMAKE_CXXFLAGS += --coverage
    QMAKE_LFLAGS += --coverage
}TEMPLATE = app

LIBS += -L../../components \
  -lxinxcmp \
  -L../../framework \
  -lxinxframework \
  -L../../components/ \
  -L../../framework/ \
  -L../../ext/qcodeedit \
  -lcomponents \
  -lframework \
  ../../components \
  -lqcodeedit

