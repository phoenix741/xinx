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

/* PrivateXINXConfig */

class PrivateXINXConfig {
public:
	PrivateXINXConfig( XINXConfig * parent );

	QSettings * m_settings;

	QPoint m_xinxPosition;
	QSize m_xinxSize;
	
	QByteArray m_mainWindowState;
	
	QString m_lang;
	bool m_createBackupFile;
	bool m_alertWhenStdFile;
	bool m_saveSessionByDefault;
	bool m_popupWhenFileModified;
	QString m_xinxProjectPath;
	QString m_objectDescriptionPath;
	
	QStringList m_recentProjectFiles;
	
	QList<struct XINXConfig::managedFile> m_managedFileList;
	QHash<QString, struct XINXConfig::managedStructure> m_managedStrucureList;
	
	QHash<QString,QString> m_toolsPath;
	
	QString m_cvsProgressMessages;
	int m_cvsCompressionLevel;
	bool m_cvsPruneEmptyDirectories;
	bool m_cvsCreateDirectories;
	
	void createSettings();
	void deleteSettings();
private:
	XINXConfig * m_parent;
};

PrivateXINXConfig::PrivateXINXConfig( XINXConfig * parent ) {
	m_parent = parent;
}

void PrivateXINXConfig::createSettings() {
	m_settings = new QSettings("Generix", "XINX");
}

void PrivateXINXConfig::deleteSettings() {
	if( m_settings )
		delete m_settings;
	m_settings = NULL;
}

/* XINXConfig */

XINXConfig::XINXConfig(  ) {
	d = new PrivateXINXConfig( this );
	
	d->m_settings = NULL;
	
	d->m_lang = QLocale::system().name();
	d->m_createBackupFile = true;
	d->m_alertWhenStdFile = true;
	d->m_saveSessionByDefault = true;	
	d->m_popupWhenFileModified = true;
	d->m_xinxProjectPath = QDir( qApp->applicationDirPath() ).absoluteFilePath( "project" );
	d->m_objectDescriptionPath = QDir( qApp->applicationDirPath() ).absoluteFilePath( "xml" );
	
	d->m_xinxPosition = QPoint(200, 200);
	d->m_xinxSize = QSize(400, 400);
	struct XINXConfig::managedStructure structure;
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
	d->m_managedStrucureList["xml"] = structure;
	
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
	d->m_managedStrucureList["js"] = structure;
	
	struct XINXConfig::managedFile file;
	file.extentions = "*.xml";
	file.name = QObject::tr("All XML File");
	file.canBeCustomize = true;
	file.customPath = "";
	file.structure = "xml";
	d->m_managedFileList.append( file );
	
	file.extentions = "*.xsl";
	file.name = QObject::tr("All Stylesheet");
	file.canBeCustomize = true;
	file.customPath = "";
	file.structure = "xml";
	d->m_managedFileList.append( file );

	file.extentions = "*.fws";
	file.name = QObject::tr("All XINX Webservices");
	file.canBeCustomize = false;
	file.customPath = "";
	file.structure = "xml";
	d->m_managedFileList.append( file );	

	file.extentions = "*.js";
	file.name = QObject::tr("All JavaScript");
	file.canBeCustomize = true;
	file.structure = "js";
	file.customPath = "js/";
	d->m_managedFileList.append( file );

	d->m_toolsPath[ "cvs" ] = "/usr/bin/cvs";
	d->m_cvsProgressMessages = "-q";
	d->m_cvsCompressionLevel = 9;
	d->m_cvsPruneEmptyDirectories = false;
	d->m_cvsCreateDirectories = true;
}

XINXConfig::~XINXConfig(  ) {
	d->deleteSettings();
}

QStringList XINXConfig::dialogFilters() {
	QStringList result;

	QString managed = QObject::tr( "All Managed file" ) + "(";
	foreach( struct XINXConfig::managedFile file, d->m_managedFileList ) {
		managed += file.extentions;
		if( d->m_managedFileList.last().name != file.name ) {
			managed += " ";
		}
	}
	managed += ")";
	result.append( managed );
	
	foreach( struct XINXConfig::managedFile file, d->m_managedFileList ) {
		result.append( file.name + "(" + file.extentions + ")" );
	}
	
	return result;
}

