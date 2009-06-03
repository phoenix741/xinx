include(../../project_mode.pri)

DESTDIR += ./
SOURCES = testxinxcodeedit.cpp
CONFIG  += qtestlib
INCLUDEPATH += ../../libxinx ../../ext/qcodeedit/lib ../../ext/qcodeedit/lib/document ../../ext/qcodeedit/lib/language
LIBS = -L ../../components -L../../libxinx -L../../ext/qcodeedit -lsharedxinx0 -lxinxcmp0 -lqcodeedit
