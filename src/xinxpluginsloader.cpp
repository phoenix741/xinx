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
#include <QDebug>

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
	IPlugin * iPlugin = qobject_cast<IPlugin*>( plugin );
	if( iPlugin ) {
		QHash<QString,QString> libelles = iPlugin->extentionsDescription();
		
		foreach( QString extentions, iPlugin->extentions() ) {
			QString libelle = libelles[ extentions.section( ' ', 0, 0 ) ];
			QStringList suffixes = extentions.split( ' ' );
			foreach( QString suffix, suffixes ) {
				m_filterIndex[ suffix ] = m_filters.count();
				m_icons[ suffix ] = iPlugin->icon( suffix );
			}
			
			suffixes = suffixes.replaceInStrings( QRegExp( "^" ), "*." );
			m_defaultProjectFilter << suffixes;
			
			m_filters.append( QString( "%1 (%2)" ).arg( libelle ).arg( suffixes.join( " " ) ) );			
		}
		m_filters.insert( 0, QApplication::translate( "XINXConfig", "All managed file") + " (" + m_defaultProjectFilter.join( " " ) + ")" );
	}
	
	IPluginSyntaxHighlighter * iSyntaxHighlighter = qobject_cast<IPluginSyntaxHighlighter*>( plugin );
	if( iSyntaxHighlighter )
		m_syntaxPlugins.append( iSyntaxHighlighter );

	IPluginPrettyPrint * iPrettyPrinter = qobject_cast<IPluginPrettyPrint*>( plugin );
	if( iPrettyPrinter )
		m_prettyPlugins.append( iPrettyPrinter );

	IPluginExtendedEditor * iExtendedEditor = qobject_cast<IPluginExtendedEditor*>( plugin );
	if( iExtendedEditor )
		m_extendedEditorPlugins.append( iExtendedEditor );
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
        } else
        	qDebug() << loader.errorString();
    }
}

const QList<IPluginSyntaxHighlighter*> & XinxPluginsLoader::syntaxPlugins() const {
	return m_syntaxPlugins;
}

const QList<IPluginPrettyPrint*> & XinxPluginsLoader::prettyPlugins() const {
	return m_prettyPlugins;
}

const QList<IPluginExtendedEditor*> & XinxPluginsLoader::extendedEditorPlugins() const {
	return m_extendedEditorPlugins;
}

QString XinxPluginsLoader::highlighterOfSuffix( const QString & suffix ) const {
	foreach( IPluginSyntaxHighlighter* interface, m_syntaxPlugins ) {
		QString h = interface->highlighterOfExtention( suffix );
		if( ! h.isEmpty() )
			return h;
	}
	return QString();
}

QString XinxPluginsLoader::filter( const QString & suffix ) const {
	return m_filters[ m_filterIndex[ suffix ] + 1 ];
}

const QStringList & XinxPluginsLoader::filters() const {
	return m_filters;
}

const QStringList & XinxPluginsLoader::defaultProjectFilter() const {
	return m_defaultProjectFilter;
}

QIcon XinxPluginsLoader::iconOfSuffix( const QString & suffix ) const {
	return m_icons.value( suffix );
}