QString XINXConfig::dialogFilter( QString ext ) {
	foreach( struct managedFile file, d->m_managedFileList ) {
		QStringList list = file.extentions.split(" ");
		foreach( QString regexp, list ) {
			QRegExp extention( regexp );
			extention.setPatternSyntax( QRegExp::Wildcard );
			if( extention.exactMatch( "filename." + ext ) ) {
				return file.name + "(" + file.extentions + ")";
			}
		}
	}
	return QString();
}


void XINXConfig::save() {
	d->createSettings();
	d->m_settings->setValue( "Language", d->m_lang );
	d->m_settings->setValue( "Create Backup File", d->m_createBackupFile );
	d->m_settings->setValue( "Save Session By Default", d->m_saveSessionByDefault );
	d->m_settings->setValue( "Position", d->m_xinxPosition );
	d->m_settings->setValue( "Size", d->m_xinxSize );
	d->m_settings->setValue( "Descriptions/Object", d->m_objectDescriptionPath );
	d->m_settings->setValue( "Descriptions/Completion", d->m_objectDescriptionPath );
	d->m_settings->setValue( "Popup When File Modified", d->m_popupWhenFileModified );

	d->m_settings->setValue( "Project/Default Path", d->m_xinxProjectPath );
	d->m_settings->setValue( "Project/Alert when saving Standard File", d->m_alertWhenStdFile );
	d->m_settings->setValue( "Project/Recent Project Files",d-> m_recentProjectFiles );

	d->m_settings->setValue( "State", d->m_mainWindowState );
	
	foreach( QString cle, d->m_managedStrucureList.keys() ) {
		foreach( QString color, d->m_managedStrucureList[cle].color.keys() ) {
			QTextCharFormat format = d->m_managedStrucureList[cle].color[color];
			d->m_settings->setValue( QString("Structure/%1/%2/family").arg( cle ).arg( color ), format.fontFamily() );
			d->m_settings->setValue( QString("Structure/%1/%2/italic").arg( cle ).arg( color ), format.fontItalic() );
			d->m_settings->setValue( QString("Structure/%1/%2/overline").arg( cle ).arg( color ), format.fontOverline() );
			d->m_settings->setValue( QString("Structure/%1/%2/strikeOut").arg( cle ).arg( color ), format.fontStrikeOut() );
			d->m_settings->setValue( QString("Structure/%1/%2/underline").arg( cle ).arg( color ), format.fontUnderline() );
			d->m_settings->setValue( QString("Structure/%1/%2/size").arg( cle ).arg( color ), format.fontPointSize() );
			d->m_settings->setValue( QString("Structure/%1/%2/weight").arg( cle ).arg( color ), format.fontWeight() );
			d->m_settings->setValue( QString("Structure/%1/%2/color").arg( cle ).arg( color ), format.foreground() );
//			d->m_settings->setValue( QString("Structure/%1/%2").arg( cle ).arg( color ), QColor(Qt::red) );
		}
	}
	foreach( struct managedFile file, d->m_managedFileList ) {
		d->m_settings->setValue( QString("Files/%1/customPath").arg( file.name ), file.customPath );
		d->m_settings->setValue( QString("Files/%1/canBeCustomize").arg( file.name ), file.canBeCustomize );
	}
	
	foreach( QString tool, d->m_toolsPath.keys() ) {
		d->m_settings->setValue( QString("Tools/%1").arg( tool ), d->m_toolsPath[ tool ] );
	}
	d->m_settings->setValue( "CVS/Progress Messages", d->m_cvsProgressMessages );
	d->m_settings->setValue( "CVS/Comression Level", d->m_cvsCompressionLevel );
	d->m_settings->setValue( "CVS/Prune Empty Directories", d->m_cvsPruneEmptyDirectories );
	d->m_settings->setValue( "CVS/Create Directories", d->m_cvsCreateDirectories );
	
	d->deleteSettings();
}

