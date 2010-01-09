// This file is auto-generated file, don't modify it unless you want to lose your code
// Generated by qgenconfig v0.1 (c) Ulrich Van Den Hekke


#include "generixsettings.h"

/* GenerixSettingsSettings */

GenerixSettingsSettings::GenerixSettingsSettings( const QString & organization, const QString & application ) : QSettings( organization, application ) {
}

void GenerixSettingsSettings::setValue( const QString & key, const QVariant & value, const QVariant & defaultValue ) {
	if( value == defaultValue )
		remove( key );
	else
		QSettings::setValue( key, value );
}

void GenerixSettingsSettings::setValue( const QString & key, const QVariant & value ) {
	QSettings::setValue( key, value );
}

/* PrivateGenerixSettings */

class PrivateGenerixSettings {
public:
	PrivateGenerixSettings( GenerixSettings * parent );

	GenerixSettingsSettings * m_settings;
	void createSettings();
	void deleteSettings();

	GenerixSettings::struct_globals m_globals;
private:
	GenerixSettings * m_parent;
};

PrivateGenerixSettings::PrivateGenerixSettings( GenerixSettings * parent ) {
	m_parent = parent;
}

void PrivateGenerixSettings::createSettings() {
	m_settings = new GenerixSettingsSettings("Shadoware.Org", "XINX");
}

void PrivateGenerixSettings::deleteSettings() {
	if( m_settings )
		delete m_settings;
	m_settings = NULL;
}

/* GenerixSettings */

GenerixSettings::GenerixSettings( const GenerixSettings & origine ) {
	d = new PrivateGenerixSettings( this );
	d->m_globals = origine.d->m_globals;
}

GenerixSettings::GenerixSettings() {
	d = new PrivateGenerixSettings( this );
}

GenerixSettings::~GenerixSettings() {
	delete d;
}

GenerixSettings&GenerixSettings::operator=( const GenerixSettings& p ) {
	d->m_globals = p.d->m_globals;
	return *this;
}

GenerixSettings::struct_globals & GenerixSettings::config() {
	return d->m_globals;
}

void GenerixSettings::setDefault() {
	d->m_globals = getDefaultGlobals();
}

void GenerixSettings::save() {
	d->createSettings();
	setSettingsGlobals( d->m_settings, "PluginsSettings/generix", d->m_globals );
	d->deleteSettings();
}

void GenerixSettings::load() {
	d->createSettings();
	d->m_globals = getSettingsGlobals( d->m_settings, "PluginsSettings/generix", getDefaultGlobals() );
	d->deleteSettings();
}

GenerixSettings::GenerixSettings::struct_extentions GenerixSettings::getDefaultExtentions() {
	struct_extentions value;

	value.canBeSaveAsSpecifique = true;
	value.canBeFindInConfiguration = false;

	return value;
}

GenerixSettings::GenerixSettings::struct_extentions GenerixSettings::getSettingsExtentions( GenerixSettingsSettings * settings, const QString & path, const GenerixSettings::GenerixSettings::struct_extentions & defaultValue ) {
	struct_extentions value;
	settings->beginGroup( path );

	value.specifiqueSubDirectory = settings->value( "Specifique Sub Directory", defaultValue.specifiqueSubDirectory ).toString();
	value.canBeSaveAsSpecifique = settings->value( "Can Be Save As Specifique", defaultValue.canBeSaveAsSpecifique ).toBool();
	value.canBeFindInConfiguration = settings->value( "Can Be Find In Configuration", defaultValue.canBeFindInConfiguration ).toBool();

	settings->endGroup();
	return value;
}

void GenerixSettings::setSettingsExtentions( GenerixSettingsSettings * settings, const QString & path, const GenerixSettings::GenerixSettings::struct_extentions & value ) {
	struct_extentions defaultValue = getDefaultExtentions();
	settings->beginGroup( path );

	settings->setValue( "Specifique Sub Directory", value.specifiqueSubDirectory, defaultValue.specifiqueSubDirectory );
	settings->setValue( "Can Be Save As Specifique", value.canBeSaveAsSpecifique, defaultValue.canBeSaveAsSpecifique );
	settings->setValue( "Can Be Find In Configuration", value.canBeFindInConfiguration, defaultValue.canBeFindInConfiguration );

	settings->endGroup();
}

GenerixSettings::GenerixSettings::struct_globals GenerixSettings::getDefaultGlobals() {
	struct_globals value;

	value.defaultProjectPathName = "projet";
	value.files = getDefaultHash_struct_extentions();

	return value;
}

GenerixSettings::GenerixSettings::struct_globals GenerixSettings::getSettingsGlobals( GenerixSettingsSettings * settings, const QString & path, const GenerixSettings::GenerixSettings::struct_globals & defaultValue ) {
	struct_globals value;
	settings->beginGroup( path );

	value.defaultProjectPathName = settings->value( "Default Project Path Name", defaultValue.defaultProjectPathName ).toString();
	value.files = getSettingsHash_struct_extentions( settings, "Files", defaultValue.files );

	settings->endGroup();
	return value;
}

void GenerixSettings::setSettingsGlobals( GenerixSettingsSettings * settings, const QString & path, const GenerixSettings::GenerixSettings::struct_globals & value ) {
	struct_globals defaultValue = getDefaultGlobals();
	settings->beginGroup( path );

	settings->setValue( "Default Project Path Name", value.defaultProjectPathName, defaultValue.defaultProjectPathName );
	setSettingsHash_struct_extentions( settings, "Files", value.files );

	settings->endGroup();
}

QHash<QString,GenerixSettings::struct_extentions> GenerixSettings::getDefaultHash_struct_extentions() {
	QHash<QString,struct_extentions> value;


	return value;
}

QHash<QString,GenerixSettings::struct_extentions> GenerixSettings::getSettingsHash_struct_extentions( GenerixSettingsSettings * settings, const QString & path, const QHash<QString,GenerixSettings::struct_extentions> & defaultValue ) {
	QHash<QString,struct_extentions> value;
	settings->beginGroup( path );

	QStringList keys = settings->childKeys() + settings->childGroups();
	foreach( const QString & key, keys ) {
		struct_extentions def;
		if( defaultValue.contains( key ) )
			def = defaultValue[ key ];
		else
			def = getDefaultExtentions();
		value[ key ] = getSettingsExtentions( settings, key, def );
	}
	foreach( const QString & defaultValueKey, defaultValue.keys() ) {
		if( ! value.contains( defaultValueKey ) ) {
			value[ defaultValueKey ] = defaultValue[ defaultValueKey ];
		}
	}

	settings->endGroup();
	return value;
}

void GenerixSettings::setSettingsHash_struct_extentions( GenerixSettingsSettings * settings, const QString & path, const QHash<QString,GenerixSettings::struct_extentions> & value ) {
	QHash<QString,struct_extentions> defaultValue = getDefaultHash_struct_extentions();
	settings->beginGroup( path );

	foreach( const QString & key, value.keys() ) {
		setSettingsExtentions( settings, key, value[ key ] );
	}

	settings->endGroup();
}
