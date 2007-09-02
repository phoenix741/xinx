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
 
#ifndef _APPSETTINGS_H_
#define _APPSETTINGS_H_

#include <QSettings>
#include <QHash>
#include <QString>
#include <QTextCharFormat>
#include <QPoint>
#include <QSize>
#include <QByteArray>

class PrivateAppSettings;

struct struct_extentions {
	QString customPath;
	bool canBeSpecifique;
};

struct struct_descriptions {
	QString object;
	QString completion;
};

struct struct_project {
	bool saveWithSessionByDefault;
	QString defaultPath;
	bool alertWhenSavingStandardFile;
	QStringList recentProjectFiles;
	QString defaultProjectPathName;
};

struct struct_cvs {
	QString progressMessages;
	int compressionLevel;
	bool pruneEmptyDirectories;
	bool createDirectories;
};

struct struct_editor {
	bool autoindentOnSaving;
	bool popupWhenFileModified;
	bool createBackupFile;
	int completionLevel;
	int tabulationSize;
	bool showTabulationAndSpace;
	QTextCharFormat defaultFormat;
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
	QHash<QString,QString> tools;
	QHash<QString,struct_extentions> files;
	QHash<QString,QTextCharFormat> formats;
};

class AppSettings {
public:
	AppSettings( const AppSettings & origine );
	AppSettings();
	virtual ~AppSettings();
	
	virtual void save();
	virtual void load();
	virtual void setDefault();

	struct_globals & config();
	
protected:
	virtual struct_globals getDefaultGlobals();
	virtual struct_project getDefaultProject();
	virtual struct_descriptions getDefaultDescriptions();
	virtual struct_editor getDefaultEditor();
	virtual struct_cvs getDefaultCvs();
	virtual struct_extentions getDefaultExtentions();
	

	virtual struct_globals getSettingsGlobals( QSettings * settings, const QString & path, struct_globals defaultValue );
	virtual void setSettingsGlobals( QSettings * settings, const QString & path, struct_globals value );
	virtual struct_project getSettingsProject( QSettings * settings, const QString & path, struct_project defaultValue );
	virtual void setSettingsProject( QSettings * settings, const QString & path, struct_project value );
	virtual struct_descriptions getSettingsDescriptions( QSettings * settings, const QString & path, struct_descriptions defaultValue );
	virtual void setSettingsDescriptions( QSettings * settings, const QString & path, struct_descriptions value );
	virtual struct_editor getSettingsEditor( QSettings * settings, const QString & path, struct_editor defaultValue );
	virtual void setSettingsEditor( QSettings * settings, const QString & path, struct_editor value );
	virtual struct_cvs getSettingsCvs( QSettings * settings, const QString & path, struct_cvs defaultValue );
	virtual void setSettingsCvs( QSettings * settings, const QString & path, struct_cvs value );
	virtual struct_extentions getSettingsExtentions( QSettings * settings, const QString & path, struct_extentions defaultValue );
	virtual void setSettingsExtentions( QSettings * settings, const QString & path, struct_extentions value );

	virtual QTextCharFormat getTextFormatFromSettings( QSettings * settings, const QString & path, QTextCharFormat defaultValue );
	virtual void setTextFormatFromSettings( QSettings * settings, const QString & path, QTextCharFormat value );

	virtual QHash<QString,QString> getSettingsHash_QString( QSettings * settings, const QString & path, QHash<QString,QString> defaultValue );
	virtual void setSettingsHash_QString( QSettings * settings, const QString & path, QHash<QString,QString> value );
	virtual QHash<QString,struct_extentions> getSettingsHash_Extentions( QSettings * settings, const QString & path, QHash<QString,struct_extentions> defaultValue );
	virtual void setSettingsHash_Extentions( QSettings * settings, const QString & path, QHash<QString,struct_extentions> value );
	virtual QHash<QString,QTextCharFormat> getSettingsHash_QTextCharFormat( QSettings * settings, const QString & path, QHash<QString,QTextCharFormat> defaultValue );
	virtual void setSettingsHash_QTextCharFormat( QSettings * settings, const QString & path, QHash<QString,QTextCharFormat> value );
private:
	PrivateAppSettings * d;
	friend class PrivateAppSettings;
};

#endif // _APPSETTINGS_H_
