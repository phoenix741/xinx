// This file is auto-generated file, don't modify it unless you want to lose your code
// Generated by qgenconfig v0.1 (c) Ulrich Van Den Hekke


#include "webpluginsettings.h"

/* WebPluginSettingsSettings */

WebPluginSettingsSettings::WebPluginSettingsSettings( const QString & organization, const QString & application ) : QSettings( organization, application ) {
}

void WebPluginSettingsSettings::setValue( const QString & key, const QVariant & value, const QVariant & defaultValue ) {
	if( value == defaultValue )
		remove( key );
	else
		QSettings::setValue( key, value );
}

void WebPluginSettingsSettings::setValue( const QString & key, const QVariant & value ) {
	QSettings::setValue( key, value );
}

/* PrivateWebPluginSettings */

class PrivateWebPluginSettings {
public:
	PrivateWebPluginSettings( WebPluginSettings * parent );

	WebPluginSettingsSettings * m_settings;
	void createSettings();
	void deleteSettings();

	WebPluginSettings::struct_globals m_globals;
private:
	WebPluginSettings * m_parent;
};

PrivateWebPluginSettings::PrivateWebPluginSettings( WebPluginSettings * parent ) {
	m_parent = parent;
}

void PrivateWebPluginSettings::createSettings() {
	m_settings = new WebPluginSettingsSettings("Shadoware.Org", "XINX");
}

void PrivateWebPluginSettings::deleteSettings() {
	if( m_settings )
		delete m_settings;
	m_settings = NULL;
}

/* WebPluginSettings */

WebPluginSettings::WebPluginSettings( const WebPluginSettings & origine ) {
	d = new PrivateWebPluginSettings( this );
	d->m_globals = origine.d->m_globals;
}

WebPluginSettings::WebPluginSettings() {
	d = new PrivateWebPluginSettings( this );
}

WebPluginSettings::~WebPluginSettings() {
	delete d;
}

WebPluginSettings&WebPluginSettings::operator=( const WebPluginSettings& p ) {
	d->m_globals = p.d->m_globals;
	return *this;
}

WebPluginSettings::struct_globals & WebPluginSettings::config() {
	return d->m_globals;
}

void WebPluginSettings::setDefault() {
	d->m_globals = getDefaultGlobals();
}

void WebPluginSettings::save() {
	d->createSettings();
	setSettingsGlobals( d->m_settings, "plugins/webplugin", d->m_globals );
	d->deleteSettings();
}

void WebPluginSettings::load() {
	d->createSettings();
	d->m_globals = getSettingsGlobals( d->m_settings, "plugins/webplugin", getDefaultGlobals() );
	d->deleteSettings();
}

WebPluginSettings::WebPluginSettings::struct_javascript WebPluginSettings::getDefaultJavascript() {
	struct_javascript value;

	value.activeCompletion = true;

	return value;
}

WebPluginSettings::WebPluginSettings::struct_javascript WebPluginSettings::getSettingsJavascript( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::WebPluginSettings::struct_javascript & defaultValue ) {
	struct_javascript value;
	settings->beginGroup( path );

	value.activeCompletion = settings->value( "Active completion", defaultValue.activeCompletion ).toBool();

	settings->endGroup();
	return value;
}

void WebPluginSettings::setSettingsJavascript( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::WebPluginSettings::struct_javascript & value ) {
	struct_javascript defaultValue = getDefaultJavascript();
	settings->beginGroup( path );

	settings->setValue( "Active completion", value.activeCompletion, defaultValue.activeCompletion );

	settings->endGroup();
}

WebPluginSettings::WebPluginSettings::struct_stylesheetParsing WebPluginSettings::getDefaultStylesheetParsing() {
	struct_stylesheetParsing value;

	value.viewer = getDefaultViewerInformation();
	value.parser = getDefaultParserInformation();

	return value;
}

WebPluginSettings::WebPluginSettings::struct_stylesheetParsing WebPluginSettings::getSettingsStylesheetParsing( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::WebPluginSettings::struct_stylesheetParsing & defaultValue ) {
	struct_stylesheetParsing value;
	settings->beginGroup( path );

	value.viewer = getSettingsViewerInformation( settings, "Viewer", defaultValue.viewer );
	value.parser = getSettingsParserInformation( settings, "Parser", defaultValue.parser );

	settings->endGroup();
	return value;
}

void WebPluginSettings::setSettingsStylesheetParsing( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::struct_stylesheetParsing & value ) {
	struct_stylesheetParsing defaultValue = getDefaultStylesheetParsing();
	settings->beginGroup( path );

	setSettingsViewerInformation( settings, "Viewer", value.viewer );
	setSettingsParserInformation( settings, "Parser", value.parser );

	settings->endGroup();
}

WebPluginSettings::struct_parserInformation WebPluginSettings::getDefaultParserInformation() {
	struct_parserInformation value;

	value.type = "oracle";
	value.oracleParser = getDefaultOracleParser();

	return value;
}

WebPluginSettings::struct_parserInformation WebPluginSettings::getSettingsParserInformation( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::struct_parserInformation & defaultValue ) {
	struct_parserInformation value;
	settings->beginGroup( path );

	value.type = settings->value( "Type", defaultValue.type ).toString();
	value.oracleParser = getSettingsOracleParser( settings, "Oracle Parser", defaultValue.oracleParser );

	settings->endGroup();
	return value;
}

