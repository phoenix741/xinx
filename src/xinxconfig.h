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
#include <QByteArray>

class PrivateXINXConfig;
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
	~XINXConfig();
	
	void save();
	void load();
	
	QString lang() const;
	void setLang( const QString & value );
	
	bool saveSessionByDefault() const;
	void setSaveSessionByDefault( bool value );

	QByteArray storedMainWindowState();
	void storeMainWindowState( QByteArray state );
	
	QPoint position() const;
	void setPosition( QPoint value );
	
	QSize size() const;
	void setSize( QSize value );
	
	bool isCreateBackupFile() const;
	void setCreateBackupFile( bool value );
	
	bool isAlertWhenStdFile() const;
	void setAlertWhenStdFile( bool value );
	
	bool popupWhenFileModified() const;
	void setPopupWhenFileModified( bool value );
	
	QString xinxProjectPath() const;
	void setXinxProjectPath( const QString & value );
	
	QString objectDescriptionPath() const;
	void setObjectDescriptionPath( const QString & value );
	
	QString completionFilesPath() const;
	void setCompletionFilesPath( const QString & value );
	
	QStringList & recentProjectFiles();
	QList<struct managedFile> & managedFile();
	QHash<QString,struct managedStructure> & managedStructure();
	
	QStringList dialogFilters();
	QString dialogFilter( QString ext );
	struct managedFile managedFile4Name( QString filename );
	struct managedFile managedFile4Suffix( QString suffix );
	
	QHash<QString,QString> & toolsPath();
	QString cvsProgressMessages();
	void setCVSProgressMessages( QString value );
	int cvsCompressionLevel();
	void setCVSCompressionLevel( int value );
	bool cvsPruneEmptyDirectories();
	void setCVSPruneEmptyDirectories( bool value );
	bool cvsCreateDirectories();
	void setCVSCreateDirectories( bool value );
	
private:
	PrivateXINXConfig * d;
	friend class PrivateXINXConfig;
};

#endif
