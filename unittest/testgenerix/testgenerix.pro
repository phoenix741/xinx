include(../../project_mode.pri)

DESTDIR += ../bin/
SOURCES = testgenerix.cpp \
	../../plugins/generix/docks/dictionary/dictionaryparser.cpp \
	../../plugins/generix/configuration/gceconfigurationxmlparser.cpp \
	../../plugins/generix/configuration/gceconfigurationxmlparser2.cpp \
	../../plugins/generix/configuration/configurationfile.cpp
HEADERS =  ../../plugins/generix/docks/dictionary/dictionaryparser.h \
	../../plugins/generix/configuration/gceconfigurationxmlparser.h \
	../../plugins/generix/configuration/gceconfigurationxmlparser2.h \
	../../plugins/generix/configuration/configurationfile.h
CONFIG  += qtestlib
QT += sql xml xmlpatterns webkit
INCLUDEPATH += ../../components ../../framework ../../plugins/core/ ../../plugins/generix/docks/dictionary/ ../../plugins/generix/ ../../plugins/generix/configuration/
LIBS += -L ../../components -L../../plugins -L../../framework -L../../ext/qcodeedit
LIBS += -lcoreplugin -lxinxframework -lxinxcmp -lqcodeedit  
win32:LIBS += -lwsock32 \
	-llibxml2 \
	-llibxslt
unix:LIBS += -lxml2 \
	-lxslt

profiling {
  QMAKE_CXXFLAGS += --coverage
  QMAKE_LFLAGS   += --coverage
}


include(../../ext/qmodeltest/modeltest.pri)
