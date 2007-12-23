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

// Xinx header
#include "xinxconfig.h"
#include "globals.h"
#include "xinxpluginsloader.h"

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

XINXConfig::XINXConfig( const XINXConfig & origine ) : AppSettings( origine ) {
	d = new PrivateXINXConfig( this );
}

XINXConfig::XINXConfig() : AppSettings() {
	d = new PrivateXINXConfig( this );
}

XINXConfig::~XINXConfig() {
	delete d;
}
	
struct_globals XINXConfig::getDefaultGlobals() {
	struct_globals value = AppSettings::getDefaultGlobals();

	foreach( IPluginSyntaxHighlighter * interface, global.m_pluginsLoader->syntaxPlugins() ) {
		foreach( QString highlighter, interface->plugins() ) {
			QHash<QString,QTextCharFormat> formats = interface->formatOfHighlighter( highlighter );
			foreach( QString key, formats.keys() )
				value.formats[ key ] = formats[ key ];
		}
	}
	
	return value;
}

struct_editor XINXConfig::getDefaultEditor() {
	struct_editor value = AppSettings::getDefaultEditor();

	value.defaultFormat.setFamily( "Monospace" );
	value.defaultFormat.setFixedPitch( true );
	value.defaultFormat.setPointSize( 8 );

	return value;
}


