include(../../project_mode.pri)

DESTDIR += ./
SOURCES = testxinxcodeedit.cpp
CONFIG  += qtestlib
INCLUDEPATH += ../../framework ../../ext/qcodeedit/lib ../../ext/qcodeedit/lib/document ../../ext/qcodeedit/lib/language
LIBS = -L ../../components -L../../framework -L../../ext/qcodeedit -lxinxframework -lxinxcmp -lqcodeedit

profiling {
  QMAKE_CXXFLAGS += --coverage
  QMAKE_LFLAGS   += --coverage
}