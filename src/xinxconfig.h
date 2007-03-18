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
#include <QTextCharFormat>

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

	XINXConfig();
	
	void save();
	void load();
	
	QString lang() const { return m_lang; };
	void setLang( const QString & value ) { m_lang = value; };
	
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
	
	const QList<struct managedFile> & managedFile() const { return m_managedFileList; };
	const QHash<QString,struct managedStructure> & managedStructure() const { return m_managedStrucureList; };
	
	QString extentions();
	
private:
	QString m_lang;
	bool m_createBackupFile;
	bool m_alertWhenStdFile;
	QString m_xinxProjectPath;
	QString m_objectDescriptionPath;
	
	QList<struct managedFile> m_managedFileList;
	QHash<QString, struct managedStructure> m_managedStrucureList;
};
#endif
