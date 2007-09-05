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

#ifndef __P_RCS_CVS_H__
#define __P_RCS_CVS_H__

// Xinx header
#include "rcs_cvs.h"
#include "globals.h"
#include "xinxconfig.h"

// Qt header
#include <QFileSystemWatcher>
#include <QHash>
#include <QDateTime>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QThread>

/* RCS_CVSEntry */

class RCS_CVSEntry {
public:
	RCS_CVSEntry();
	RCS_CVSEntry( const QString & path );
	
	void setFileName( const QString & filename );
	void setDate( QString date );
	void loadFileDate();
	RCS::rcsState status();
	
	QString m_path;
	QString m_fileName;

	QString m_cvsVersion;
	QDateTime m_cvsDate;
	bool m_hasConflict;
	
	QFileInfo m_fileInfo;
	QDateTime m_fileDate;	
};

/* PrivateRCS_CVS */

class PrivateRCS_CVS : public QObject {
	Q_OBJECT
public:
	PrivateRCS_CVS( RCS_CVS * parent );
	virtual ~PrivateRCS_CVS();

	RCS_CVS * m_parent;
	QThread * m_thread;
	QFileSystemWatcher * m_watcher;
	QStringList m_entriesFile;
	QHash<QString,RCS_CVSEntry> m_entries;
	
	void reloadDir( const QString & path );
	void reloadEntriesFile( const QString & path );
	void reloadEntriesFiles();
	
	void callUpdate( const QStringList & path );
	void callCommit( const RCS::FilesOperation & path, const QString & message );
	void callAdd( const QStringList & path );
	void callRemove( const QStringList & path );
	void callUpdateToRevision( const QString & path, const QString & revision, QString * content );
	
	RCS::FilesOperation operationsOfPath( const QString & path );
	RCS::FilesOperation operationsOfRecursivePath( const QString & path );
	RCS::FilesOperation operationsOfRecursivePath( const QStringList & path );
	QHash<QString,RCS_CVSEntry> loadEntryFile( const QString & entry );
	QHash<QString,RCS_CVSEntry> loadEntryDir( const QString & dir );
public slots:
	void watcherFileChanged ( const QString & path );
};

#endif // __P_RCS_CVS_H__
