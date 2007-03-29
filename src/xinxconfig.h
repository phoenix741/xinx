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
 
#ifndef XINXCONFIG_H
#define XINXCONFIG_H
//

#include <QHash>
#include <QList>
#include <QString>
#include <QTextFormat>
#include <QPoint>
#include <QSize>

class QSettings;

//
class XINXConfig {

public:
	struct managedStructure {
		QHash<QString,QTextCharFormat> color;
		QString example;
	};
	struct managedFile {
		QString extentions;
		QString name;
		QString customPath;
		bool canBeCustomize;
		QString structure;
	};
	enum docks {
		contents = 0x01,
		services = 0x02,
		configuration = 0x04,
		files = 0x08
	};

	XINXConfig();
	~XINXConfig();
	
	void save();
	void load();
	
	QString lang() const { return m_lang; };
	void setLang( const QString & value ) { m_lang = value; };
	
	int visibleDock() { return m_docks; };
	bool isDockSet( enum docks dock ) { return (m_docks & dock) == dock; };
	void setDock( enum docks dock ) { m_docks |= dock; };
	void unsetDock( enum docks dock ) { m_docks &= ~dock; };
	void setDock( enum docks dock, bool unset ) { if( unset ) unsetDock( dock ); else setDock( dock ); };
	
	QPoint position() const { return m_xinxPosition; };
	void setPosition( QPoint value ) { m_xinxPosition = value; };
	
	QSize size() const { return m_xinxSize; };
	void setSize( QSize value ) { m_xinxSize = value; };
	
	bool isCreateBackupFile() const { return m_createBackupFile; };
	void setCreateBackupFile( bool value ) { m_createBackupFile = value; };
	
	bool isAlertWhenStdFile() const { return m_alertWhenStdFile; };
	void setAlertWhenStdFile( bool value ) {  m_alertWhenStdFile = value; };
	
	QString xinxProjectPath() const { return m_xinxProjectPath; };
	void setXinxProjectPath( const QString & value ) { m_xinxProjectPath = value; };
	
	QString objectDescriptionPath() const { return m_objectDescriptionPath; };
	void setObjectDescriptionPath( const QString & value ) { m_objectDescriptionPath = value; };
	
	QString completionFilesPath() const { return m_objectDescriptionPath; };
	void setCompletionFilesPath( const QString & value ) { m_objectDescriptionPath = value; };
	
	QStringList & recentProjectFiles() { return m_recentProjectFiles; };
	QList<struct managedFile> & managedFile() { return m_managedFileList; };
	QHash<QString,struct managedStructure> & managedStructure() { return m_managedStrucureList; };
	
	QString extentions();
	
private:
	QSettings * m_settings;

	QPoint m_xinxPosition;
	QSize m_xinxSize;
	
	int m_docks;

	QString m_lang;
	bool m_createBackupFile;
	bool m_alertWhenStdFile;
	QString m_xinxProjectPath;
	QString m_objectDescriptionPath;
	
	QStringList m_recentProjectFiles;
	
	QList<struct managedFile> m_managedFileList;
	QHash<QString, struct managedStructure> m_managedStrucureList;
	
	void createSettings();
	void deleteSettings();
};

extern XINXConfig * xinxConfig;
#endif
