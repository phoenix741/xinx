// This file is auto-generated file, don't modify it unless you want to lose your code
// Generated by qgenconfig v0.1 (c) Ulrich Van Den Hekke

#ifndef _PLUGINSETTINGS_H_
#define _PLUGINSETTINGS_H_
#pragma once

#include <QSettings>
#include <QString>

class PrivatePluginSettings;

class PluginSettingsSettings : public QSettings {
public:
	PluginSettingsSettings( const QString & organization, const QString & application );
	void setValue( const QString & key, const QVariant & value, const QVariant & defaultValue );
	void setValue( const QString & key, const QVariant & value );
};

class PluginSettings {
public:
	struct struct_globals {
		QString progressMessages;
		int compressionLevel;
		bool pruneEmptyDirectories;
		bool createDirectories;
	};


	PluginSettings( const PluginSettings & origine );
	PluginSettings();
	virtual ~PluginSettings();

	virtual void setDefault();
	virtual void load();
	virtual void save();

	struct_globals & config();

	PluginSettings& operator=(const PluginSettings& p);
protected:
	virtual struct_globals getDefaultGlobals();
	virtual PluginSettings::struct_globals getSettingsGlobals( PluginSettingsSettings * settings, const QString & path, PluginSettings::struct_globals defaultValue );
	virtual void setSettingsGlobals( PluginSettingsSettings * settings, const QString & path, PluginSettings::struct_globals value );

private:
	PrivatePluginSettings * d;
	friend class PrivatePluginSettings;
};
#endif // _PLUGINSETTINGS_H_

