include(../../project_mode.pri)

DESTDIR += ./
SOURCES = testcontentview.cpp
CONFIG  += qtestlib
INCLUDEPATH += ../../framework
LIBS = -L ../../components -L../../framework -L../../ext/qcodeedit -lxinxframework -lxinxcmp -lqcodeedit