void XINXConfig::load() {
	d->createSettings();
	d->m_lang                  = d->m_settings->value( "Language", d->m_lang ).toString();
	d->m_createBackupFile      = d->m_settings->value( "Create Backup File", d->m_createBackupFile ).toBool();
	d->m_saveSessionByDefault  = d->m_settings->value( "Save Session By Default", d->m_saveSessionByDefault ).toBool();
	d->m_xinxPosition          = d->m_settings->value( "Position", d->m_settings->value( "pos", d->m_xinxPosition ) ).toPoint();
	d->m_xinxSize              = d->m_settings->value( "Size", d->m_settings->value( "size", d->m_xinxSize ) ).toSize();
	d->m_popupWhenFileModified = d->m_settings->value( "Popup When File Modified", d->m_popupWhenFileModified ).toBool();

	d->m_objectDescriptionPath = d->m_settings->value( "Descriptions/Object", d->m_settings->value( "xmljavapath", d->m_objectDescriptionPath ) ).toString();
	
	d->m_xinxProjectPath    = d->m_settings->value( "Project/Default Path", d->m_xinxProjectPath ).toString();
	d->m_alertWhenStdFile   = d->m_settings->value( "Project/Alert when saving Standard File", d->m_alertWhenStdFile ).toBool();
	d->m_recentProjectFiles = d->m_settings->value( "Project/Recent Project Files", d->m_settings->value( "Recent Project Files" ) ).toStringList();

	d->m_mainWindowState    = d->m_settings->value( "State" ).toByteArray();		

	foreach( QString cle, d->m_managedStrucureList.keys() ) {
		foreach( QString color, d->m_managedStrucureList[cle].color.keys() ) {
			QTextCharFormat format = d->m_managedStrucureList[cle].color[color];
			format.setFontFamily(d->m_settings->value( QString("Structure/%1/%2/family").arg( cle ).arg( color ), format.fontFamily() ).toString());
			format.setFontItalic(d->m_settings->value( QString("Structure/%1/%2/italic").arg( cle ).arg( color ), format.fontItalic() ).toBool());
			format.setFontOverline(d->m_settings->value( QString("Structure/%1/%2/overline").arg( cle ).arg( color ), format.fontOverline() ).toBool());
			format.setFontStrikeOut(d->m_settings->value( QString("Structure/%1/%2/strikeOut").arg( cle ).arg( color ), format.fontStrikeOut() ).toBool());
			format.setFontUnderline(d->m_settings->value( QString("Structure/%1/%2/underline").arg( cle ).arg( color ), format.fontUnderline() ).toBool());
			qreal pointSize = d->m_settings->value( QString("Structure/%1/%2/size").arg( cle ).arg( color ), format.fontPointSize() ).toDouble();
			if( pointSize > 0 )
				format.setFontPointSize( pointSize );
			else 
				format.setFontPointSize( 8 );
			format.setFontWeight(d->m_settings->value( QString("Structure/%1/%2/weight").arg( cle ).arg( color ), format.fontWeight() ).toInt());
			format.setForeground(d->m_settings->value( QString("Structure/%1/%2/color").arg( cle ).arg( color ), format.foreground() ).value<QColor>());
			d->m_managedStrucureList[cle].color[color] = format;
			//QString name = QString("Structure/%1/%2").arg( cle ).arg( color );
			//QVariant value = m_settings->value( name, m_managedStrucureList[cle].color[color] );
			//if( value.isValid() && value.canConvert<QTextFormat>() ) {
				//m_managedStrucureList[cle].color[color] = value.value<QTextFormat>();
			//}
			
		}
	}

	for(int i = 0; i < d->m_managedFileList.size(); i++) {
		struct managedFile file = d->m_managedFileList.at(i);
		file.customPath = d->m_settings->value( QString("Files/%1/customPath").arg( file.name ), file.customPath ).toString();
		file.canBeCustomize = d->m_settings->value( QString("Files/%1/canBeCustomize").arg( file.name ), file.canBeCustomize ).toBool();
		d->m_managedFileList[i] = file;
	}		

	foreach( QString tool, d->m_toolsPath.keys() ) {
		d->m_toolsPath[ tool ] = d->m_settings->value( QString("Tools/%1").arg( tool ), d->m_toolsPath[ tool ] ).toString();
	}
	d->m_cvsProgressMessages = d->m_settings->value( "CVS/Progress Messages", d->m_cvsProgressMessages ).toString();
	d->m_cvsCompressionLevel = d->m_settings->value( "CVS/Comression Level", d->m_cvsCompressionLevel ).toInt();
	d->m_cvsPruneEmptyDirectories = d->m_settings->value( "CVS/Prune Empty Directories", d->m_cvsPruneEmptyDirectories ).toBool();
	d->m_cvsCreateDirectories = d->m_settings->value( "CVS/Create Directories", d->m_cvsCreateDirectories ).toBool();

	d->deleteSettings();
}

