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
#include "dictionary.h"
#include <contentview/contentviewnode.h>
#include "dictionaryparser.h"
#include <project/xinxproject.h>

// Qt header
#include <QString>
#include <QVariant>
#include <QTranslator>
#include <QApplication>
#include <QDockWidget>
#include <QDir>

/* DictionaryPlugin */

DictionaryPlugin::DictionaryPlugin() : m_dock( 0 ) {
		Q_INIT_RESOURCE(dictionary);
}

DictionaryPlugin::~DictionaryPlugin() {
}

bool DictionaryPlugin::initializePlugin( const QString & lang ) {
	QTranslator * tranlator = new QTranslator( this );
	tranlator->load( QString(":/dictionary/translations/dictionary_%1").arg( lang ) );
	qApp->installTranslator(tranlator);

	return true;
}

QVariant DictionaryPlugin::getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr ) {
	switch( attr ) {
	case PLG_NAME:
		return tr("XINX Dictionary Plugin");
	case PLG_DESCRIPTION:
		return tr("This small plugin show a new dock with the dictionary of the application");
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
    case PLG_ICON:
		return QPixmap( ":/dictionary/images/dictionary.png" );
	case PLG_EMAIL:
		return "ulrich.vdh@shadoware.org";
	case PLG_WEBSITE:
		return "http://www.shadoware.org";
	case PLG_VERSION:
		return "1.0";
	case PLG_LICENCE:
		return "GPL v2.0 or later";
	}
	return QVariant();
}

bool DictionaryPlugin::initializeProject( XinxProject * project ) {
	Q_ASSERT( m_dock );

	m_dock->loadDictionaryList( QDir( project->projectPath() ).absoluteFilePath( "configurationdef.xml" ) );

	return true;
}

bool DictionaryPlugin::destroyProject( XinxProject * project ) {
	Q_ASSERT( m_dock );

	m_dock->clearDictionaryList();

	return true;
}

QDockWidget * DictionaryPlugin::createDockWidget( QWidget * parent ) {
	if( ! m_dock ) {
		m_dock = new DictionaryDockWidgetImpl( parent );
		m_dock->setObjectName( QString::fromUtf8( "m_dictionaryDock" ) );
	}
	return m_dock;
}

Q_EXPORT_PLUGIN2(dictionaryplugin, DictionaryPlugin)
