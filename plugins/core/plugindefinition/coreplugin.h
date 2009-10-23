/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

#ifndef COREPLUGIN_H_
#define COREPLUGIN_H_
#pragma once

// Xinx header
#include <plugins/plugininterfaces.h>
#include <actions/actioninterface.h>
#include "docks/datastream/xmlpresentationdockwidget.h"

class WebPluginSettings;

class CorePlugin : public QObject, public IFilePlugin, public IXinxPluginConfiguration, public IXinxPluginProjectConfiguration, public IDockPlugin {
	Q_OBJECT
	Q_INTERFACES(IXinxPlugin)
	Q_INTERFACES(IXinxPluginConfiguration)
	Q_INTERFACES(IXinxPluginProjectConfiguration)
	Q_INTERFACES(IFilePlugin)
    Q_INTERFACES(IDockPlugin)
public:
	CorePlugin();
	virtual ~CorePlugin();

	virtual bool initializePlugin( const QString & lang );
	virtual QVariant getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr );

	virtual QList<IFileTypePlugin*> fileTypes();

	virtual XinxAction::MenuList actions();

	virtual QList<IXinxPluginConfigurationPage*> createSettingsDialog( QWidget * parent );

	virtual QWidget * createProjectSettingsPage();
	virtual bool loadProjectSettingsPage( QWidget * widget );
	virtual bool saveProjectSettingsPage( QWidget * widget );

	virtual QList<QWizardPage*> createNewProjectSettingsPages( int nextid );
	virtual bool saveNewProjectSettingsPage( XinxProject * project, QWizardPage * page );

	virtual QList<QDockWidget*> createDocksWidget( QWidget * parent );

    XmlPresentationDockWidget * dock();
private:
	QList<IFileTypePlugin*> m_fileTypes;
	XinxAction::MenuList m_menus;

    XmlPresentationDockWidget * m_dock;
};

#endif /* COREPLUGIN_H_*/
