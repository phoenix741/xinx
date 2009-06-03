include(../../project_mode.pri)

DESTDIR += ./
INCLUDEPATH += ../../libxinx ../../ext/qcodeedit/lib ../../ext/qcodeedit/lib/document
LIBS = -L ../../components -L../../libxinx -L../../ext/qcodeedit -lsharedxinx0 -lxinxcmp0 -lqcodeedit
FORMS += codedialog.ui

HEADERS += codedialogimpl.h

SOURCES += codedialogimpl.cpp \
 guixinxcodeedit.cpp

RESOURCES += application.qrc

