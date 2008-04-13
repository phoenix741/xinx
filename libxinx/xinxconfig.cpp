/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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
 
// Qt header
#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QFileDialog>

// Xinx header
#include "xinxconfig.h"
#include "xinxcore.h"
#include "xinxpluginsloader.h"

/* ToolsNotDefinedException */

ToolsNotDefinedException::ToolsNotDefinedException( const QString & tool ) : XinxException( QString( "Tool %1 not correctly defined." ).arg( tool ) ) {
	
}

/* Static member */

XINXConfig * XINXConfig::s_self = 0;

/* PrivateXINXConfig */

class PrivateXINXConfig {
public:
	PrivateXINXConfig( XINXConfig * parent );
	~PrivateXINXConfig();
private:
	XINXConfig * m_parent;
};

PrivateXINXConfig::PrivateXINXConfig( XINXConfig * parent ) : m_parent( parent ) {

}

PrivateXINXConfig::~PrivateXINXConfig() {
	
}


/* XINXConfig */

XINXConfig::XINXConfig( const XINXConfig & origine ) : QObject(), AppSettings( origine ) {
	d = new PrivateXINXConfig( this );
}

XINXConfig::XINXConfig() : AppSettings() {
	d = new PrivateXINXConfig( this );
}

XINXConfig::~XINXConfig() {
	if( this == s_self )
		s_self = NULL;
	delete d;
}

XINXConfig& XINXConfig::operator=(const XINXConfig& p) {
	AppSettings::operator=( p );
	return *this;
}

void XINXConfig::load() {
	AppSettings::load();
	
	setXinxDataFiles( config().descriptions.datas );
}

void XINXConfig::save() {
	AppSettings::save();
	
	emit changed();
}

QString XINXConfig::getTools( const QString & tool, bool showDialog, QWidget * parentWindow ) {
	QString toolsPath = config().tools.value( tool );
	if( toolsPath.isEmpty() || (! QFile::exists( toolsPath ) ) ) {
		if( ! showDialog )
			throw ToolsNotDefinedException( toolsPath );

		toolsPath = QFileDialog::getOpenFileName( parentWindow, tr("Select the %1 tool").arg( tool ), toolsPath );
		if( (!toolsPath.isEmpty()) && QFile::exists( toolsPath ) )
			config().tools[ tool ] = toolsPath;
		else
			throw ToolsNotDefinedException( toolsPath );
	}
	return toolsPath;
}

void XINXConfig::addDefaultTool( const QString & tool, const QString & defaultValue ) {
	if( config().tools.value( tool ).isEmpty() ) {
		config().tools[ tool ] = defaultValue;
		// Store imediately the hashtable 
		QSettings settings("Shadoware.Org", "XINX"); 
		setSettingsHash_QString( &settings, "Tools", config().tools );
	}
}

void XINXConfig::setXinxDataFiles( const QString & path ) {
	config().descriptions.datas = path;
	QDir::setSearchPaths( "datas", QStringList() << path );
}

XINXConfig * XINXConfig::self() {
	if( s_self == 0 ) {
		s_self = new XINXConfig();
		XINXStaticDeleter::self()->addObject( s_self );
	}
	return s_self;
}
	
AppSettings::struct_globals XINXConfig::getDefaultGlobals() {
	struct_globals value = AppSettings::getDefaultGlobals();
	
	foreach( QString highlighter, XinxPluginsLoader::self()->highlighters() ) {
		const QHash<QString,QTextCharFormat> formats = XinxPluginsLoader::self()->formatOfHighlighter( highlighter );
		foreach( QString key, formats.keys() )
			value.formats[ key ] = formats.value( key );
	}
	
	return value;
}

AppSettings::struct_editor XINXConfig::getDefaultEditor() {
	struct_editor value = AppSettings::getDefaultEditor();

	value.defaultFormat.setFamily( "Monospace" );
	value.defaultFormat.setFixedPitch( true );
	value.defaultFormat.setPointSize( 8 );

	return value;
}


