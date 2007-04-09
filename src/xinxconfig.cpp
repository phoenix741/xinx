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
	m_settings = NULL;
	
	m_lang = QLocale::system().name();
	m_createBackupFile = true;
	m_alertWhenStdFile = true;
	m_xinxProjectPath = QDir( qApp->applicationDirPath() ).absoluteFilePath( "project" );
	m_objectDescriptionPath = QDir( qApp->applicationDirPath() ).absoluteFilePath( "xml" );
	
	m_xinxPosition = QPoint(200, 200);
	m_xinxSize = QSize(400, 400);
	m_docks = contents | files;
	
	struct managedStructure structure;
	structure.example = QObject::tr(
								"<library name=\"xinx\">\n"
								"	<book name=\"noname\">This is a description</book>\n"
								"	<!--This is a comment ;) -->\n"
								"	<<book name=\"with error\">\n"
								"</library>\n"
								);
	structure.color.clear();
	XmlHighlighter * xmlh = new XmlHighlighter( static_cast<QObject*>( NULL ), false );
	foreach( QString key, xmlh->formats().keys() )
		structure.color[ key ] = xmlh->formats()[ key ];
	delete xmlh;
	m_managedStrucureList["xml"] = structure;
	
	structure.example = QObject::tr(
								"function xxx( a, b, c ) {\n"
								"	var xyz;\n"
								"	xyz = 'this is a text';\n"
								"	xyz = 123; // no a number\n"
								"}\n"
								);
	structure.color.clear();
	JsHighlighter * jsh = new JsHighlighter( static_cast<QObject*>( NULL ), false );
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
	deleteSettings();
}

void XINXConfig::createSettings() {
	m_settings = new QSettings("Generix", "XINX");
}

void XINXConfig::deleteSettings() {
	if( m_settings )
		delete m_settings;
	m_settings = NULL;
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
	createSettings();
	m_settings->setValue( "Language", m_lang );
	m_settings->setValue( "Create Backup File", m_createBackupFile );
	m_settings->setValue( "Position", m_xinxPosition );
	m_settings->setValue( "Size", m_xinxSize );
	m_settings->setValue( "Descriptions/Object", m_objectDescriptionPath );
	m_settings->setValue( "Descriptions/Completion", m_objectDescriptionPath );

	m_settings->setValue( "Project/Default Path", m_xinxProjectPath );
	m_settings->setValue( "Project/Alert when saving Standard File", m_alertWhenStdFile );
	m_settings->setValue( "Project/Recent Project Files", m_recentProjectFiles );

	m_settings->setValue( "Dock", m_docks );
	
	foreach( QString cle, m_managedStrucureList.keys() ) {
		foreach( QString color, m_managedStrucureList[cle].color.keys() ) {
			QTextCharFormat format = m_managedStrucureList[cle].color[color];
			m_settings->setValue( QString("Structure/%1/%2/family").arg( cle ).arg( color ), format.fontFamily() );
			m_settings->setValue( QString("Structure/%1/%2/italic").arg( cle ).arg( color ), format.fontItalic() );
			m_settings->setValue( QString("Structure/%1/%2/overline").arg( cle ).arg( color ), format.fontOverline() );
			m_settings->setValue( QString("Structure/%1/%2/strikeOut").arg( cle ).arg( color ), format.fontStrikeOut() );
			m_settings->setValue( QString("Structure/%1/%2/underline").arg( cle ).arg( color ), format.fontUnderline() );
			m_settings->setValue( QString("Structure/%1/%2/size").arg( cle ).arg( color ), format.fontPointSize() );
			m_settings->setValue( QString("Structure/%1/%2/weight").arg( cle ).arg( color ), format.fontWeight() );
			m_settings->setValue( QString("Structure/%1/%2/color").arg( cle ).arg( color ), format.foreground() );
//			m_settings->setValue( QString("Structure/%1/%2").arg( cle ).arg( color ), QColor(Qt::red) );
		}
	}
	foreach( struct managedFile file, m_managedFileList ) {
		m_settings->setValue( QString("Files/%1/customPath").arg( file.name ), file.customPath );
		m_settings->setValue( QString("Files/%1/canBeCustomize").arg( file.name ), file.canBeCustomize );
	}
	deleteSettings();
}

