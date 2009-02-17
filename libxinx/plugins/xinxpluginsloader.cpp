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

// Xinx header
#include "core/xinxcore.h"
#include "core/xinxconfig.h"
#include "core/xinxproject.h"
#include "plugins/xinxpluginsloader.h"
#include "editors/xinxformatscheme.h"
#include "editors/xinxlanguagefactory.h"

// Qt header
#include <QPluginLoader>
#include <QApplication>
#include <QDebug>

// QCodeEdit header
#include <qlanguagefactory.h>
#include <qnfadefinition.h>

/* Static member */

XinxPluginsLoader * XinxPluginsLoader::s_self = 0;

/* Static function */

static bool pluginsLessThan( XinxPluginElement * s1, XinxPluginElement * s2 ) {
	if( s1->isStatic() && (!s2->isStatic()) ) return true;
	if( (!s1->isStatic()) && s2->isStatic() ) return false;
	return s1->name() < s2->name();
}

/* XinxPluginsLoader */

XinxPluginsLoader::XinxPluginsLoader() {

}

XinxPluginsLoader::~XinxPluginsLoader() {
	qDeleteAll( plugins() );
	if( s_self == this )
		s_self = NULL;
}

XinxPluginsLoader * XinxPluginsLoader::self() {
	if( s_self == 0 ) {
		s_self = new XinxPluginsLoader();
		XINXStaticDeleter::self()->addObject( s_self );
	}
	return s_self;
}

QList<XinxPluginElement*> XinxPluginsLoader::plugins() const {
	QList<XinxPluginElement*> result = m_plugins.values();
	qSort( result.begin(), result.end(), pluginsLessThan );
	return result;
}

XinxPluginElement * XinxPluginsLoader::plugin( const QString & name ) {
	return m_plugins.value( name );
}

void XinxPluginsLoader::addPlugin( QObject * plugin, bool staticLoaded ) {
	/* Manage all XINX Plugin */
	IXinxPlugin * iXinxPlugin = qobject_cast<IXinxPlugin*>( plugin );
	if( ! iXinxPlugin ) return;

	QString name = plugin->metaObject()->className();

	// Initialize the plugin (if needed)
	if(! iXinxPlugin->initializePlugin( XINXConfig::self()->config().language ) ) {
		qCritical() << "Can't load " << name << " plugin.";
		return;
	}

	// Create a glue to use the plugin with XinxPluginsLoader
	XinxPluginElement * element = new XinxPluginElement( plugin, staticLoaded );

	// Set the status of the plugin with the configuration
	element->setActivated( XINXConfig::self()->config().plugins.value( name, false ) );
	m_plugins.insert( name, element );

	//! \todo move this line in setActivated method of the plugin
	// Create default tools given by the plugin
	QPair<QString,QString> tools;
	foreach( tools, iXinxPlugin->pluginTools() )
		XINXConfig::self()->addDefaultTool( tools.first, tools.second );

	// Create possible extention definition
	IFilePlugin * interface = qobject_cast<IFilePlugin*>( plugin );
	if( interface ) {
		foreach( IFileTypePlugin * t, interface->fileTypes() ) {
			struct AppSettings::struct_extentions properties = t->properties();
			XINXConfig::self()->addDefaultExtention( t->description(), properties );

			// If the plugin contains format and language description, we loaded it.
			IFileTextPlugin * textPlugin = dynamic_cast<IFileTextPlugin*>( t );
			if( textPlugin ) {
				// Format
				QFormatScheme * scheme = textPlugin->createFormatScheme( XINXConfig::self() );
				XINXConfig::self()->addFormatScheme( textPlugin->highlighterId(), qobject_cast<XinxFormatScheme*>( scheme ) );
				if( ! scheme ) {
					scheme = XINXConfig::self()->languageFactory()->defaultFormatScheme();
				}

				// Language
				QDomDocument doc;
				QLanguageFactory::LangData data;
				doc.setContent( textPlugin->createLanguageDescription() );
				QNFADefinition::load( doc, &data, scheme );
				XINXConfig::self()->languageFactory()->addLanguage( data );
			}
		}
	}
}

void XinxPluginsLoader::loadPlugins() {
	foreach( QObject * plugin, QPluginLoader::staticInstances() )
		addPlugin( plugin, true );

	m_pluginsDir = QDir( "plugins:" );

	foreach( const QString & fileName, m_pluginsDir.entryList( QDir::Files ) ) {
		QPluginLoader loader( m_pluginsDir.absoluteFilePath( fileName ) );
		QObject * plugin = loader.instance();
		if ( plugin )
			addPlugin( plugin );
		else
			qDebug() << loader.errorString();
	}
}

QList<IFileTypePlugin*> XinxPluginsLoader::fileTypes() const {
	QList<IFileTypePlugin*> result;

	foreach( XinxPluginElement * element, plugins() ) {
		IFilePlugin * interface = qobject_cast<IFilePlugin*>( element->plugin() );
		if( element->isActivated() && interface ) {
			result += interface->fileTypes();
		}
	}

	return result;
}

