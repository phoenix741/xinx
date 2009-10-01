include(../../project_mode.pri)

DESTDIR += ../
SOURCES = testutils.cpp
CONFIG += qtestlib
QT += sql
INCLUDEPATH += ../../framework
LIBS += -L ../../components -L../../framework -L../../ext/qcodeedit -lxinxframework -lxinxcmp -lqcodeedit

include(../../ext/qmodeltest/modeltest.pri)


profiling {
    QMAKE_CXXFLAGS += --coverage
    QMAKE_LFLAGS += --coverage
}