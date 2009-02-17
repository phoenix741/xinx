DESTDIR += ./
SOURCES = testcontentview.cpp
CONFIG  += qtestlib
INCLUDEPATH += ../../libxinx ../../ext/qcodeedit/lib ../../ext/qcodeedit/lib/document ../../ext/qcodeedit/lib/language
LIBS = -L ../../components -L../../libxinx -L../../ext/qcodeedit -lsharedxinx -lxinxcmp -lqcodeedit
include(../../project_mode.pro)
