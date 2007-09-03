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

#include "appsettings.h"
#include "jshighlighter.h"
#include "xmlhighlighter.h"

/* PrivateXINXConfig */

class PrivateAppSettings {
public:
	PrivateAppSettings( AppSettings * parent );

	QSettings * m_settings;
	void createSettings();
	void deleteSettings();
	
	struct_globals m_globals;
private:
	AppSettings * m_parent;
};

PrivateAppSettings::PrivateAppSettings( AppSettings * parent ) {
	m_parent = parent;
}

void PrivateAppSettings::createSettings() {
	m_settings = new QSettings("XINX Software", "XINX");
}

void PrivateAppSettings::deleteSettings() {
	if( m_settings )
		delete m_settings;
	m_settings = NULL;
}

/* XINXConfig */

AppSettings::AppSettings( const AppSettings & origine ) {
	d = new PrivateAppSettings( this );
	d->m_globals = origine.d->m_globals;
}


AppSettings::AppSettings() {
	d = new PrivateAppSettings( this );
}

AppSettings::~AppSettings() {
	delete d;
}
	
AppSettings& AppSettings::operator=(const AppSettings& p) {
	d->m_globals = p.d->m_globals;
	return *this;
}

struct_globals & AppSettings::config() {
	return d->m_globals;
}

void AppSettings::setDefault() {
	d->m_globals = getDefaultGlobals();
}

void AppSettings::save() {
	d->createSettings();
	setSettingsGlobals( d->m_settings, "", d->m_globals );
	d->deleteSettings();
}

void AppSettings::load() {
	d->createSettings();
	d->m_globals = getSettingsGlobals( d->m_settings, "", getDefaultGlobals() );
	d->deleteSettings();
}

struct_globals AppSettings::getDefaultGlobals() {
	struct_globals value;

	value.maximized = false;
	value.size = QSize(400,400);
	value.position = QPoint(200,200);
	value.language = QLocale::system().name();

	value.tools[ "cvs"  ] = "/usr/bin/cvs";
	value.tools[ "diff" ] = "/usr/bin/kompare";

	value.project = getDefaultProject();
	value.descriptions = getDefaultDescriptions();
	value.editor = getDefaultEditor();
	value.cvs = getDefaultCvs();

	value.files["xml"] = getDefaultExtentions();
	value.files["xsl"] = getDefaultExtentions();
	value.files["fws"] = getDefaultExtentions();
	value.files["fws"].canBeSpecifique = false;
	value.files["js"] = getDefaultExtentions();
	value.files["js"].customPath = "js/";
	
	return value;
}

struct_project AppSettings::getDefaultProject() {
	struct_project value; 
	
	value.saveWithSessionByDefault = false; 
	value.defaultPath = QDir( qApp->applicationDirPath() ).absoluteFilePath( "project" ); 
	value.alertWhenSavingStandardFile = true; 
	value.defaultProjectPathName = "projet"; 
	
	return value;
}

struct_descriptions AppSettings::getDefaultDescriptions() {
	struct_descriptions value; 
	
	value.object = QDir( qApp->applicationDirPath() ).absoluteFilePath( "../xml" ); 
	value.completion = QDir( qApp->applicationDirPath() ).absoluteFilePath( "../xml" );
	
	return value;
}

struct_editor AppSettings::getDefaultEditor() {
	struct_editor value;

	value.autoindentOnSaving = false;
	value.popupWhenFileModified = true; 
	value.createBackupFile = true; 
	value.completionLevel = 3; // 0 .. 3
	value.tabulationSize = 4; 
	value.showTabulationAndSpace = false; 

	return value;
}

struct_cvs AppSettings::getDefaultCvs() {
	struct_cvs value; 

	value.progressMessages = "-q";
	value.compressionLevel = 9; 
	value.pruneEmptyDirectories = false; 
	value.createDirectories = true;

	return value;
}

struct_extentions AppSettings::getDefaultExtentions() {
	struct_extentions value; 

	value.canBeSpecifique = true; 

	return value;
}


