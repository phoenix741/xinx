include(../../project_mode.pri)

DESTDIR += ../
SOURCES = testcontentview.cpp
CONFIG  += qtestlib
INCLUDEPATH += ../../framework
LIBS = -L ../../components -L../../framework -L../../ext/qcodeedit -lxinxframework -lxinxcmp -lqcodeedit


profiling {
  QMAKE_CXXFLAGS += --coverage
  QMAKE_LFLAGS   += --coverage
}