void WebPluginSettings::setSettingsParserInformation( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::struct_parserInformation & value ) {
	struct_parserInformation defaultValue = getDefaultParserInformation();
	settings->beginGroup( path );

	settings->setValue( "Type", value.type, defaultValue.type );
	setSettingsOracleParser( settings, "Oracle Parser", value.oracleParser );

	settings->endGroup();
}

WebPluginSettings::struct_oracleParser WebPluginSettings::getDefaultOracleParser() {
	struct_oracleParser value;

	value.classPath = "oracle.xml.parser.v2.oraxsl";
	value.parameters = "<source> <stylesheet>";

	return value;
}

WebPluginSettings::struct_oracleParser WebPluginSettings::getSettingsOracleParser( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::struct_oracleParser & defaultValue ) {
	struct_oracleParser value;
	settings->beginGroup( path );

	value.jarName = settings->value( "Jar Name", defaultValue.jarName ).toString();
	value.classPath = settings->value( "Class Path", defaultValue.classPath ).toString();
	value.parameters = settings->value( "Parameters", defaultValue.parameters ).toString();

	settings->endGroup();
	return value;
}

void WebPluginSettings::setSettingsOracleParser( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::struct_oracleParser & value ) {
	struct_oracleParser defaultValue = getDefaultOracleParser();
	settings->beginGroup( path );

	settings->setValue( "Jar Name", value.jarName, defaultValue.jarName );
	settings->setValue( "Class Path", value.classPath, defaultValue.classPath );
	settings->setValue( "Parameters", value.parameters, defaultValue.parameters );

	settings->endGroup();
}

WebPluginSettings::struct_viewerInformation WebPluginSettings::getDefaultViewerInformation() {
	struct_viewerInformation value;

	value.type = "internal";

	return value;
}

WebPluginSettings::struct_viewerInformation WebPluginSettings::getSettingsViewerInformation( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::struct_viewerInformation & defaultValue ) {
	struct_viewerInformation value;
	settings->beginGroup( path );

	value.type = settings->value( "Type", defaultValue.type ).toString();

	settings->endGroup();
	return value;
}

void WebPluginSettings::setSettingsViewerInformation( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::struct_viewerInformation & value ) {
	struct_viewerInformation defaultValue = getDefaultViewerInformation();
	settings->beginGroup( path );

	settings->setValue( "Type", value.type, defaultValue.type );

	settings->endGroup();
}

WebPluginSettings::struct_xml WebPluginSettings::getDefaultXml() {
	struct_xml value;

	value.activeCompletion = true;
	value.addClosedBalise = true;
	value.addDefaultAttribute = true;
	value.addDefaultSubBalise = true;

	return value;
}

WebPluginSettings::struct_xml WebPluginSettings::getSettingsXml( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::struct_xml & defaultValue ) {
	struct_xml value;
	settings->beginGroup( path );

	value.activeCompletion = settings->value( "Active completion", defaultValue.activeCompletion ).toBool();
	value.addClosedBalise = settings->value( "Add closed balise", defaultValue.addClosedBalise ).toBool();
	value.addDefaultAttribute = settings->value( "Add default attribute", defaultValue.addDefaultAttribute ).toBool();
	value.addDefaultSubBalise = settings->value( "Add default sub-balise", defaultValue.addDefaultSubBalise ).toBool();

	settings->endGroup();
	return value;
}

void WebPluginSettings::setSettingsXml( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::struct_xml & value ) {
	struct_xml defaultValue = getDefaultXml();
	settings->beginGroup( path );

	settings->setValue( "Active completion", value.activeCompletion, defaultValue.activeCompletion );
	settings->setValue( "Add closed balise", value.addClosedBalise, defaultValue.addClosedBalise );
	settings->setValue( "Add default attribute", value.addDefaultAttribute, defaultValue.addDefaultAttribute );
	settings->setValue( "Add default sub-balise", value.addDefaultSubBalise, defaultValue.addDefaultSubBalise );

	settings->endGroup();
}

WebPluginSettings::struct_globals WebPluginSettings::getDefaultGlobals() {
	struct_globals value;

	value.xml = getDefaultXml();
	value.stylesheetParsing = getDefaultStylesheetParsing();
	value.javascript = getDefaultJavascript();

	return value;
}

WebPluginSettings::struct_globals WebPluginSettings::getSettingsGlobals( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::struct_globals & defaultValue ) {
	struct_globals value;
	settings->beginGroup( path );

	value.xml = getSettingsXml( settings, "XML", defaultValue.xml );
	value.stylesheetParsing = getSettingsStylesheetParsing( settings, "Stylesheet Parsing", defaultValue.stylesheetParsing );
	value.javascript = getSettingsJavascript( settings, "JavaScript", defaultValue.javascript );

	settings->endGroup();
	return value;
}

void WebPluginSettings::setSettingsGlobals( WebPluginSettingsSettings * settings, const QString & path, const WebPluginSettings::struct_globals & value ) {
	struct_globals defaultValue = getDefaultGlobals();
	settings->beginGroup( path );

	setSettingsXml( settings, "XML", value.xml );
	setSettingsStylesheetParsing( settings, "Stylesheet Parsing", value.stylesheetParsing );
	setSettingsJavascript( settings, "JavaScript", value.javascript );

	settings->endGroup();
}

