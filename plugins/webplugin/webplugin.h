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

#ifndef WEBPLUGIN_H_
#define WEBPLUGIN_H_

// Xinx header
#include <plugins/plugininterfaces.h>

class WebPluginSettings;

class WebPlugin : public QObject, public IFilePlugin, public IXinxPluginConfiguration {
	Q_OBJECT
	Q_INTERFACES(IXinxPlugin)
	Q_INTERFACES(IXinxPluginConfiguration)
	Q_INTERFACES(IFilePlugin)
public:
	WebPlugin();
	virtual ~WebPlugin();

	virtual bool initializePlugin( const QString & lang );
	virtual QVariant getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr );

	virtual QList<IFileTypePlugin*> fileTypes();

	virtual QWidget * createSettingsDialog();
	virtual bool loadSettingsDialog( QWidget * widget );
	virtual bool saveSettingsDialog( QWidget * widget );
private:
	QList<IFileTypePlugin*> m_fileTypes;
};

#endif /* WEBPLUGIN_H_*/