struct_globals AppSettings::getSettingsGlobals( QSettings * settings, const QString & path, struct_globals defaultValue ) {
	struct_globals value;
	
	settings->beginGroup( path );

	value.maximized = settings->value( "Maximized", defaultValue.maximized ).toBool();
	value.size = settings->value( "Size", defaultValue.size ).toSize();
	value.position = settings->value( "Position", defaultValue.position ).toPoint();
	value.language = settings->value( "Language", defaultValue.size ).toString();
	value.state = settings->value( "State", defaultValue.state ).toByteArray();

	value.project = getSettingsProject( settings, "Project", getDefaultProject() );
	value.descriptions = getSettingsDescriptions( settings, "Descriptions", getDefaultDescriptions() );
	value.editor = getSettingsEditor( settings, "Editor", getDefaultEditor() );
	value.cvs = getSettingsCvs( settings, "Cvs", getDefaultCvs() );

	value.tools = getSettingsHash_QString( settings, "Tools", defaultValue.tools );
	value.files = getSettingsHash_Extentions( settings, "Files", defaultValue.files );
	value.formats = getSettingsHash_QTextCharFormat( settings, "Formats", defaultValue.formats );

	settings->endGroup();

	return value;
}

void AppSettings::setSettingsGlobals( QSettings * settings, const QString & path, struct_globals value ) {
	settings->beginGroup( path );

	settings->setValue( "Maximized", value.maximized );
	settings->setValue( "Size", value.size );
	settings->setValue( "Position", value.position );
	settings->setValue( "Language", value.language );
	settings->setValue( "State", value.state );

	setSettingsProject( settings, "Project", value.project );
	setSettingsDescriptions( settings, "Descriptions", value.descriptions );
	setSettingsEditor( settings, "Editor", value.editor );
	setSettingsCvs( settings, "Cvs", value.cvs );

	setSettingsHash_QString( settings, "Tools", value.tools );
	setSettingsHash_Extentions( settings, "Files", value.files );
	setSettingsHash_QTextCharFormat( settings, "Formats", value.formats );

	settings->endGroup();
}

struct_project AppSettings::getSettingsProject( QSettings * settings, const QString & path, struct_project defaultValue ) {
	struct_project value; 
	
	settings->beginGroup( path );

	value.saveWithSessionByDefault = settings->value( "Save With Session By Default", defaultValue.saveWithSessionByDefault ).toBool();
	value.defaultPath = settings->value( "Default Path", defaultValue.defaultPath ).toString();
	value.alertWhenSavingStandardFile = settings->value( "Alert when saving Standard File", defaultValue.alertWhenSavingStandardFile ).toBool();
	value.recentProjectFiles = settings->value( "Recent Project Files", defaultValue.recentProjectFiles ).toStringList();
	value.defaultProjectPathName = settings->value( "Default Project Path Name", defaultValue.defaultProjectPathName ).toString();

	settings->endGroup();
	
	return value;
}

void AppSettings::setSettingsProject( QSettings * settings, const QString & path, struct_project value ) {
	settings->beginGroup( path );

	settings->setValue( "Save With Session By Default", value.saveWithSessionByDefault );
	settings->setValue( "Default Path", value.defaultPath );
	settings->setValue( "Alert when saving Standard File", value.alertWhenSavingStandardFile );
	settings->setValue( "Recent Project Files", value.recentProjectFiles );
	settings->setValue( "Default Project Path Name", value.defaultProjectPathName );

	settings->endGroup();

}

struct_descriptions AppSettings::getSettingsDescriptions( QSettings * settings, const QString & path, struct_descriptions defaultValue ) {
	struct_descriptions value; 
	
	settings->beginGroup( path );

	value.object = settings->value( "Object", defaultValue.object ).toString();
	value.completion = settings->value( "Completion", defaultValue.completion ).toString();

	settings->endGroup();
	
	return value;
}

void AppSettings::setSettingsDescriptions( QSettings * settings, const QString & path, struct_descriptions value ) {
	settings->beginGroup( path );

	settings->setValue( "Object", value.object );
	settings->setValue( "Completion", value.completion );

	settings->endGroup();
}

struct_editor AppSettings::getSettingsEditor( QSettings * settings, const QString & path, struct_editor defaultValue ) {
	struct_editor value; 
	
	settings->beginGroup( path );

	value.autoindentOnSaving = settings->value( "Autoindent On Saving", defaultValue.autoindentOnSaving ).toBool();
	value.popupWhenFileModified = settings->value( "Popup When File Modified", defaultValue.popupWhenFileModified ).toBool();
	value.createBackupFile = settings->value( "Create Backup File", defaultValue.createBackupFile ).toBool();
	value.completionLevel = settings->value( "Completion Level", defaultValue.completionLevel ).toInt();
	value.tabulationSize = settings->value( "Tabulation Size", defaultValue.tabulationSize ).toInt();
	value.showTabulationAndSpace = settings->value( "Show Tabulation and space", defaultValue.showTabulationAndSpace ).toBool();
	value.defaultFormat = settings->value( "Default Format", defaultValue.defaultFormat ).value<QFont>();

	settings->endGroup();
	
	return value;
}

