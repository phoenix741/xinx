// This file is auto-generated file, don't modify it unless you want to lose your code
// Generated by qgenconfig v0.1 (c) Ulrich Van Den Hekke

#ifndef _APPSETTINGS_H_
#define _APPSETTINGS_H_

#include <QSettings>
#include <QString>
#include <QStringList>
#include <QFont>
#include <QColor>
#include <QPoint>
#include <QSize>
#include <QByteArray>
#include <QHash>
#include <QHash>
#include <QTextCharFormat>
#include <QHash>

class PrivateAppSettings;

class AppSettings {
public:
	struct struct_configurationEditor {
		bool activateConfigurationEditor;
		bool autoLoadConfigurationFile;
		bool autoSaveConfigurationFile;
	};

	struct struct_editor {
		bool autoindentOnSaving;
		bool popupWhenFileModified;
		bool createBackupFile;
		int completionLevel;
		int tabulationSize;
		bool showTabulationAndSpace;
		bool highlightCurrentLine;
		QFont defaultFormat;
		QColor highlightWord;
		bool autoHighlight;
		bool hideCloseTab;
		bool closeButtonOnEachTab;
	};

	struct struct_cvs {
		QString progressMessages;
		int compressionLevel;
		bool pruneEmptyDirectories;
		bool createDirectories;
		bool createChangelog;
	};

	struct struct_project {
		bool saveWithSessionByDefault;
		QString defaultPath;
		bool alertWhenSavingStandardFile;
		QStringList recentProjectFiles;
		QString defaultProjectPathName;
		bool closeVersionManagementLog;
	};

	struct struct_descriptions {
		QString datas;
	};

	struct struct_extentions {
		QString customPath;
		bool canBeSpecifique;
	};

	struct struct_globals {
		QString language;
		QPoint position;
		QSize size;
		bool maximized;
		QByteArray state;
		struct_descriptions descriptions;
		struct_project project;
		struct_cvs cvs;
		struct_editor editor;
		struct_configurationEditor configurationEditor;
		QHash<QString,QString> tools;
		QHash<QString,struct_extentions> files;
		QHash<QString,QTextCharFormat> formats;
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
	virtual struct_configurationEditor getDefaultConfigurationEditor();
	virtual struct_configurationEditor getSettingsConfigurationEditor( QSettings * settings, const QString & path, struct_configurationEditor defaultValue );
	virtual void setSettingsConfigurationEditor( QSettings * settings, const QString & path, struct_configurationEditor value );

	virtual struct_editor getDefaultEditor();
	virtual struct_editor getSettingsEditor( QSettings * settings, const QString & path, struct_editor defaultValue );
	virtual void setSettingsEditor( QSettings * settings, const QString & path, struct_editor value );

	virtual struct_cvs getDefaultCvs();
	virtual struct_cvs getSettingsCvs( QSettings * settings, const QString & path, struct_cvs defaultValue );
	virtual void setSettingsCvs( QSettings * settings, const QString & path, struct_cvs value );

	virtual struct_project getDefaultProject();
	virtual struct_project getSettingsProject( QSettings * settings, const QString & path, struct_project defaultValue );
	virtual void setSettingsProject( QSettings * settings, const QString & path, struct_project value );

	virtual struct_descriptions getDefaultDescriptions();
	virtual struct_descriptions getSettingsDescriptions( QSettings * settings, const QString & path, struct_descriptions defaultValue );
	virtual void setSettingsDescriptions( QSettings * settings, const QString & path, struct_descriptions value );

	virtual struct_extentions getDefaultExtentions();
	virtual struct_extentions getSettingsExtentions( QSettings * settings, const QString & path, struct_extentions defaultValue );
	virtual void setSettingsExtentions( QSettings * settings, const QString & path, struct_extentions value );

	virtual struct_globals getDefaultGlobals();
	virtual struct_globals getSettingsGlobals( QSettings * settings, const QString & path, struct_globals defaultValue );
	virtual void setSettingsGlobals( QSettings * settings, const QString & path, struct_globals value );

	virtual QHash<QString,QString> getDefaultHash_QString();
	virtual QHash<QString,QString> getSettingsHash_QString( QSettings * settings, const QString & path, QHash<QString,QString> defaultValue );
	virtual void setSettingsHash_QString( QSettings * settings, const QString & path, QHash<QString,QString> value );

	virtual QHash<QString,struct_extentions> getDefaultHash_struct_extentions();
	virtual QHash<QString,struct_extentions> getSettingsHash_struct_extentions( QSettings * settings, const QString & path, QHash<QString,struct_extentions> defaultValue );
	virtual void setSettingsHash_struct_extentions( QSettings * settings, const QString & path, QHash<QString,struct_extentions> value );

	virtual QTextCharFormat getSettingsTextCharFormat( QSettings * settings, const QString & path, QTextCharFormat defaultValue );
	virtual void setSettingsTextCharFormat( QSettings * settings, const QString & path, QTextCharFormat value );

	virtual QHash<QString,QTextCharFormat> getDefaultHash_QTextCharFormat();
	virtual QHash<QString,QTextCharFormat> getSettingsHash_QTextCharFormat( QSettings * settings, const QString & path, QHash<QString,QTextCharFormat> defaultValue );
	virtual void setSettingsHash_QTextCharFormat( QSettings * settings, const QString & path, QHash<QString,QTextCharFormat> value );

private:
	PrivateAppSettings * d;
	friend class PrivateAppSettings;
};
#endif // _APPSETTINGS_H_