IFileTypePlugin * XinxPluginsLoader::matchedFileType( const QString & filename ) const {
	QList<IFileTypePlugin*> types = fileTypes();
	foreach( IFileTypePlugin* plugin, types ) {
		QStringList patterns = plugin->match().split( " " );
		foreach( const QString & match, patterns ) {
			QRegExp pattern( match, Qt::CaseInsensitive, QRegExp::Wildcard );
			if( pattern.exactMatch( filename ) )
				return plugin;
		}
	}
	return 0;
}

QString XinxPluginsLoader::allManagedFileFilter() const {
	QStringList result;
	QList<IFileTypePlugin*> types = fileTypes();
	foreach( IFileTypePlugin* plugin, types ) {
		result += plugin->match();
	}
	return tr( "All managed files (%1)" ).arg( result.join( " " ) );
}

QString XinxPluginsLoader::fileTypeFilter( IFileTypePlugin * fileType ) {
	return tr( "All %1 (%2)" ).arg( fileType->description() ).arg( fileType->match() );
}

QString XinxPluginsLoader::exampleOfHighlighter( const QString & name ) const {
	foreach( XinxPluginElement * element, plugins() ) {
		IFilePlugin * interface = qobject_cast<IFilePlugin*>( element->plugin() );
		if( element->isActivated() && interface ) {
			foreach( IFileTypePlugin * p, interface->fileTypes() ) {
				IFileTextPlugin * textFileType = dynamic_cast<IFileTextPlugin*>( p );
				if( textFileType && textFileType->highlighterId().toLower() == name.toLower() ) {
					return textFileType->fileExample();
				}
			}
		}
	}
	return QString();
}

XinxFormatScheme * XinxPluginsLoader::scheme( const QString & highlighter, XINXConfig * config ) {
	foreach( XinxPluginElement * element, plugins() ) {
		IFilePlugin * interface = qobject_cast<IFilePlugin*>( element->plugin() );
		if( element->isActivated() && interface ) {
			foreach( IFileTypePlugin * p, interface->fileTypes() ) {
				IFileTextPlugin * textFileType = dynamic_cast<IFileTextPlugin*>( p );
				if( textFileType && textFileType->highlighterId().toLower() == highlighter.toLower() ) {
					return textFileType->createFormatScheme( config );
				}
			}
		}
	}
	return 0;
}

QStringList XinxPluginsLoader::openDialogBoxFilters() const {
	QStringList result;
	QList<IFileTypePlugin*> types = fileTypes();
	foreach( IFileTypePlugin* plugin, types ) {
		result += fileTypeFilter( plugin );
	}
	result += allManagedFileFilter();
	return result;
}

QStringList XinxPluginsLoader::managedFilters() const {
	QStringList result;
	foreach( IFileTypePlugin* plugin, fileTypes() ) {
		result += plugin->match().split( " " );
	}
	return result;
}

FileContentElement * XinxPluginsLoader::createElement( QString & filename, FileContentElement * parent, int line ) {
	IFileTypePlugin * fileType = matchedFileType( filename );
	if( ! fileType ) return NULL;

	QStringList searchList;

	if( ! parent->filename().isEmpty() )
		searchList << QFileInfo( parent->filename() ).absolutePath();

	if( XINXProjectManager::self()->project() )
		searchList += XINXProjectManager::self()->project()->processedSearchPathList();

	QString absPath = QString();
	bool finded = false;
	foreach( const QString & path, searchList ) {
		absPath = QDir( path ).absoluteFilePath( filename );
		if( QFile::exists( absPath ) ) {
			finded = true;
			break;
		}
	}

	if( finded ) filename = absPath;

	FileContentElement * element = fileType->createElement( parent, line, filename );
	if( ! element ) return NULL;

	Q_ASSERT( dynamic_cast<FileContentParser*>( element ) );

	return element;
}

QList< QPair<QString,QString> > XinxPluginsLoader::revisionsControls() const {
	QList< QPair<QString,QString> > result;
	foreach( XinxPluginElement * element, plugins() ) {
		IRCSPlugin * interface = qobject_cast<IRCSPlugin*>( element->plugin() );
		if( element->isActivated() && interface ) {
			foreach( const QString & rcsKey, interface->rcs() ) {
				result << qMakePair( rcsKey, interface->descriptionOfRCS( rcsKey ) );
			}
		}
	}
	return result;
}

RCS * XinxPluginsLoader::createRevisionControl( QString revision, QString basePath ) const {
	RCS * rcs = NULL;
	foreach( XinxPluginElement * element, plugins() ) {
		IRCSPlugin * interface = qobject_cast<IRCSPlugin*>( element->plugin() );
		if( element->isActivated() && interface ) {
			rcs = interface->createRCS( revision, basePath );
			if( rcs ) break;
		}
	}
	return rcs;
}
