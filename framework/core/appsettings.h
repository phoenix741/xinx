// This file is auto-generated file, don't modify it unless you want to lose your code
// Generated by qgenconfig v0.1 (c) Ulrich Van Den Hekke

#ifndef _APPSETTINGS_H_
#define _APPSETTINGS_H_
#pragma once

#include <QSettings>
#include <QString>
#include <QStringList>
#include <QColor>
#include <QFont>
#include <QPoint>
#include <QSize>
#include <QByteArray>
#include <QHash>
#include <QHash>
#include <QHash>
#include <QHash>

class PrivateAppSettings;

class AppSettingsSettings : public QSettings {
public:
	AppSettingsSettings( const QString & organization, const QString & application );
	void setValue( const QString & key, const QVariant & value, const QVariant & defaultValue );
	void setValue( const QString & key, const QVariant & value );
};

class AppSettings {
public:
	struct struct_snipets {
		bool alwaysShowDialog;
	};

	struct struct_qformat {
		bool italic;
		bool bold;
		bool overline;
		bool strikout;
		bool underline;
		bool waveunderline;
		QColor foreground;
		QColor background;
	};

	struct struct_configurationEditor {
		bool activateConfigurationEditor;
		bool autoLoadConfigurationFile;
		bool autoSaveConfigurationFile;
	};

	struct struct_editor {
		QString defaultTextCodec;
		bool autoindentOnSaving;
		bool popupWhenFileModified;
		bool createBackupFile;
		int tabulationSize;
		bool showTabulationAndSpace;
		bool highlightCurrentLine;
		QFont defaultFormat;
		QColor highlightWord;
		bool autoHighlight;
		bool hideCloseTab;
		bool closeButtonOnEachTab;
		int automaticModelRefreshTimeout;
	};

	struct struct_rcs {
		bool createChangelog;
	};

	struct struct_project {
		QString lastOpenedProject;
		bool openTheLastProjectAtStart;
		bool saveWithSessionByDefault;
		QString defaultPath;
		QStringList recentProjectFiles;
		QString defaultProjectPathName;
		bool closeVersionManagementLog;
		int automaticProjectDirectoryRefreshTimeout;
	};

	struct struct_extentions {
		QString specifiqueSubDirectory;
		bool canBeSaveAsSpecifique;
		bool canBeFindInConfiguration;
		bool canBeCommitToRcs;
	};

	struct struct_globals {
		QString language;
		QPoint position;
		QSize size;
		bool maximized;
		QByteArray state;
		QString xinxTrace;
		QString style;
		QHash<QString,bool> plugins;
		struct_project project;
		struct_rcs rcs;
		struct_editor editor;
		struct_configurationEditor configurationEditor;
		struct_snipets snipets;
		QHash<QString,QString> tools;
		QHash<QString,struct_extentions> files;
		QHash<QString,struct_qformat> formats;
	};


	AppSettings( const AppSettings & origine );
	AppSettings();
	virtual ~AppSettings();

	virtual void setDefault();
	virtual void load();
	virtual void save();

	struct_globals & config();

	AppSettings& operator=(const AppSettings& p);
protected:
	virtual struct_snipets getDefaultSnipets();
	virtual AppSettings::struct_snipets getSettingsSnipets( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_snipets & defaultValue );
	virtual void setSettingsSnipets( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_snipets & value );

	virtual struct_qformat getDefaultQformat();
	virtual AppSettings::struct_qformat getSettingsQformat( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_qformat & defaultValue );
	virtual void setSettingsQformat( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_qformat & value );

	virtual struct_configurationEditor getDefaultConfigurationEditor();
	virtual AppSettings::struct_configurationEditor getSettingsConfigurationEditor( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_configurationEditor & defaultValue );
	virtual void setSettingsConfigurationEditor( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_configurationEditor & value );

	virtual struct_editor getDefaultEditor();
	virtual AppSettings::struct_editor getSettingsEditor( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_editor & defaultValue );
	virtual void setSettingsEditor( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_editor & value );

	virtual struct_rcs getDefaultRcs();
	virtual AppSettings::struct_rcs getSettingsRcs( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_rcs & defaultValue );
	virtual void setSettingsRcs( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_rcs & value );

	virtual struct_project getDefaultProject();
	virtual AppSettings::struct_project getSettingsProject( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_project & defaultValue );
	virtual void setSettingsProject( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_project & value );

	virtual struct_extentions getDefaultExtentions();
	virtual AppSettings::struct_extentions getSettingsExtentions( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_extentions & defaultValue );
	virtual void setSettingsExtentions( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_extentions & value );

	virtual struct_globals getDefaultGlobals();
	virtual AppSettings::struct_globals getSettingsGlobals( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_globals & defaultValue );
	virtual void setSettingsGlobals( AppSettingsSettings * settings, const QString & path, const AppSettings::struct_globals & value );

	virtual QHash<QString,bool> getSettingsHash_bool( AppSettingsSettings * settings, const QString & path, const QHash<QString,bool> & defaultValue );
	virtual void setSettingsHash_bool( AppSettingsSettings * settings, const QString & path, const QHash<QString,bool> & value );

	virtual QHash<QString,QString> getSettingsHash_QString( AppSettingsSettings * settings, const QString & path, const QHash<QString,QString> & defaultValue );
	virtual void setSettingsHash_QString( AppSettingsSettings * settings, const QString & path, const QHash<QString,QString> & value );

	virtual QHash<QString,struct_extentions> getDefaultHash_struct_extentions();
	virtual QHash<QString,AppSettings::struct_extentions> getSettingsHash_struct_extentions( AppSettingsSettings * settings, const QString & path, const QHash<QString,AppSettings::struct_extentions> & defaultValue );
	virtual void setSettingsHash_struct_extentions( AppSettingsSettings * settings, const QString & path, const QHash<QString,AppSettings::struct_extentions> & value );

	virtual QHash<QString,struct_qformat> getDefaultHash_struct_qformat();
	virtual QHash<QString,AppSettings::struct_qformat> getSettingsHash_struct_qformat( AppSettingsSettings * settings, const QString & path, const QHash<QString,AppSettings::struct_qformat> & defaultValue );
	virtual void setSettingsHash_struct_qformat( AppSettingsSettings * settings, const QString & path, const QHash<QString,AppSettings::struct_qformat> & value );

private:
	PrivateAppSettings * d;
	friend class PrivateAppSettings;
};
#endif // _APPSETTINGS_H_

