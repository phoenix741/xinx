// This file is auto-generated file, don't modify it unless you want to lose your code
// Generated by qgenconfig v0.1 (c) Ulrich Van Den Hekke

#include <QLocale>
#include <QDir>
#include <QApplication>

#include "appsettings.h"

/* AppSettingsSettings */

AppSettingsSettings::AppSettingsSettings( const QString & organization, const QString & application ) : QSettings( organization, application ) {
}

void AppSettingsSettings::setValue( const QString & key, const QVariant & value, const QVariant & defaultValue ) {
	if( value == defaultValue )
		remove( key );
	else
		QSettings::setValue( key, value );
}

void AppSettingsSettings::setValue( const QString & key, const QVariant & value ) {
	QSettings::setValue( key, value );
}

/* PrivateAppSettings */

class PrivateAppSettings {
public:
	PrivateAppSettings( AppSettings * parent );

	AppSettingsSettings * m_settings;
	void createSettings();
	void deleteSettings();

	AppSettings::struct_globals m_globals;
private:
	AppSettings * m_parent;
};

PrivateAppSettings::PrivateAppSettings( AppSettings * parent ) {
	m_parent = parent;
}

void PrivateAppSettings::createSettings() {
	m_settings = new AppSettingsSettings("Shadoware.Org", "XINX");
}

void PrivateAppSettings::deleteSettings() {
	if( m_settings )
		delete m_settings;
	m_settings = NULL;
}

/* AppSettings */

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

AppSettings&AppSettings::operator=( const AppSettings& p ) {
	d->m_globals = p.d->m_globals;
	return *this;
}