void XINXConfig::load() {
	createSettings();
	m_lang             = m_settings->value( "Language", m_lang ).toString();
	m_createBackupFile = m_settings->value( "Create Backup File", m_createBackupFile ).toBool();
	m_xinxPosition     = m_settings->value( "Position", m_settings->value( "pos", m_xinxPosition ) ).toPoint();
	m_xinxSize         = m_settings->value( "Size", m_settings->value( "size", m_xinxSize ) ).toSize();

	m_objectDescriptionPath = m_settings->value( "Descriptions/Object", m_settings->value( "xmljavapath", m_objectDescriptionPath ) ).toString();
	
	m_xinxProjectPath    = m_settings->value( "Project/Default Path", m_xinxProjectPath ).toString();
	m_alertWhenStdFile   = m_settings->value( "Project/Alert when saving Standard File", m_alertWhenStdFile ).toBool();
	m_recentProjectFiles = m_settings->value( "Project/Recent Project Files", m_settings->value( "Recent Project Files" ) ).toStringList();

	m_docks = m_settings->value( "Dock", m_docks ).toInt();
		

	foreach( QString cle, m_managedStrucureList.keys() ) {
		foreach( QString color, m_managedStrucureList[cle].color.keys() ) {
			QTextCharFormat format = m_managedStrucureList[cle].color[color];
			format.setFontFamily(m_settings->value( QString("Structure/%1/%2/family").arg( cle ).arg( color ), format.fontFamily() ).toString());
			format.setFontItalic(m_settings->value( QString("Structure/%1/%2/italic").arg( cle ).arg( color ), format.fontItalic() ).toBool());
			format.setFontOverline(m_settings->value( QString("Structure/%1/%2/overline").arg( cle ).arg( color ), format.fontOverline() ).toBool());
			format.setFontStrikeOut(m_settings->value( QString("Structure/%1/%2/strikeOut").arg( cle ).arg( color ), format.fontStrikeOut() ).toBool());
			format.setFontUnderline(m_settings->value( QString("Structure/%1/%2/underline").arg( cle ).arg( color ), format.fontUnderline() ).toBool());
			qreal pointSize = m_settings->value( QString("Structure/%1/%2/size").arg( cle ).arg( color ), format.fontPointSize() ).toDouble();
			if( pointSize > 0 )
				format.setFontPointSize( pointSize );
			else 
				format.setFontPointSize( 8 );
			format.setFontWeight(m_settings->value( QString("Structure/%1/%2/weight").arg( cle ).arg( color ), format.fontWeight() ).toInt());
			format.setForeground(m_settings->value( QString("Structure/%1/%2/color").arg( cle ).arg( color ), format.foreground() ).value<QColor>());
			m_managedStrucureList[cle].color[color] = format;
			//QString name = QString("Structure/%1/%2").arg( cle ).arg( color );
			//QVariant value = m_settings->value( name, m_managedStrucureList[cle].color[color] );
			//if( value.isValid() && value.canConvert<QTextFormat>() ) {
				//m_managedStrucureList[cle].color[color] = value.value<QTextFormat>();
			//}
			
		}
	}

	for(int i = 0; i < m_managedFileList.size(); i++) {
		struct managedFile file = m_managedFileList.at(i);
		file.customPath = m_settings->value( QString("Files/%1/customPath").arg( file.name ), file.customPath ).toString();
		file.canBeCustomize = m_settings->value( QString("Files/%1/canBeCustomize").arg( file.name ), file.canBeCustomize ).toBool();
		m_managedFileList[i] = file;
	}		
	deleteSettings();
}

struct XINXConfig::managedFile XINXConfig::managedFile4Name( QString filename ) {
	foreach( struct managedFile file, m_managedFileList ) {
		QStringList list = file.extentions.split(" ");
		foreach( QString regexp, list ) {
			QRegExp extention( regexp );
			extention.setPatternSyntax( QRegExp::Wildcard );
			if( extention.exactMatch( filename ) ) {
				return file;
			}
		}
	}
	struct managedFile vide;
	vide.extentions = QString();
	vide.name = QString();
	vide.customPath = QString();
	vide.canBeCustomize = false;
	vide.structure = QString();
	return vide;
}


//
