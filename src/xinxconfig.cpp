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
 
#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QSettings>

#include "xinxconfig.h"
#include "jshighlighter.h"
#include "xmlhighlighter.h"

//

// #define OPEN_SAVE_DIALOG_FILTER "All Managed File (*.xml;*.xsl;*.js;*.fws);;All XML File (*.xml;*.xsl);;All XSL File (*.xsl);;All JS File (*.js);;All WebServices XINX File (*.fws)"

XINXConfig * xinxConfig = NULL;

XINXConfig::XINXConfig(  ) {
	m_settings = new QSettings("Generix", "XINX");

	m_lang = QLocale::system().name();
	m_createBackupFile = true;
	m_alertWhenStdFile = true;
	m_xinxProjectPath = QDir( qApp->applicationDirPath() ).absoluteFilePath( "project" );
	m_objectDescriptionPath = QDir( qApp->applicationDirPath() ).absoluteFilePath( "xml" );
	
	m_xinxPosition = QPoint(200, 200);
	m_xinxSize = QSize(400, 400);
	m_docks = contents | services;
	
	struct managedStructure structure;
	structure.example = QObject::tr(
								"<library name=\"xinx\">"
								"	<book name=\"noname\">This is a description</book>"
								"	<!--This is a comment ;) -->"
								"	<<book name=\"with error\">"
								"</library>"
								);
	structure.color.clear();
	XmlHighlighter * xmlh = new XmlHighlighter( );
	foreach( QString key, xmlh->formats().keys() )
		structure.color[ key ] = xmlh->formats()[ key ];
	delete xmlh;
	m_managedStrucureList["xml"] = structure;
	
	structure.example = QObject::tr(
								"function xxx( a, b, c ) {"
								"	var xyz;"
								"	xyz = 'this is a text';"
								"	xyz = 123; // no a number"
								"}"
								);
	structure.color.clear();
	JsHighlighter * jsh = new JsHighlighter( );
	foreach( QString key, jsh->formats().keys() )
		structure.color[ key ] = jsh->formats()[ key ];
	delete jsh;
	m_managedStrucureList["js"] = structure;
	
	struct managedFile file;
	file.extentions = "*.xml *.xsl";
	file.name = QObject::tr("All XML File");
	file.canBeCustomize = true;
	file.customPath = "";
	file.structure = "xml";
	m_managedFileList.append( file );
	
	file.extentions = "*.xsl";
	file.name = QObject::tr("All Stylesheet");
	file.canBeCustomize = true;
	file.customPath = "";
	file.structure = "xml";
	m_managedFileList.append( file );

	file.extentions = "*.fws";
	file.name = QObject::tr("All XINX Webservices");
	file.canBeCustomize = false;
	file.customPath = "";
	file.structure = "xml";
	m_managedFileList.append( file );	

	file.extentions = "*.js";
	file.name = QObject::tr("All JavaScript");
	file.canBeCustomize = true;
	file.structure = "js";
	file.customPath = "js/";
	m_managedFileList.append( file );

}

XINXConfig::~XINXConfig(  ) {
	delete m_settings;
}


QString XINXConfig::extentions() {
	QString result;

	result += QObject::tr( "All Managed file" ) + "(";
	foreach( struct managedFile file, m_managedFileList ) {
		result += file.extentions;
		if( m_managedFileList.last().name != file.name ) {
			result += " ";
		}
	}
	result += ")";
	
	foreach( struct managedFile file, m_managedFileList ) {
		result += ";;" + file.name + "(" + file.extentions + ")";
	}
	
	return result;
}

void XINXConfig::save() {
	m_settings->setValue( "Language", m_lang );
	m_settings->setValue( "Create Backup File", m_createBackupFile );
	m_settings->setValue( "Position", m_xinxPosition );
	m_settings->setValue( "Size", m_xinxSize );
	m_settings->setValue( "Descriptions/Object", m_objectDescriptionPath );
	m_settings->setValue( "Descriptions/Completion", m_objectDescriptionPath );

	m_settings->setValue( "Project/Default Path", m_xinxProjectPath );
	m_settings->setValue( "Project/Alert when saving Standard File", m_alertWhenStdFile );
	m_settings->setValue( "Project/Recent Project Files", m_recentProjectFiles );

	m_settings->setValue( "Dock/File Content Visible", isDockSet( contents ) );
	m_settings->setValue( "Dock/Web Services Visible", isDockSet( services ) );
	
	foreach( QString cle, m_managedStrucureList.keys() ) {
		foreach( QString color, m_managedStrucureList[cle].color.keys() ) {
			QTextFormat format = m_managedStrucureList[cle].color[color];
			m_settings->setValue( QString("Structure/%1/%2").arg( cle ).arg( color ), format );
//			m_settings->setValue( QString("Structure/%1/%2").arg( cle ).arg( color ), QColor(Qt::red) );
		}
	}
	foreach( struct managedFile file, m_managedFileList ) {
		m_settings->setValue( QString("Files/%1/customPath").arg( file.name ), file.customPath );
		m_settings->setValue( QString("Files/%1/canBeCustomize").arg( file.name ), file.canBeCustomize );
	}
}

void XINXConfig::load() {
	m_lang             = m_settings->value( "Language", m_lang ).toString();
	m_createBackupFile = m_settings->value( "Create Backup File", m_createBackupFile ).toBool();
	m_xinxPosition     = m_settings->value( "Position", m_xinxPosition ).toPoint();
	m_xinxSize         = m_settings->value( "Size", m_xinxSize ).toSize();

	m_objectDescriptionPath = m_settings->value( "Descriptions/Object", m_objectDescriptionPath ).toString();
	
	m_xinxProjectPath    = m_settings->value( "Project/Default Path", m_xinxProjectPath ).toString();
	m_alertWhenStdFile   = m_settings->value( "Project/Alert when saving Standard File", m_alertWhenStdFile ).toBool();
	m_recentProjectFiles = m_settings->value( "Project/Recent Project Files" ).toStringList();

	if( m_settings->value( "Dock/File Content Visible", isDockSet( contents ) ).toBool() )
		setDock( contents );
	else
		unsetDock( contents );
		
	if( m_settings->value( "Dock/Web Services Visible", isDockSet( services ) ).toBool() )
		setDock( services );
	else
		unsetDock( services );
		

	foreach( QString cle, m_managedStrucureList.keys() ) {
		foreach( QString color, m_managedStrucureList[cle].color.keys() ) {
			QString name = QString("Structure/%1/%2").arg( cle ).arg( color );
			QVariant value = m_settings->value( name, m_managedStrucureList[cle].color[color] );
			if( value.isValid() && value.canConvert<QTextFormat>() ) {
				m_managedStrucureList[cle].color[color] = value.value<QTextFormat>();
			}
		}
	}

	foreach( struct managedFile file, m_managedFileList ) {
		file.customPath = m_settings->value( QString("Files/%1/customPath").arg( file.name ), file.customPath ).toString();
		file.canBeCustomize = m_settings->value( QString("Files/%1/canBeCustomize").arg( file.name ), file.canBeCustomize ).toBool();
	}		
}

//