AppSettings::struct_globals & AppSettings::config() {
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

AppSettings::AppSettings::struct_configurationEditor AppSettings::getDefaultConfigurationEditor() {
	struct_configurationEditor value;

	value.activateConfigurationEditor = false;
	value.autoLoadConfigurationFile = true;
	value.autoSaveConfigurationFile = false;

	return value;
}

AppSettings::AppSettings::struct_configurationEditor AppSettings::getSettingsConfigurationEditor( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_configurationEditor defaultValue ) {
	struct_configurationEditor value;
	settings->beginGroup( path );

	value.activateConfigurationEditor = settings->value( "Activate Configuration Editor", defaultValue.activateConfigurationEditor ).toBool();
	value.autoLoadConfigurationFile = settings->value( "Auto load configuration file", defaultValue.autoLoadConfigurationFile ).toBool();
	value.autoSaveConfigurationFile = settings->value( "Auto save configuration file", defaultValue.autoSaveConfigurationFile ).toBool();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsConfigurationEditor( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_configurationEditor value ) {
	struct_configurationEditor defaultValue = getDefaultConfigurationEditor();
	settings->beginGroup( path );

	settings->setValue( "Activate Configuration Editor", value.activateConfigurationEditor, defaultValue.activateConfigurationEditor );
	settings->setValue( "Auto load configuration file", value.autoLoadConfigurationFile, defaultValue.autoLoadConfigurationFile );
	settings->setValue( "Auto save configuration file", value.autoSaveConfigurationFile, defaultValue.autoSaveConfigurationFile );

	settings->endGroup();
}

AppSettings::AppSettings::struct_editor AppSettings::getDefaultEditor() {
	struct_editor value;

	value.defaultTextCodec = "UTF-8";
	value.autoindentOnSaving = false;
	value.popupWhenFileModified = true;
	value.createBackupFile = true;
	value.tabulationSize = 4;
	value.showTabulationAndSpace = false;
	value.highlightCurrentLine = true;
	value.highlightWord = Qt::yellow;
	value.autoHighlight = false;
	value.hideCloseTab = false;
	value.closeButtonOnEachTab = false;
	value.automaticModelRefreshTimeout = 1000;

	return value;
}

AppSettings::AppSettings::struct_editor AppSettings::getSettingsEditor( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_editor defaultValue ) {
	struct_editor value;
	settings->beginGroup( path );

	value.defaultTextCodec = settings->value( "Default Text Codec", defaultValue.defaultTextCodec ).toString();
	value.autoindentOnSaving = settings->value( "Autoindent On Saving", defaultValue.autoindentOnSaving ).toBool();
	value.popupWhenFileModified = settings->value( "Popup When File Modified", defaultValue.popupWhenFileModified ).toBool();
	value.createBackupFile = settings->value( "Create Backup File", defaultValue.createBackupFile ).toBool();
	value.tabulationSize = settings->value( "Tabulation Size", defaultValue.tabulationSize ).toInt();
	value.showTabulationAndSpace = settings->value( "Show Tabulation and space", defaultValue.showTabulationAndSpace ).toBool();
	value.highlightCurrentLine = settings->value( "Highlight Current Line", defaultValue.highlightCurrentLine ).toBool();
	value.defaultFormat = settings->value( "Default format", defaultValue.defaultFormat ).value<QFont>();
	value.highlightWord = settings->value( "Highlight word", defaultValue.highlightWord ).value<QColor>();
	value.autoHighlight = settings->value( "Auto Highlight", defaultValue.autoHighlight ).toBool();
	value.hideCloseTab = settings->value( "Hide Close Tab", defaultValue.hideCloseTab ).toBool();
	value.closeButtonOnEachTab = settings->value( "Close Button on each Tab", defaultValue.closeButtonOnEachTab ).toBool();
	value.automaticModelRefreshTimeout = settings->value( "Automatic Model Refresh Timeout", defaultValue.automaticModelRefreshTimeout ).toInt();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsEditor( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_editor value ) {
	struct_editor defaultValue = getDefaultEditor();
	settings->beginGroup( path );

	settings->setValue( "Default Text Codec", value.defaultTextCodec, defaultValue.defaultTextCodec );
	settings->setValue( "Autoindent On Saving", value.autoindentOnSaving, defaultValue.autoindentOnSaving );
	settings->setValue( "Popup When File Modified", value.popupWhenFileModified, defaultValue.popupWhenFileModified );
	settings->setValue( "Create Backup File", value.createBackupFile, defaultValue.createBackupFile );
	settings->setValue( "Tabulation Size", value.tabulationSize, defaultValue.tabulationSize );
	settings->setValue( "Show Tabulation and space", value.showTabulationAndSpace, defaultValue.showTabulationAndSpace );
	settings->setValue( "Highlight Current Line", value.highlightCurrentLine, defaultValue.highlightCurrentLine );
	settings->setValue( "Default format", value.defaultFormat, defaultValue.defaultFormat );
	settings->setValue( "Highlight word", value.highlightWord, defaultValue.highlightWord );
	settings->setValue( "Auto Highlight", value.autoHighlight, defaultValue.autoHighlight );
	settings->setValue( "Hide Close Tab", value.hideCloseTab, defaultValue.hideCloseTab );
	settings->setValue( "Close Button on each Tab", value.closeButtonOnEachTab, defaultValue.closeButtonOnEachTab );
	settings->setValue( "Automatic Model Refresh Timeout", value.automaticModelRefreshTimeout, defaultValue.automaticModelRefreshTimeout );

	settings->endGroup();
}

AppSettings::AppSettings::struct_rcs AppSettings::getDefaultRcs() {
	struct_rcs value;

	value.createChangelog = false;

	return value;
}

AppSettings::AppSettings::struct_rcs AppSettings::getSettingsRcs( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_rcs defaultValue ) {
	struct_rcs value;
	settings->beginGroup( path );

	value.createChangelog = settings->value( "Create ChangeLog", defaultValue.createChangelog ).toBool();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsRcs( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_rcs value ) {
	struct_rcs defaultValue = getDefaultRcs();
	settings->beginGroup( path );

	settings->setValue( "Create ChangeLog", value.createChangelog, defaultValue.createChangelog );

	settings->endGroup();
}

AppSettings::AppSettings::struct_project AppSettings::getDefaultProject() {
	struct_project value;

	value.openTheLastProjectAtStart = false;
	value.saveWithSessionByDefault = false;
	value.defaultPath = QDir( qApp->applicationDirPath() ).absoluteFilePath( "project" );
	value.defaultProjectPathName = "projet";
	value.closeVersionManagementLog = true;
	value.automaticProjectDirectoryRefreshTimeout = 500;

	return value;
}

AppSettings::AppSettings::struct_project AppSettings::getSettingsProject( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_project defaultValue ) {
	struct_project value;
	settings->beginGroup( path );

	value.lastOpenedProject = settings->value( "Last opened project", defaultValue.lastOpenedProject ).toString();
	value.openTheLastProjectAtStart = settings->value( "Open the last project at start", defaultValue.openTheLastProjectAtStart ).toBool();
	value.saveWithSessionByDefault = settings->value( "Save With Session By Default", defaultValue.saveWithSessionByDefault ).toBool();
	value.defaultPath = settings->value( "Default Path", defaultValue.defaultPath ).toString();
	value.recentProjectFiles = settings->value( "Recent Project Files", defaultValue.recentProjectFiles ).toStringList();
	value.defaultProjectPathName = settings->value( "Default Project Path Name", defaultValue.defaultProjectPathName ).toString();
	value.closeVersionManagementLog = settings->value( "Close Version Management Log", defaultValue.closeVersionManagementLog ).toBool();
	value.automaticProjectDirectoryRefreshTimeout = settings->value( "Automatic Project Directory Refresh Timeout", defaultValue.automaticProjectDirectoryRefreshTimeout ).toInt();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsProject( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_project value ) {
	struct_project defaultValue = getDefaultProject();
	settings->beginGroup( path );

	settings->setValue( "Last opened project", value.lastOpenedProject, defaultValue.lastOpenedProject );
	settings->setValue( "Open the last project at start", value.openTheLastProjectAtStart, defaultValue.openTheLastProjectAtStart );
	settings->setValue( "Save With Session By Default", value.saveWithSessionByDefault, defaultValue.saveWithSessionByDefault );
	settings->setValue( "Default Path", value.defaultPath, defaultValue.defaultPath );
	settings->setValue( "Recent Project Files", value.recentProjectFiles, defaultValue.recentProjectFiles );
	settings->setValue( "Default Project Path Name", value.defaultProjectPathName, defaultValue.defaultProjectPathName );
	settings->setValue( "Close Version Management Log", value.closeVersionManagementLog, defaultValue.closeVersionManagementLog );
	settings->setValue( "Automatic Project Directory Refresh Timeout", value.automaticProjectDirectoryRefreshTimeout, defaultValue.automaticProjectDirectoryRefreshTimeout );

	settings->endGroup();
}

AppSettings::AppSettings::struct_descriptions AppSettings::getDefaultDescriptions() {
	struct_descriptions value;

	value.datas = QDir( qApp->applicationDirPath() ).absoluteFilePath( "../xml" );

	return value;
}

AppSettings::AppSettings::struct_descriptions AppSettings::getSettingsDescriptions( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_descriptions defaultValue ) {
	struct_descriptions value;
	settings->beginGroup( path );

	value.datas = settings->value( "Datas", defaultValue.datas ).toString();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsDescriptions( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_descriptions value ) {
	struct_descriptions defaultValue = getDefaultDescriptions();
	settings->beginGroup( path );

	settings->setValue( "Datas", value.datas, defaultValue.datas );

	settings->endGroup();
}

AppSettings::AppSettings::struct_extentions AppSettings::getDefaultExtentions() {
	struct_extentions value;

	value.canBeSpecifique = true;

	return value;
}

AppSettings::AppSettings::struct_extentions AppSettings::getSettingsExtentions( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_extentions defaultValue ) {
	struct_extentions value;
	settings->beginGroup( path );

	value.customPath = settings->value( "Custom Path", defaultValue.customPath ).toString();
	value.canBeSpecifique = settings->value( "Can Be Specifique", defaultValue.canBeSpecifique ).toBool();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsExtentions( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_extentions value ) {
	struct_extentions defaultValue = getDefaultExtentions();
	settings->beginGroup( path );

	settings->setValue( "Custom Path", value.customPath, defaultValue.customPath );
	settings->setValue( "Can Be Specifique", value.canBeSpecifique, defaultValue.canBeSpecifique );

	settings->endGroup();
}

AppSettings::AppSettings::struct_xmlpres AppSettings::getDefaultXmlpres() {
	struct_xmlpres value;

	value.autoExpandedPath = "/layout_data/VueUtilisateurCourantSociete/JUt_UtiView/JUt_UtiViewRow";
	value.hidePath = QString("debug,application_data").split(",");
	value.showFilteredSubTree = true;
	value.viewColor = Qt::blue;
	value.errorColor = Qt::red;
	value.screenDataColor = Qt::darkRed;
	value.showNameAttributeIfExists = true;

	return value;
}

AppSettings::AppSettings::struct_xmlpres AppSettings::getSettingsXmlpres( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_xmlpres defaultValue ) {
	struct_xmlpres value;
	settings->beginGroup( path );

	value.autoExpandedPath = settings->value( "Auto expanded path", defaultValue.autoExpandedPath ).toString();
	value.hidePath = settings->value( "Hide path", defaultValue.hidePath ).toStringList();
	value.showFilteredSubTree = settings->value( "Show filtered sub-tree", defaultValue.showFilteredSubTree ).toBool();
	value.viewColor = settings->value( "View Color", defaultValue.viewColor ).value<QColor>();
	value.errorColor = settings->value( "Error Color", defaultValue.errorColor ).value<QColor>();
	value.screenDataColor = settings->value( "Screen data Color", defaultValue.screenDataColor ).value<QColor>();
	value.showNameAttributeIfExists = settings->value( "Show name attribute if exists", defaultValue.showNameAttributeIfExists ).toBool();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsXmlpres( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_xmlpres value ) {
	struct_xmlpres defaultValue = getDefaultXmlpres();
	settings->beginGroup( path );

	settings->setValue( "Auto expanded path", value.autoExpandedPath, defaultValue.autoExpandedPath );
	settings->setValue( "Hide path", value.hidePath, defaultValue.hidePath );
	settings->setValue( "Show filtered sub-tree", value.showFilteredSubTree, defaultValue.showFilteredSubTree );
	settings->setValue( "View Color", value.viewColor, defaultValue.viewColor );
	settings->setValue( "Error Color", value.errorColor, defaultValue.errorColor );
	settings->setValue( "Screen data Color", value.screenDataColor, defaultValue.screenDataColor );
	settings->setValue( "Show name attribute if exists", value.showNameAttributeIfExists, defaultValue.showNameAttributeIfExists );

	settings->endGroup();
}

AppSettings::AppSettings::struct_globals AppSettings::getDefaultGlobals() {
	struct_globals value;

	value.language = QLocale::system().name();
	value.position = QPoint( 200,200 );
	value.size = QSize( 400,400 );
	value.maximized = false;
	value.plugins = getDefaultHash_bool();
	value.descriptions = getDefaultDescriptions();
	value.project = getDefaultProject();
	value.rcs = getDefaultRcs();
	value.editor = getDefaultEditor();
	value.configurationEditor = getDefaultConfigurationEditor();
	value.xmlPres = getDefaultXmlpres();
	value.tools = getDefaultHash_QString();
	value.files = getDefaultHash_struct_extentions();
	value.formats = getDefaultHash_QTextCharFormat();

	return value;
}

AppSettings::AppSettings::struct_globals AppSettings::getSettingsGlobals( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_globals defaultValue ) {
	struct_globals value;
	settings->beginGroup( path );

	value.language = settings->value( "Language", defaultValue.language ).toString();
	value.position = settings->value( "Position", defaultValue.position ).toPoint();
	value.size = settings->value( "Size", defaultValue.size ).toSize();
	value.maximized = settings->value( "Maximized", defaultValue.maximized ).toBool();
	value.state = settings->value( "State", defaultValue.state ).toByteArray();
	value.xinxTrace = settings->value( "XINX Trace", defaultValue.xinxTrace ).toString();
	value.plugins = getSettingsHash_bool( settings, "Plugins", defaultValue.plugins );
	value.descriptions = getSettingsDescriptions( settings, "Descriptions", defaultValue.descriptions );
	value.project = getSettingsProject( settings, "Project", defaultValue.project );
	value.rcs = getSettingsRcs( settings, "RCS", defaultValue.rcs );
	value.editor = getSettingsEditor( settings, "Editor", defaultValue.editor );
	value.configurationEditor = getSettingsConfigurationEditor( settings, "Configuration Editor", defaultValue.configurationEditor );
	value.xmlPres = getSettingsXmlpres( settings, "Xml Pres", defaultValue.xmlPres );
	value.tools = getSettingsHash_QString( settings, "Tools", defaultValue.tools );
	value.files = getSettingsHash_struct_extentions( settings, "Files", defaultValue.files );
	value.formats = getSettingsHash_QTextCharFormat( settings, "Formats", defaultValue.formats );

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsGlobals( AppSettingsSettings * settings, const QString & path, AppSettings::AppSettings::struct_globals value ) {
	struct_globals defaultValue = getDefaultGlobals();
	settings->beginGroup( path );

	settings->setValue( "Language", value.language, defaultValue.language );
	settings->setValue( "Position", value.position, defaultValue.position );
	settings->setValue( "Size", value.size, defaultValue.size );
	settings->setValue( "Maximized", value.maximized, defaultValue.maximized );
	settings->setValue( "State", value.state, defaultValue.state );
	settings->setValue( "XINX Trace", value.xinxTrace, defaultValue.xinxTrace );
	setSettingsHash_bool( settings, "Plugins", value.plugins );
	setSettingsDescriptions( settings, "Descriptions", value.descriptions );
	setSettingsProject( settings, "Project", value.project );
	setSettingsRcs( settings, "RCS", value.rcs );
	setSettingsEditor( settings, "Editor", value.editor );
	setSettingsConfigurationEditor( settings, "Configuration Editor", value.configurationEditor );
	setSettingsXmlpres( settings, "Xml Pres", value.xmlPres );
	setSettingsHash_QString( settings, "Tools", value.tools );
	setSettingsHash_struct_extentions( settings, "Files", value.files );
	setSettingsHash_QTextCharFormat( settings, "Formats", value.formats );

	settings->endGroup();
}

QHash<QString,bool> AppSettings::getDefaultHash_bool() {
	QHash<QString,bool> value;

	value[ "WebPlugin" ] = true;
	value[ "ServicesPlugin" ] = true;
	value[ "SVNPlugin" ] = true;
	value[ "CVSPlugin" ] = true;

	return value;
}

QHash<QString,bool> AppSettings::getSettingsHash_bool( AppSettingsSettings * settings, const QString & path, QHash<QString,bool> defaultValue ) {
	QHash<QString,bool> value;
	settings->beginGroup( path );

	QStringList keys = settings->childKeys() + settings->childGroups();
	foreach( QString key, keys ) {
		value[ key ] = settings->value( key, defaultValue[ key ] ).toBool();
	}
	foreach( QString defaultValueKey, defaultValue.keys() ) {
		if( ! value.contains( defaultValueKey ) ) {
			value[ defaultValueKey ] = defaultValue[ defaultValueKey ];
		}
	}

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsHash_bool( AppSettingsSettings * settings, const QString & path, QHash<QString,bool> value ) {
	QHash<QString,bool> defaultValue = getDefaultHash_bool();
	settings->beginGroup( path );

	foreach( QString key, value.keys() ) {
		settings->setValue( key, value[ key ], defaultValue[ key ] );
	}

	settings->endGroup();
}

QHash<QString,QString> AppSettings::getDefaultHash_QString() {
	QHash<QString,QString> value;

	value[ "diff" ] = "/usr/bin/kompare";

	return value;
}

QHash<QString,QString> AppSettings::getSettingsHash_QString( AppSettingsSettings * settings, const QString & path, QHash<QString,QString> defaultValue ) {
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

void AppSettings::setSettingsHash_QString( AppSettingsSettings * settings, const QString & path, QHash<QString,QString> value ) {
	QHash<QString,QString> defaultValue = getDefaultHash_QString();
	settings->beginGroup( path );

	foreach( QString key, value.keys() ) {
		settings->setValue( key, value[ key ], defaultValue[ key ] );
	}

	settings->endGroup();
}

QHash<QString,AppSettings::struct_extentions> AppSettings::getDefaultHash_struct_extentions() {
	QHash<QString,struct_extentions> value;


	return value;
}

QHash<QString,AppSettings::struct_extentions> AppSettings::getSettingsHash_struct_extentions( AppSettingsSettings * settings, const QString & path, QHash<QString,AppSettings::struct_extentions> defaultValue ) {
	QHash<QString,struct_extentions> value;
	settings->beginGroup( path );

	QStringList keys = settings->childKeys() + settings->childGroups();
	foreach( QString key, keys ) {
		value[ key ] = getSettingsExtentions( settings, key, defaultValue[ key ] );
	}
	foreach( QString defaultValueKey, defaultValue.keys() ) {
		if( ! value.contains( defaultValueKey ) ) {
			value[ defaultValueKey ] = defaultValue[ defaultValueKey ];
		}
	}

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsHash_struct_extentions( AppSettingsSettings * settings, const QString & path, QHash<QString,AppSettings::struct_extentions> value ) {
	QHash<QString,struct_extentions> defaultValue = getDefaultHash_struct_extentions();
	settings->beginGroup( path );

	foreach( QString key, value.keys() ) {
		setSettingsExtentions( settings, key, value[ key ] );
	}

	settings->endGroup();
}

QTextCharFormat AppSettings::getSettingsTextCharFormat( AppSettingsSettings * settings, const QString & path, QTextCharFormat defaultValue ) {
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

void AppSettings::setSettingsTextCharFormat( AppSettingsSettings * settings, const QString & path, QTextCharFormat value ) {
	settings->beginGroup( path );

	settings->setValue( "italic", value.fontItalic() );
	settings->setValue( "overline", value.fontOverline() );
	settings->setValue( "strikeOut", value.fontStrikeOut() );
	settings->setValue( "underline", value.fontUnderline() );
	settings->setValue( "weight", value.fontWeight() );
	settings->setValue( "color", value.foreground() );

	settings->endGroup();
}

QHash<QString,QTextCharFormat> AppSettings::getDefaultHash_QTextCharFormat() {
	QHash<QString,QTextCharFormat> value;


	return value;
}

QHash<QString,QTextCharFormat> AppSettings::getSettingsHash_QTextCharFormat( AppSettingsSettings * settings, const QString & path, QHash<QString,QTextCharFormat> defaultValue ) {
	QHash<QString,QTextCharFormat> value;
	settings->beginGroup( path );

	QStringList keys = settings->childKeys() + settings->childGroups();
	foreach( QString key, keys ) {
		value[ key ] = getSettingsTextCharFormat( settings, key, defaultValue[ key ] );
	}
	foreach( QString defaultValueKey, defaultValue.keys() ) {
		if( ! value.contains( defaultValueKey ) ) {
			value[ defaultValueKey ] = defaultValue[ defaultValueKey ];
		}
	}

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsHash_QTextCharFormat( AppSettingsSettings * settings, const QString & path, QHash<QString,QTextCharFormat> value ) {
	QHash<QString,QTextCharFormat> defaultValue = getDefaultHash_QTextCharFormat();
	settings->beginGroup( path );

	foreach( QString key, value.keys() ) {
		setSettingsTextCharFormat( settings, key, value[ key ] );
	}

	settings->endGroup();
}

