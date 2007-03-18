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
#include "xinxconfig.h"
//

// #define OPEN_SAVE_DIALOG_FILTER "All Managed File (*.xml;*.xsl;*.js;*.fws);;All XML File (*.xml;*.xsl);;All XSL File (*.xsl);;All JS File (*.js);;All WebServices XINX File (*.fws)"


XINXConfig::XINXConfig(  ) {
	m_lang = QLocale::languageToString( QLocale::system().language() );
	m_createBackupFile = true;
	m_alertWhenStdFile = true;
	m_xinxProjectPath = QDir( qApp->applicationDirPath() ).filePath( "project" );
	m_objectDescriptionPath = QDir( qApp->applicationDirPath() ).filePath( "xml" );
	
	struct managedStructure structure;
	structure.example = QObject::tr(
								"<library name=\"xinx\">"
								"	<book name=\"noname\">This is a description</book>"
								"	<!--This is a comment ;) -->"
								"	<<book name=\"with error\">"
								"</library>"
								);
	m_managedStrucureList["xml"] = structure;
	
	structure.example = QObject::tr(
								"function xxx( a, b, c ) {"
								"	var xyz;"
								"	xyz = 'this is a text';"
								"	xyz = 123; // no a number"
								"}"
								);
	m_managedStrucureList["js"] = structure;
	
	struct managedFile file;
	file.extentions = "*.xml;*.xsl";
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

QString XINXConfig::extentions() {
	QString result;

	result += QObject::tr( "All Managed file" ) + " (";
	foreach( struct managedFile file, m_managedFileList ) {
		result += file.extentions + ";";
	}
	result += ")";
	
	foreach( struct managedFile file, m_managedFileList ) {
		result += ";;" + file.name + "(" + file.extentions + ")";
	}
	
	return result;
}

//
