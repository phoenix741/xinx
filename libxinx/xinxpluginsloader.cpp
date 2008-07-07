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
#include "xinxcore.h"
#include "xinxconfig.h"
#include "xslproject.h"

// Qt header
#include <QPluginLoader>
#include <QApplication>
#include <QDebug>

/* Static member */

XinxPluginsLoader * XinxPluginsLoader::s_self = 0;

/* XinxPluginsLoader */

XinxPluginsLoader::XinxPluginsLoader() {

}

XinxPluginsLoader::~XinxPluginsLoader() {
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

const QList<XinxPluginElement> & XinxPluginsLoader::plugins() const {
	return m_plugins;
}

void XinxPluginsLoader::addPlugin( QObject * plugin, bool staticLoaded ) {
	/* Manage all XINX Plugin */
	IXinxPlugin * iXinxPlugin = qobject_cast<IXinxPlugin*>( plugin );
	if( ! iXinxPlugin ) return;

	if(! iXinxPlugin->initializePlugin( XINXConfig::self()->config().language ) ) {
		qCritical() << "Can't load " << plugin->metaObject()->className() << " plugin.";
		return;
	}

	struct XinxPluginElement element = { staticLoaded, true, plugin };
	m_plugins.append( element );

	QPair<QString,QString> tools;
	foreach( tools, iXinxPlugin->pluginTools() )
		XINXConfig::self()->addDefaultTool( tools.first, tools.second );
}

void XinxPluginsLoader::loadPlugins() {
	foreach( QObject * plugin, QPluginLoader::staticInstances() )
		addPlugin( plugin, true );

	m_pluginsDir = QDir( qApp->applicationDirPath() );
	m_pluginsDir.cd( "../plugins" );
    foreach( QString fileName, m_pluginsDir.entryList( QDir::Files ) ) {
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

	foreach( XinxPluginElement element, m_plugins ) {
		IFilePlugin * interface = qobject_cast<IFilePlugin*>( element.plugin );
		if( element.isActivated && interface ) {
			result += interface->fileTypes();
		}
	}

	return result;
}

IFileTypePlugin * XinxPluginsLoader::matchedFileType( const QString & filename ) const {
	QList<IFileTypePlugin*> types = fileTypes();
	foreach( IFileTypePlugin* plugin, types ) {
		QStringList patterns = plugin->match().split( " " );
		foreach( QString match, patterns ) {
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
	foreach( QString path, searchList ) {
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
	foreach( XinxPluginElement element, m_plugins ) {
		IRCSPlugin * interface = qobject_cast<IRCSPlugin*>( element.plugin );
		if( element.isActivated && interface ) {
			foreach( QString rcsKey, interface->rcs() ) {
				result << qMakePair( rcsKey, interface->descriptionOfRCS( rcsKey ) );
			}
		}
	}
	return result;
}

RCS * XinxPluginsLoader::createRevisionControl( QString revision, QString basePath ) const {
	RCS * rcs = NULL;
	foreach( XinxPluginElement element, m_plugins ) {
		IRCSPlugin * interface = qobject_cast<IRCSPlugin*>( element.plugin );
		if( element.isActivated && interface ) {
			rcs = interface->createRCS( revision, basePath );
			if( rcs ) break;
		}
	}
	return rcs;
}

QStringList XinxPluginsLoader::highlighters() const {
	QStringList result;
	foreach( XinxPluginElement element, m_plugins ) {
		IPluginSyntaxHighlighter * interface = qobject_cast<IPluginSyntaxHighlighter*>( element.plugin );
		if( element.isActivated && interface )
			result += interface->highlighters();
	}
	return result;
}

QString XinxPluginsLoader::highlighterOfSuffix( const QString & suffix ) const {
	foreach( XinxPluginElement element, m_plugins ) {
		IPluginSyntaxHighlighter * interface = qobject_cast<IPluginSyntaxHighlighter*>( element.plugin );
		if( element.isActivated && interface ) {
			QString value = interface->highlighterOfExtention( suffix );
			if( ! value.isEmpty() )
				return value;
		}
	}
	return QString();
}

SyntaxHighlighter * XinxPluginsLoader::createHighlighter( const QString & highlighter, QTextDocument* parent, XINXConfig * config ) {
	SyntaxHighlighter * h = NULL;
	foreach( XinxPluginElement element, m_plugins ) {
		IPluginSyntaxHighlighter * interface = qobject_cast<IPluginSyntaxHighlighter*>( element.plugin );
		if( element.isActivated && interface ) {
			h = interface->createHighlighter( highlighter, parent, config );
			if( h ) break;
		}
	}
	return h;
}

QHash<QString,QTextCharFormat> XinxPluginsLoader::formatOfHighlighter( const QString & highlighter ) {
	foreach( XinxPluginElement element, m_plugins ) {
		IPluginSyntaxHighlighter * interface = qobject_cast<IPluginSyntaxHighlighter*>( element.plugin );
		if( element.isActivated && interface && interface->highlighters().contains( highlighter, Qt::CaseInsensitive ) )
			return interface->formatOfHighlighter( highlighter );
	}
	return QHash<QString,QTextCharFormat>();
}

QString XinxPluginsLoader::exampleOfHighlighter( const QString & highlighter ) {
	foreach( XinxPluginElement element, m_plugins ) {
		IPluginSyntaxHighlighter * interface = qobject_cast<IPluginSyntaxHighlighter*>( element.plugin );
		if( element.isActivated && interface ) {
			QString value = interface->exampleOfHighlighter( highlighter );
			if( ! value.isEmpty() )
				return value;
		}
	}
	return QString();
}