/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -p studiointerface com.editor.xinx.xml
 *
 * dbusxml2cpp is Copyright (C) 2006 Trolltech ASA. All rights reserved.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#include "studiointerface.h"

/*
 * Implementation of interface class ComEditorXinxInterface
 */

ComEditorXinxInterface::ComEditorXinxInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

ComEditorXinxInterface::~ComEditorXinxInterface()
{
}

