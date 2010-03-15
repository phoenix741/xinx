include(../../project_mode.pri)
TEMPLATE = lib
VERSION = 1.0.0.1
CONFIG += plugin
QT += xml \
    xmlpatterns \
    sql
DESTDIR += ../
LIBS += -L../../components \
    -lxinxcmp \
    -L../../framework \
    -lxinxframework \
    -lxml2 \
    -lxslt
TARGET = generix
INCLUDEPATH += ../../framework \
    ../../components
unix:INCLUDEPATH += /usr/include/libxml2
RESOURCES = plugindefinition/generix.qrc
TRANSLATIONS += plugindefinition/translations/dictionary_fr.ts
HEADERS = config/generixsettings.h \
    config/selfgenerixsettings.h \
    plugindefinition/generix.h \
    docks/dictionary/dictionarydockwidgetimpl.h \
    docks/dictionary/dictionaryparser.h \
    pluginresolver/gce150fileresolver.h \
    configuration/configurationfile.h \
    config/customgeneriximpl.h \
    config/specifiquemodelindex.h \
    configuration/gceinterface.h \
    configuration/gceinterfacefactory.h \
    configuration/gceconfiguration.h \
    configuration/gceconfigurationdef.h \
    configuration/gceproperties.h \
    projectproperty/std/generixprojectpageform.h \
    projectproperty/std/generixderivationpage.h \
    projectproperty/std/generixderivationoptionspage.h \
    projectproperty/wizard/newgenerixinformationpageimpl.h \
    projectproperty/wizard/newgenerixderivation1page.h \
    projectproperty/wizard/newgenerixderivation2page.h \
    projectproperty/wizard/newgenerixderivation3page.h \
    projectproperty/generixproject.h \
    configuration/configurationfileedit.h \
    configuration/configurationmanager.h \
    docks/project/generixprojectdock.h \
    savedialog/derivationdialogimpl.h \
    parser/generixxsltparser.h \
    docks/dictionary/dictionarymodel.h
SOURCES = config/generixsettings.cpp \
    config/selfgenerixsettings.cpp \
    plugindefinition/generix.cpp \
    docks/dictionary/dictionarydockwidgetimpl.cpp \
    docks/dictionary/dictionaryparser.cpp \
    pluginresolver/gce150fileresolver.cpp \
    configuration/configurationfile.cpp \
    config/customgeneriximpl.cpp \
    config/specifiquemodelindex.cpp \
    configuration/gceinterface.cpp \
    configuration/gceinterfacefactory.cpp \
    configuration/gceconfiguration.cpp \
    configuration/gceconfigurationdef.cpp \
    configuration/gceproperties.cpp \
    projectproperty/std/generixprojectpageform.cpp \
    projectproperty/std/generixderivationpage.cpp \
    projectproperty/std/generixderivationoptionspage.cpp \
    projectproperty/wizard/newgenerixinformationpageimpl.cpp \
    projectproperty/wizard/newgenerixderivation1page.cpp \
    projectproperty/wizard/newgenerixderivation2page.cpp \
    projectproperty/wizard/newgenerixderivation3page.cpp \
    projectproperty/generixproject.cpp \
    configuration/configurationfileedit.cpp \
    configuration/configurationmanager.cpp \
    docks/project/generixprojectdock.cpp \
    savedialog/derivationdialogimpl.cpp \
    parser/generixxsltparser.cpp \
    docks/dictionary/dictionarymodel.cpp
win32:RC_FILE += rc/plugin.rc
FORMS += ui/dictionarydockwidget.ui \
    config/customgenerix.ui \
    projectproperty/std/generixprojectpageform.ui \
    projectproperty/std/generixderivationpage.ui \
    projectproperty/std/generixderivationoptionspage.ui \
    projectproperty/wizard/newgenerixinformationpage.ui \
    projectproperty/wizard/newgenerixderivation1page.ui \
    projectproperty/wizard/newgenerixderivation2page.ui \
    projectproperty/wizard/newgenerixderivation3page.ui \
    docks/project/gnxprojectdock.ui \
    savedialog/derivationdialog.ui
include(../../ext/qmodeltest/modeltest.pri)
