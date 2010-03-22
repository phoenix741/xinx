include(../../project_mode.pri)

DESTDIR += ../bin/
SOURCES = testcontentview2.cpp
CONFIG  += qtestlib
QT += sql xml xmlpatterns webkit
INCLUDEPATH += ../../components ../../framework ../../plugins/core/
LIBS = -L ../../components -L../../plugins -L../../framework -L../../ext/qcodeedit -L../../ext/ideality/lib/ -lcoreplugin -lxinxframework -lxinxcmp -lqcodeedit -lideality


profiling {
  QMAKE_CXXFLAGS += --coverage
  QMAKE_LFLAGS   += --coverage
}


include(../../ext/qmodeltest/modeltest.pri)
