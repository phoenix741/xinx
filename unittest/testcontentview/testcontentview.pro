DESTDIR += ./
SOURCES = testcontentview.cpp
CONFIG  += qtestlib
INCLUDEPATH += ../../libxinx
LIBS = -L ../../components -L../../libxinx -L../../ext/qcodeedit -lsharedxinx -lxinxcmp -lqcodeedit
include(../../project_mode.pro)