void AppSettings::setSettingsEditor( QSettings * settings, const QString & path, struct_editor value ) {
	settings->beginGroup( path );

	settings->setValue( "Autoindent On Saving", value.autoindentOnSaving );
	settings->setValue( "Popup When File Modified", value.popupWhenFileModified );
	settings->setValue( "Create Backup File", value.createBackupFile );
	settings->setValue( "Completion Level", value.completionLevel );
	settings->setValue( "Tabulation Size", value.tabulationSize );
	settings->setValue( "Show Tabulation and space", value.showTabulationAndSpace );
	settings->setValue( "Default Format", value.defaultFormat );

	settings->endGroup();
}

struct_cvs AppSettings::getSettingsCvs( QSettings * settings, const QString & path, struct_cvs defaultValue ) {
	struct_cvs value; 
	
	settings->beginGroup( path );

	value.progressMessages = settings->value( "Progress Messages", defaultValue.progressMessages ).toString();
	value.compressionLevel = settings->value( "Comression Level", defaultValue.compressionLevel ).toInt();
	value.pruneEmptyDirectories = settings->value( "Prune Empty Directories", defaultValue.pruneEmptyDirectories ).toBool();
	value.createDirectories = settings->value( "Create Directories", defaultValue.createDirectories ).toBool();

	settings->endGroup();
	
	return value;
}

void AppSettings::setSettingsCvs( QSettings * settings, const QString & path, struct_cvs value ) {
	settings->beginGroup( path );

	settings->setValue( "Progress Messages", value.progressMessages );
	settings->setValue( "Comression Level", value.compressionLevel );
	settings->setValue( "Prune Empty Directories", value.pruneEmptyDirectories );
	settings->setValue( "Create Directories", value.createDirectories );

	settings->endGroup();
}

struct_extentions AppSettings::getSettingsExtentions( QSettings * settings, const QString & path, struct_extentions defaultValue ) {
	struct_extentions value; 
	
	settings->beginGroup( path );

	value.customPath = settings->value( "Custom Path", defaultValue.customPath ).toString();
	value.canBeSpecifique = settings->value( "Can Be Specifique", defaultValue.canBeSpecifique ).toBool();
	
	settings->endGroup();

	return value;
}

void AppSettings::setSettingsExtentions( QSettings * settings, const QString & path, struct_extentions value ) {
	settings->beginGroup( path );

	settings->setValue( "Custom Path", value.customPath );
	settings->setValue( "Can Be Specifique", value.canBeSpecifique );
	
	settings->endGroup();
}


QTextCharFormat AppSettings::getTextFormatFromSettings( QSettings * settings, const QString & path, QTextCharFormat defaultValue ) {
	QTextCharFormat format;
	settings->beginGroup( path );
	
	format.setFontItalic( settings->value( "italic", defaultValue.fontItalic() ).toBool() );
	format.setFontOverline( settings->value( "overline", defaultValue.fontOverline() ).toBool() );
	format.setFontStrikeOut( settings->value( "strikeOut", defaultValue.fontStrikeOut() ).toBool() );
	format.setFontUnderline( settings->value( "underline", defaultValue.fontUnderline() ).toBool() );
	format.setFontWeight( settings->value( "weight", defaultValue.fontWeight() ).toInt() );
	format.setForeground( settings->value( "color", defaultValue.foreground() ).value<QColor>() );

	settings->endGroup();
	return format;
}

void AppSettings::setTextFormatFromSettings( QSettings * settings, const QString & path, QTextCharFormat value ) {
	settings->beginGroup( path );

	settings->setValue( "italic", value.fontItalic() );
	settings->setValue( "overline", value.fontOverline() );
	settings->setValue( "strikeOut", value.fontStrikeOut() );
	settings->setValue( "underline", value.fontUnderline() );
	settings->setValue( "weight", value.fontWeight() );
	settings->setValue( "color", value.foreground() );

	settings->endGroup();
}

