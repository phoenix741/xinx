/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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

// Xinx header
#include "xinxpluginsloader.h"

// Qt header
#include <QPluginLoader>
#include <QApplication>

/* XinxPluginsLoader */

XinxPluginsLoader::XinxPluginsLoader() {
	
}

XinxPluginsLoader::~XinxPluginsLoader() {
	
}

const QDir & XinxPluginsLoader::pluginsDir() const {
	return m_pluginsDir;
}

const QStringList & XinxPluginsLoader::pluginFileNames() const {
	return m_pluginFileNames;
}
	
void XinxPluginsLoader::addPlugin( QObject * plugin ) {
	IPluginSyntaxHighlighter * iSyntaxHighlighter = qobject_cast<IPluginSyntaxHighlighter*>( plugin );
	if( iSyntaxHighlighter )
		m_syntaxPlugins.append( iSyntaxHighlighter );
}

void XinxPluginsLoader::loadPlugins() {
	foreach( QObject * plugin, QPluginLoader::staticInstances() )
		addPlugin( plugin );
	
	m_pluginsDir = QDir( qApp->applicationDirPath() );
	m_pluginsDir.cd( "plugins" );
	
    foreach( QString fileName, m_pluginsDir.entryList( QDir::Files ) ) {
        QPluginLoader loader( m_pluginsDir.absoluteFilePath( fileName ) );
        QObject * plugin = loader.instance();
        if ( plugin ) {
        	addPlugin(plugin);
        	m_pluginFileNames += fileName;
        }
    }
}

const QList<IPluginSyntaxHighlighter*> & XinxPluginsLoader::syntaxPlugins() const {
	return m_syntaxPlugins;
}

QString XinxPluginsLoader::highlighterOfSuffix( const QString & suffix ) {
	foreach( IPluginSyntaxHighlighter* interface, m_syntaxPlugins ) {
		QString h = interface->highlighterOfExtention( suffix );
		if( ! h.isEmpty() )
			return h;
	}
	return QString();
}

QString XinxPluginsLoader::filter( const QString & suffix ) {
	foreach( IPluginSyntaxHighlighter* interface, m_syntaxPlugins ) {
		QString libelle = interface->filters()[ suffix ];
		if( ! libelle.isEmpty() )
			return QString( "%1 (*.%2)" ).arg( libelle ).arg( suffix );
	}
	return QString();
}

QStringList XinxPluginsLoader::filters() {
	QStringList result;
	QStringList allExtentions;
	
	foreach( IPluginSyntaxHighlighter* interface, m_syntaxPlugins ) {
		foreach( QString suffix, interface->filters().keys() ) {
			QString libelle = interface->filters()[ suffix ];
			result << QString( "%1 (*.%2)" ).arg( libelle ).arg( suffix );
			allExtentions << QString( "*.%1" ).arg( suffix );
		}
	}
	result.insert( 0, QApplication::translate( "XINXConfig", "All managed file") + " (" + allExtentions.join( " " ) + ")" );
	
	return result;
}

