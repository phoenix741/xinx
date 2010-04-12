include(../../project_mode.pri)

DESTDIR += ../bin/
SOURCES = testgenerix.cpp \
	../../plugins/generix/docks/dictionary/dictionaryparser.cpp
HEADER =  ../../plugins/generix/docks/dictionary/dictionaryparser.h
CONFIG  += qtestlib
QT += sql xml xmlpatterns webkit
INCLUDEPATH += ../../components ../../framework ../../plugins/core/ ../../plugins/generix/docks/dictionary/
LIBS += -L ../../components -L../../plugins -L../../framework -L../../ext/qcodeedit
LIBS += -lcoreplugin -lxinxframework -lxinxcmp -lqcodeedit  

profiling {
  QMAKE_CXXFLAGS += --coverage
  QMAKE_LFLAGS   += --coverage
}


include(../../ext/qmodeltest/modeltest.pri)
