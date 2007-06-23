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

#include "rcs_cvs.h"
#include "globals.h"
#include "xinxconfig.h"

#ifdef Q_WS_WIN
	#include <windows.h>
#endif

#include <QFileSystemWatcher>
#include <QHash>
#include <QDateTime>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
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
	~PrivateRCS_CVS();

	RCS_CVS * m_parent;
	QThread * m_thread;
	QFileSystemWatcher * m_watcher;
	QStringList m_entriesFile;
	QHash<QString,RCS_CVSEntry> m_entries;
	
	void reloadDir( const QString & path );
	void reloadEntriesFile( const QString & path );
	void reloadEntriesFiles();
	
	void callUpdate( const QString & path );
	void callCommit( const QString & path, const QString & message );
	void callAdd( const QString & path );
	void callRemove( const QString & path );

	RCS::FilesOperation operationsOfPath( const QString & path );
	RCS::FilesOperation operationsOfRecursivePath( const QString & path );
	QHash<QString,RCS_CVSEntry> loadEntryFile( const QString & entry );
	QHash<QString,RCS_CVSEntry> loadEntryDir( const QString & dir );
public slots:
	void watcherFileChanged ( const QString & path );
	/*
	void processUpdateReadyReadStandardOutput();
	void processFinished( int exitCode, QProcess::ExitStatus exitStatus );
	void processUpdateFinished( int exitCode, QProcess::ExitStatus exitStatus );
	void processCommitFinished( int exitCode, QProcess::ExitStatus exitStatus );
	void processAddFinished( int exitCode, QProcess::ExitStatus exitStatus );
	void processRemoveFinished( int exitCode, QProcess::ExitStatus exitStatus );*/

};

/* CVSThread */

class CVSThread : public QThread {
	Q_OBJECT
public:
	CVSThread( PrivateRCS_CVS * parent );
	~CVSThread();
public slots:
	virtual void processReadOutput();
	virtual void processFinished();
	void abort();
protected:
	void callCVS( const QString & path, const QStringList & options );
	void processLine( bool error, const QString & line );
	PrivateRCS_CVS * m_privateParent;
	RCS_CVS * m_parent;
	QProcess * m_process;
};

/* CVSUpdateThread */

class CVSUpdateThread : public CVSThread {
	Q_OBJECT
public:
	CVSUpdateThread( PrivateRCS_CVS * parent, QString path );
public slots:
	virtual void processReadOutput();
	virtual void processFinished();
protected:
	void callUpdate( const QString & path );	
	virtual void run();
private:
	QString m_path;
};

#endif // __P_RCS_CVS_H__
