TEMPLATE = app
TARGET = sqlconf
HEADERS += xmlconfigurationreader.h \
    sqlconf.h
SOURCES += xmlconfigurationreader.cpp \
    main.cpp \
    sqlconf.cpp
FORMS += sqlconf.ui
RESOURCES += 
QT += xml \
    sql