struct XINXConfig::managedFile XINXConfig::managedFile4Name( QString filename ) {
	foreach( struct managedFile file, d->m_managedFileList ) {
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

struct XINXConfig::managedFile XINXConfig::managedFile4Suffix( QString suffix ) {
	return managedFile4Name( "noname." + suffix );
}


QString XINXConfig::lang() const { 
	return d->m_lang;
}

void XINXConfig::setLang( const QString & value ) { 
	d->m_lang = value; 
}
	
bool XINXConfig::saveSessionByDefault() const { 
	return d->m_saveSessionByDefault; 
}

void XINXConfig::setSaveSessionByDefault( bool value ) {
	d->m_saveSessionByDefault = value; 
}

QByteArray XINXConfig::storedMainWindowState() { 
	return d->m_mainWindowState; 
}

void XINXConfig::storeMainWindowState( QByteArray state ) { 
	d->m_mainWindowState = state; 
}
	
QPoint XINXConfig::position() const { 
	return d->m_xinxPosition; 
}

void XINXConfig::setPosition( QPoint value ) { 
	d->m_xinxPosition = value; 
}
	
QSize XINXConfig::size() const { 
	return d->m_xinxSize; 
}

void XINXConfig::setSize( QSize value ) { 
	d->m_xinxSize = value;
}
	
bool XINXConfig::isCreateBackupFile() const { 
	return d->m_createBackupFile; 
}

void XINXConfig::setCreateBackupFile( bool value ) { 
	d->m_createBackupFile = value; 
}
	
bool XINXConfig::isAlertWhenStdFile() const { 
	return d->m_alertWhenStdFile; 
}

void XINXConfig::setAlertWhenStdFile( bool value ) { 
	d->m_alertWhenStdFile = value;
}
	
QString XINXConfig::xinxProjectPath() const { 
	return d->m_xinxProjectPath; 
}

void XINXConfig::setXinxProjectPath( const QString & value ) { 
	d->m_xinxProjectPath = value; 
}
	
QString XINXConfig::objectDescriptionPath() const { 
	return d->m_objectDescriptionPath; 
}

void XINXConfig::setObjectDescriptionPath( const QString & value ) { 
	d->m_objectDescriptionPath = value; 
}
	
QString XINXConfig::completionFilesPath() const { 
	return d->m_objectDescriptionPath; 
}

void XINXConfig::setCompletionFilesPath( const QString & value ) { 
	d->m_objectDescriptionPath = value; 
}
	
QStringList & XINXConfig::recentProjectFiles() { 
	return d->m_recentProjectFiles; 
}

QList<struct XINXConfig::managedFile> & XINXConfig::managedFile() { 
	return d->m_managedFileList; 
}

QHash<QString,struct XINXConfig::managedStructure> & XINXConfig::managedStructure() { 
	return d->m_managedStrucureList;
}
	
QHash<QString,QString> & XINXConfig::toolsPath() { 
	return d->m_toolsPath; 
}

bool XINXConfig::popupWhenFileModified() const {
	return d->m_popupWhenFileModified;
}

void XINXConfig::setPopupWhenFileModified( bool value ) {
	d->m_popupWhenFileModified = value;
}
	
QString XINXConfig::cvsProgressMessages() {
	return d->m_cvsProgressMessages;
}

int XINXConfig::cvsCompressionLevel() {
	return d->m_cvsCompressionLevel;
}

bool XINXConfig::cvsPruneEmptyDirectories() {
	return d->m_cvsPruneEmptyDirectories;
}

bool XINXConfig::cvsCreateDirectories() {
	return d->m_cvsCreateDirectories;	
}

void XINXConfig::setCVSProgressMessages( QString value ) {
	d->m_cvsProgressMessages = value;
}

void XINXConfig::setCVSCompressionLevel( int value ) {
	d->m_cvsCompressionLevel = value;
}

void XINXConfig::setCVSPruneEmptyDirectories( bool value ) {
	d->m_cvsPruneEmptyDirectories = value;
}

void XINXConfig::setCVSCreateDirectories( bool value ) {
	d->m_cvsCreateDirectories = value;
}
