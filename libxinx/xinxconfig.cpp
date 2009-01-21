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

// Qt header
#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QFileDialog>

// Xinx header
#include "xinxconfig.h"
#include "xinxcore.h"
#include "xinxpluginsloader.h"
#include "xinxformatfactory.h"
#include "xinxlanguagefactory.h"

/* ToolsNotDefinedException */

ToolsNotDefinedException::ToolsNotDefinedException( const QString & tool ) : XinxException( QString( "Tool %1 not correctly defined." ).arg( tool ) ) {

}

/* Static member */

XINXConfig * XINXConfig::s_self = 0;

/* XINXConfig */

XINXConfig::XINXConfig( const XINXConfig & origine ) : QObject(), AppSettings( origine ), m_formats( 0 ), m_languages( 0 ) {

}

XINXConfig::XINXConfig() : QObject(), AppSettings(), m_formats( 0 ), m_languages( 0 ) {

}

XINXConfig::~XINXConfig() {
	if( this == s_self )
		s_self = NULL;

}

XINXConfig& XINXConfig::operator=(const XINXConfig& p) {
	AppSettings::operator=( p );
	return *this;
}

void XINXConfig::load() {
	AppSettings::load();
	if( m_formats ) m_formats->updateFormats();
	setXinxDataFiles( config().descriptions.datas );
}

void XINXConfig::save() {
	AppSettings::save();

	emit changed();
}

XinxFormatFactory * XINXConfig::formatFactory() {
	if( ! m_formats ) {
		m_formats = new XinxFormatFactory( this );
		connect( this, SIGNAL(changed()), m_formats, SLOT(updateFormats()) );
	}
	return m_formats;
}

XinxLanguageFactory * XINXConfig::languageFactory() {
	if( ! m_languages ) {
		m_languages = new XinxLanguageFactory( formatFactory(), this );
	}
	return m_languages;
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
		AppSettingsSettings settings("Shadoware.Org", "XINX");
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

AppSettings::struct_extentions XINXConfig::matchedFileType( const QString & filename ) {
	IFileTypePlugin * plugin = XinxPluginsLoader::self()->matchedFileType( filename );
	return config().files.value( plugin->description() );
}

AppSettings::struct_globals XINXConfig::getDefaultGlobals() {
	struct_globals value = AppSettings::getDefaultGlobals();

	/*
	foreach( IFileTypePlugin* fileType, XinxPluginsLoader::self()->fileTypes() ) {
		IFileTextPlugin * textFileType = dynamic_cast<IFileTextPlugin*>( fileType );
		if( textFileType ) {
			const QHash<QString,QTextCharFormat> formats = textFileType->defaultsFormat();
			foreach( QString key, formats.keys() )
				value.formats[ key ] = formats.value( key );
		}
	}

	foreach( IFileTypePlugin * plugin , XinxPluginsLoader::self()->fileTypes() ) {
		value.files[ plugin->description() ].canBeSpecifique = plugin->properties().canBeSaveAsSpecifique;
		value.files[ plugin->description() ].customPath = plugin->properties().specifiqueSubDirectory;
	}
	*/
	foreach( IFileTypePlugin * plugin , XinxPluginsLoader::self()->fileTypes() ) {
		value.files[ plugin->description() ] = plugin->properties();
	}

#ifndef Q_WS_WIN
	value.xinxTrace = "/tmp/xinx_trace.html";
#else
	value.xinxTrace = "c:\\xinx_trace.html";
#endif

	return value;
}

AppSettings::struct_editor XINXConfig::getDefaultEditor() {
	struct_editor value = AppSettings::getDefaultEditor();

	value.defaultFormat.setFamily( "Monospace" );
	value.defaultFormat.setFixedPitch( true );
	value.defaultFormat.setPointSize( 8 );

	return value;
}


