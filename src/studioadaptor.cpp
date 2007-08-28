/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -a studioadaptor com.editor.xinx.xml
 *
 * dbusxml2cpp is Copyright (C) 2006 Trolltech ASA. All rights reserved.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "studioadaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class XinxAdaptor
 */

XinxAdaptor::XinxAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

XinxAdaptor::~XinxAdaptor()
{
    // destructor
}

void XinxAdaptor::callWebservices()
{
    // handle method call com.editor.xinx.callWebservices
    QMetaObject::invokeMethod(parent(), "callWebservices");
}

void XinxAdaptor::closeAllFile()
{
    // handle method call com.editor.xinx.closeAllFile
    QMetaObject::invokeMethod(parent(), "closeAllFile");
}

void XinxAdaptor::closeFile()
{
    // handle method call com.editor.xinx.closeFile
    QMetaObject::invokeMethod(parent(), "closeFile");
}

void XinxAdaptor::closeProjectNoSessionData()
{
    // handle method call com.editor.xinx.closeProjectNoSessionData
    QMetaObject::invokeMethod(parent(), "closeProjectNoSessionData");
}

void XinxAdaptor::closeProjectWithSessionData()
{
    // handle method call com.editor.xinx.closeProjectWithSessionData
    QMetaObject::invokeMethod(parent(), "closeProjectWithSessionData");
}

void XinxAdaptor::newDefaultFile()
{
    // handle method call com.editor.xinx.newDefaultFile
    QMetaObject::invokeMethod(parent(), "newDefaultFile");
}

void XinxAdaptor::newJavascriptFile()
{
    // handle method call com.editor.xinx.newJavascriptFile
    QMetaObject::invokeMethod(parent(), "newJavascriptFile");
}

void XinxAdaptor::newProject()
{
    // handle method call com.editor.xinx.newProject
    QMetaObject::invokeMethod(parent(), "newProject");
}

void XinxAdaptor::newStylesheetFile()
{
    // handle method call com.editor.xinx.newStylesheetFile
    QMetaObject::invokeMethod(parent(), "newStylesheetFile");
}

void XinxAdaptor::newTemplate()
{
    // handle method call com.editor.xinx.newTemplate
    QMetaObject::invokeMethod(parent(), "newTemplate");
}

void XinxAdaptor::newWebservicesFile()
{
    // handle method call com.editor.xinx.newWebservicesFile
    QMetaObject::invokeMethod(parent(), "newWebservicesFile");
}

void XinxAdaptor::newXmlDataFile()
{
    // handle method call com.editor.xinx.newXmlDataFile
    QMetaObject::invokeMethod(parent(), "newXmlDataFile");
}

void XinxAdaptor::openFile(const QString &filename)
{
    // handle method call com.editor.xinx.openFile
    QMetaObject::invokeMethod(parent(), "openFile", Q_ARG(QString, filename));
}

void XinxAdaptor::openProject(const QString &filename)
{
    // handle method call com.editor.xinx.openProject
    QMetaObject::invokeMethod(parent(), "openProject", Q_ARG(QString, filename));
}

void XinxAdaptor::saveAllFile()
{
    // handle method call com.editor.xinx.saveAllFile
    QMetaObject::invokeMethod(parent(), "saveAllFile");
}

void XinxAdaptor::saveFileAs(const QString &filename)
{
    // handle method call com.editor.xinx.saveFileAs
    QMetaObject::invokeMethod(parent(), "saveFileAs", Q_ARG(QString, filename));
}

void XinxAdaptor::updateWebServicesList()
{
    // handle method call com.editor.xinx.updateWebServicesList
    QMetaObject::invokeMethod(parent(), "updateWebServicesList");
}

