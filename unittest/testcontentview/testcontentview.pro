include(../../project_mode.pri)

DESTDIR += ./
SOURCES = testcontentview.cpp
CONFIG  += qtestlib
INCLUDEPATH += ../../libxinx
LIBS = -L ../../components -L../../libxinx -L../../ext/qcodeedit -lsharedxinx0 -lxinxcmp0 -lqcodeedit
