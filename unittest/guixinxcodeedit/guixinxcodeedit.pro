include(../../project_mode.pri)

DESTDIR += ../bin/
INCLUDEPATH += ../../framework ../../ext/qcodeedit/lib ../../ext/qcodeedit/lib/document
LIBS = -L ../../components -L../../framework -L../../ext/qcodeedit -lxinxframework -lxinxcmp -lqcodeedit
FORMS += codedialog.ui

HEADERS += codedialogimpl.h

SOURCES += codedialogimpl.cpp \
 guixinxcodeedit.cpp

RESOURCES += application.qrc


profiling {
  QMAKE_CXXFLAGS += --coverage
  QMAKE_LFLAGS   += --coverage
}