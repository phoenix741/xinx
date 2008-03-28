/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef CVSPLUGIN_H_
#define CVSPLUGIN_H_

// Xinx header
#include <plugininterfaces.h>
#include "pluginsettings.h"

class CVSPlugin : public QObject, public IXinxPlugin, public IXinxPluginConfiguration, public IRCSPlugin {
	Q_OBJECT
	Q_INTERFACES(IXinxPlugin)
	Q_INTERFACES(IXinxPluginConfiguration)
	Q_INTERFACES(IRCSPlugin)
public:
	CVSPlugin();
	virtual ~CVSPlugin();
	
	virtual bool initializePlugin( const QString & lang );
	virtual QVariant getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr );

	virtual QList< QPair<QString,QString> > pluginTools();

	virtual QWidget * createSettingsDialog();
	virtual bool loadSettingsDialog( QWidget * widget );
	virtual bool saveSettingsDialog( QWidget * widget );
	
	virtual QStringList rcs();
	virtual QString descriptionOfRCS( const QString & rcs );
	virtual RCS * createRCS( const QString & rcs, const QString & basePath );
private:
	PluginSettings * m_settings;
};

#endif /* CVSPLUGIN_H_*/
