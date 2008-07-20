// This file is auto-generated file, don't modify it unless you want to lose your code
// Generated by qgenconfig v0.1 (c) Ulrich Van Den Hekke

#ifndef _WEBPLUGINSETTINGS_H_
#define _WEBPLUGINSETTINGS_H_

#include <QSettings>

class PrivateWebPluginSettings;

class WebPluginSettings {
public:
	struct struct_javascript {
		bool activeCompletion;
	};

	struct struct_xml {
		bool activeCompletion;
		bool addClosedBalise;
		bool addDefaultAttribute;
		bool addDefaultSubBalise;
	};

	struct struct_globals {
		struct_xml xml;
		struct_javascript javascript;
	};


	WebPluginSettings( const WebPluginSettings & origine );
	WebPluginSettings();
	virtual ~WebPluginSettings();

	virtual void setDefault();
	virtual void load();
	virtual void save();

	struct_globals & config();

	WebPluginSettings& operator=(const WebPluginSettings& p);
protected:
	virtual struct_javascript getDefaultJavascript();
	virtual WebPluginSettings::struct_javascript getSettingsJavascript( QSettings * settings, const QString & path, WebPluginSettings::struct_javascript defaultValue );
	virtual void setSettingsJavascript( QSettings * settings, const QString & path, WebPluginSettings::struct_javascript value );

	virtual struct_xml getDefaultXml();
	virtual WebPluginSettings::struct_xml getSettingsXml( QSettings * settings, const QString & path, WebPluginSettings::struct_xml defaultValue );
	virtual void setSettingsXml( QSettings * settings, const QString & path, WebPluginSettings::struct_xml value );

	virtual struct_globals getDefaultGlobals();
	virtual WebPluginSettings::struct_globals getSettingsGlobals( QSettings * settings, const QString & path, WebPluginSettings::struct_globals defaultValue );
	virtual void setSettingsGlobals( QSettings * settings, const QString & path, WebPluginSettings::struct_globals value );

private:
	PrivateWebPluginSettings * d;
	friend class PrivateWebPluginSettings;
};
#endif // _WEBPLUGINSETTINGS_H_