QHash<QString,QString> AppSettings::getSettingsHash_QString( QSettings * settings, const QString & path, QHash<QString,QString> defaultValue ) {
	QHash<QString,QString> value;
	settings->beginGroup( path );
	
	QStringList keys = settings->childKeys() + settings->childGroups();
	foreach( QString key, keys ) {
		value[ key ] = settings->value( key, defaultValue[ key ] ).toString();		
	}	
	foreach( QString defaultValueKey, defaultValue.keys() ) {
		if( ! value.contains( defaultValueKey ) ) {
			value[ defaultValueKey ] = defaultValue[ defaultValueKey ];
		}
	}
	
	settings->endGroup();
	return value;
}

void AppSettings::setSettingsHash_QString( QSettings * settings, const QString & path, QHash<QString,QString> value ) {
	settings->beginGroup( path );
	
	foreach( QString key, value.keys() ) {
		settings->setValue( key, value[ key ] );		
	}	
	
	settings->endGroup();
}

QHash<QString,struct_extentions> AppSettings::getSettingsHash_Extentions( QSettings * settings, const QString & path, QHash<QString,struct_extentions> defaultValue ) {
	QHash<QString,struct_extentions> value;
	settings->beginGroup( path );
	
	QStringList keys = settings->childKeys() + settings->childGroups();
	foreach( QString key, keys ) {
		value[ key ] = 	getSettingsExtentions( settings, key, defaultValue[ key ] );		
	}	
	foreach( QString defaultValueKey, defaultValue.keys() ) {
		if( ! value.contains( defaultValueKey ) ) {
			value[ defaultValueKey ] = defaultValue[ defaultValueKey ];
		}
	}
	
	settings->endGroup();
	return value;
}

void AppSettings::setSettingsHash_Extentions( QSettings * settings, const QString & path, QHash<QString,struct_extentions> value ) {
	settings->beginGroup( path );
	
	foreach( QString key, value.keys() ) {
		setSettingsExtentions( settings, key, value[ key ] );
	}	
	
	settings->endGroup();
}

QHash<QString,QTextCharFormat> AppSettings::getSettingsHash_QTextCharFormat( QSettings * settings, const QString & path, QHash<QString,QTextCharFormat> defaultValue ) {
	QHash<QString,QTextCharFormat> value;
	settings->beginGroup( path );
	
	QStringList keys = settings->childKeys() + settings->childGroups();
	foreach( QString key, keys ) {
		value[ key ] = getTextFormatFromSettings( settings, key, defaultValue[ key ] );		
	}
	foreach( QString defaultValueKey, defaultValue.keys() ) {
		if( ! value.contains( defaultValueKey ) ) {
			value[ defaultValueKey ] = defaultValue[ defaultValueKey ];
		}
	}
	
	settings->endGroup();
	return value;
}

void AppSettings::setSettingsHash_QTextCharFormat( QSettings * settings, const QString & path, QHash<QString,QTextCharFormat> value ) {
	settings->beginGroup( path );
	
	foreach( QString key, value.keys() ) {
		setTextFormatFromSettings( settings, key, value[ key ] );
	}	
	
	settings->endGroup();
}


/*
XINXConfig::XINXConfig(  ) {
	d = new PrivateXINXConfig( this );
	
	d->m_settings = NULL;
	
	d->m_lang = QLocale::system().name();
	d->m_createBackupFile = true;
	d->m_alertWhenStdFile = true;
	d->m_saveSessionByDefault = true;	
	d->m_popupWhenFileModified = true;
	d->m_xinxProjectPath = QDir( qApp->applicationDirPath() ).absoluteFilePath( "project" );
	d->m_objectDescriptionPath = QDir( qApp->applicationDirPath() ).absoluteFilePath( "../xml" );
	d->m_defaultProjectPathName = "projet";
	
	d->m_xinxPosition = QPoint(200, 200);
	d->m_xinxSize = QSize(400, 400);
	struct XINXConfig::managedStructure structure;
	structure.example = QObject::tr(
								"<library name=\"xinx\">\n"
								"	<book name=\"noname\">This is a description</book>\n"
								"	<!--This is a comment ;) -.\n"
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

	d->m_toolsPath[ "merge" ] = "WinMerge.exe";
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

QList<struct XINXConfig::managedFile> & XINXConfig::managedFile() { 
	return d->m_managedFileList; 
}

QHash<QString,struct XINXConfig::managedStructure> & XINXConfig::managedStructure() { 
	return d->m_managedStrucureList;
}
*/
