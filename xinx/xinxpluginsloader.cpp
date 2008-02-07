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
	
void XinxPluginsLoader::addPlugin( QString extention, QObject * plugin ) {
	QString subplugin;
	
	IPluginSyntaxHighlighter * iSyntaxHighlighter = qobject_cast<IPluginSyntaxHighlighter*>( plugin );
	if( iSyntaxHighlighter ) {
		subplugin = iSyntaxHighlighter->highlighterOfExtention( extention ).toUpper();
		if( ! subplugin.isEmpty() ) {
			m_syntaxPlugins.insert( extention.toLower(), qMakePair( iSyntaxHighlighter, subplugin ) );
		}
	}

	IPluginPrettyPrint * iPrettyPrinter = qobject_cast<IPluginPrettyPrint*>( plugin );
	if( iPrettyPrinter ) {
		subplugin = iPrettyPrinter->prettyPrinterOfExtention( extention ).toUpper();
		if( ! subplugin.isEmpty() ) {
			m_prettyPlugins.insert( extention.toLower(), qMakePair( iPrettyPrinter, subplugin ) );
		}
	}

	IPluginExtendedEditor * iExtendedEditor = qobject_cast<IPluginExtendedEditor*>( plugin );
	if( iExtendedEditor ) {
		subplugin = iExtendedEditor->extendedEditorOfExtention( extention ).toUpper();
		if( ! subplugin.isEmpty() ) {
			m_extendedEditorPlugins.insert( extention.toLower(), qMakePair( iExtendedEditor, subplugin ) );
		}
	}
}

void XinxPluginsLoader::addPlugin( QObject * plugin ) {
	IPlugin * iPlugin = qobject_cast<IPlugin*>( plugin );
	if( iPlugin ) {
		QHash<QString,QString> libelles = iPlugin->extentionsDescription();
		
		foreach( QString extentions, iPlugin->extentions() ) {
			QString libelle = libelles[ extentions.section( ' ', 0, 0 ) ];
			QStringList suffixes = extentions.split( ' ' );
			
			if( suffixes.count() > 0 ) {
				m_libelles.insert( suffixes.at( 0 ), libelle );
			}
			
			foreach( QString suffix, suffixes ) {
				m_filterIndex[ suffix ] = m_filters.count();
				m_icons[ suffix ] = iPlugin->icon( suffix );

				addPlugin( suffix, plugin );
			}
			
			suffixes = suffixes.replaceInStrings( QRegExp( "^" ), "*." );
			m_defaultProjectFilter << suffixes;
			
			m_filters.append( QApplication::translate( "XINXConfig", "All %1 (%2)" ).arg( libelle ).arg( suffixes.join( " " ) ) );			
		}
		m_filters.insert( 0, QApplication::translate( "XINXConfig", "All managed file") + " (" + m_defaultProjectFilter.join( " " ) + ")" );
	}

	IPluginSyntaxHighlighter * iSyntaxHighlighter = qobject_cast<IPluginSyntaxHighlighter*>( plugin );
	if( iSyntaxHighlighter ) {
		foreach( QString p, iSyntaxHighlighter->highlighters() )
			m_directSyntaxPlugins.insert( p.toUpper(), iSyntaxHighlighter );
	}

	IPluginPrettyPrint * iPrettyPrinter = qobject_cast<IPluginPrettyPrint*>( plugin );
	if( iPrettyPrinter ) {
		foreach( QString p, iPrettyPrinter->prettyPrinters() )
			m_directPrettyPlugins.insert( p.toUpper(), iPrettyPrinter );
	}

	IPluginExtendedEditor * iExtendedEditor = qobject_cast<IPluginExtendedEditor*>( plugin );
	if( iExtendedEditor ) {
		foreach( QString p, iExtendedEditor->extendedEditors() )
			m_directExtendedEditorPlugins.insert( p.toUpper(), iExtendedEditor );
	}}

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

QIcon XinxPluginsLoader::iconOfSuffix( const QString & suffix ) const {
	return m_icons.value( suffix );
}

const QHash<QString,QString> & XinxPluginsLoader::extentions() const {
	return m_libelles;
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

QStringList XinxPluginsLoader::defaultProjectFilter( const QString & name ) const {
	QStringList result;
	foreach( QString t, m_defaultProjectFilter )
		result << t.replace( "*", QString( "*%1*" ).arg( name ) );
	return result;
}

QPair<IPluginSyntaxHighlighter*,QString> XinxPluginsLoader::highlighterOfSuffix( const QString & suffix ) const {
	return m_syntaxPlugins.value( suffix.toLower() );
}

QPair<IPluginPrettyPrint*,QString> XinxPluginsLoader::prettyPrinterOfSuffix( const QString & suffix ) const {
	return m_prettyPlugins.value( suffix.toLower() );
}

QPair<IPluginExtendedEditor*,QString> XinxPluginsLoader::extendedEditorOfSuffix( const QString & suffix ) const {
	return m_extendedEditorPlugins.value( suffix.toLower() );
}

QStringList XinxPluginsLoader::highlighterOfPlugins() const {
	return m_directSyntaxPlugins.keys();
}

QStringList XinxPluginsLoader::prettyPrinterOfPlugins() const {
	return m_directPrettyPlugins.keys();
}

QStringList XinxPluginsLoader::extendedEditorOfPlugins() const {
	return m_directExtendedEditorPlugins.keys();
}

IPluginSyntaxHighlighter* XinxPluginsLoader::highlighterOfPlugin( const QString & suffix ) const {
	return m_directSyntaxPlugins.value( suffix.toUpper() );
}

IPluginPrettyPrint* XinxPluginsLoader::prettyPrinterOfPlugin( const QString & suffix ) const {
	return m_directPrettyPlugins.value( suffix.toUpper() );
}

IPluginExtendedEditor* XinxPluginsLoader::extendedEditorOfPlugin( const QString & suffix ) const {
	return m_directExtendedEditorPlugins.value( suffix.toUpper() );
}
