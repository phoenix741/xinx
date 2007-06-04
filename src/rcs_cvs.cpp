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

#include "rcs_cvs.h"
#include "xinxconfig.h"

#include <QFileSystemWatcher>
#include <QHash>
#include <QDateTime>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QProcess>

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

RCS_CVSEntry::RCS_CVSEntry() {
	m_path = QString();
}

RCS_CVSEntry::RCS_CVSEntry( const QString & path ) {
	m_path = path;
}

void RCS_CVSEntry::setDate( QString date ) {
	if( date.contains( "+conflict" ) )
		m_hasConflict = true;
	else
		m_hasConflict = false;
	
	m_cvsDate = QDateTime::fromString( date.remove( "+conflict" ) ); // , "ddd MMM d hh:mm:ss yyyy"
	m_cvsDate.setTimeSpec( Qt::UTC );
}

void RCS_CVSEntry::setFileName( const QString & filename ) {
	m_fileName = filename;
	loadFileDate();
}

void RCS_CVSEntry::loadFileDate() {
	m_fileInfo = QFileInfo( QDir( m_path ).absoluteFilePath( m_fileName ));
	m_fileDate = QDateTime();
	if( ! m_fileInfo.isDir() ) {
		m_fileDate = m_fileInfo.lastModified().toUTC();
		
		/* Clear ms */
		QTime modifiedTime = m_fileDate.time(); 
		modifiedTime = QTime(modifiedTime.hour(), modifiedTime.minute(), modifiedTime.second());
		
		m_fileDate.setTime( modifiedTime );
	}
}

RCS::rcsState RCS_CVSEntry::status() {
	if( ! m_fileInfo.exists() ) return RCS::LocallyRemoved;
	if( m_cvsVersion == "0" ) return RCS::LocallyAdded;
	if( ! m_fileInfo.isDir() ) {
		if( m_cvsDate.isNull() || ( m_cvsDate < m_fileDate ) )
			return RCS::LocallyModified;
		else if( m_hasConflict )
			return RCS::FileHadConflictsOnMerge;
		else		
			return RCS::Updated;
	} else
		return RCS::Updated;
}


/* PrivateRCS_CVS */

class PrivateRCS_CVS : QObject {
	Q_OBJECT
public:
	PrivateRCS_CVS( RCS_CVS * parent );
	~PrivateRCS_CVS();

	QFileSystemWatcher * m_watcher;
	QStringList m_entriesFile;
	QHash<QString,RCS_CVSEntry> m_entries;
	QProcess * m_process;
	
	void reloadDir( const QString & path );
	void reloadEntriesFile( const QString & path );
	void reloadEntriesFiles();
	
	void callUpdate( const QString & path );
public slots:
	void watcherFileChanged ( const QString & path );
	
	void processUpdateReadyReadStandardError();
	void processUpdateReadyReadStandardOutput();
	void processUpdateFinished( int exitCode, QProcess::ExitStatus exitStatus );
private:
	RCS_CVS * m_parent;
	QString m_updatePath, m_updateContent;
};

PrivateRCS_CVS::PrivateRCS_CVS( RCS_CVS * parent ) {
	m_parent = parent;
	m_watcher = new QFileSystemWatcher( parent );
	m_process = NULL;
	connect( m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(watcherFileChanged(QString)) );
}

PrivateRCS_CVS::~PrivateRCS_CVS() {
	delete m_watcher;
}

void PrivateRCS_CVS::reloadEntriesFile( const QString & entries ) {
	if( ! m_entriesFile.contains( entries ) )
		m_entriesFile << entries;
	m_watcher->addPath( entries );
		
	QDir entriesPath = QFileInfo( entries ).dir();
	entriesPath.cdUp();
	QString path = entriesPath.absolutePath();
		
	QFile entriesFile( entries );
	entriesFile.open( QIODevice::ReadOnly | QIODevice::Text );
	QTextStream entriesTextStream( &entriesFile );
	QString entryLine;
	
	while( ! ( entryLine = entriesTextStream.readLine() ).isNull() ) {
		QStringList entryField = entryLine.split( "/" );
		
		if( entryField.size() >= 4 ) {
			RCS_CVSEntry cvsEntry( path );
			cvsEntry.setFileName( entryField[1] );
			cvsEntry.m_cvsVersion  = entryField[2];
			cvsEntry.setDate( entryField[3] );
		
			QString entryPath = QDir( path ).absoluteFilePath( cvsEntry.m_fileName );
			m_watcher->addPath( entryPath ); // Watch change on each file
			m_entries[ entryPath ] = cvsEntry;  // Cache information
		}
	}
}

void PrivateRCS_CVS::reloadDir( const QString & dir ) {
	// Read Entries file
	QString entries = QDir( dir ).absoluteFilePath( "CVS/Entries" );
	if( ! QFileInfo( entries ).exists() ) 
		entries = QDir( dir ).absoluteFilePath( ".cvs/Entries" );
	
	reloadEntriesFile( entries );
}

void PrivateRCS_CVS::reloadEntriesFiles() {
	m_entries.clear();
	foreach( QString file, m_entriesFile )
		reloadEntriesFile( file );
}

void PrivateRCS_CVS::watcherFileChanged ( const QString & path ) {
	if( m_entriesFile.contains( path ) )
		reloadEntriesFile( path );
	else {
		if( m_entries.count( path ) == 0 ) 
			reloadDir( QFileInfo( path ).absolutePath() );
		if( m_entries.count( path ) > 0 ) 
			m_entries[ path ].loadFileDate();
	}
}

void PrivateRCS_CVS::processUpdateReadyReadStandardError() {
	m_process->setReadChannel( QProcess::StandardError );
	while( m_process->canReadLine() ) {
		QString line = m_process->readLine().simplified();
		emit m_parent->log( RCS::Error, line );
	}
}

void PrivateRCS_CVS::processUpdateReadyReadStandardOutput() {
	m_process->setReadChannel( QProcess::StandardOutput );
	while( m_process->canReadLine() ) {
		QString line = m_process->readLine().simplified();
		emit m_parent->log( RCS::Information, line );
	}
}

void PrivateRCS_CVS::processUpdateFinished( int exitCode, QProcess::ExitStatus exitStatus ) {
	m_process->disconnect();
	emit m_parent->updateTerminated();
}

void PrivateRCS_CVS::callUpdate( const QString & path ) {
	m_updatePath = path;
	m_updateContent = "";
	if( ! m_process ) m_process = new QProcess( this );
	connect( m_process, SIGNAL(readyReadStandardError()), this, SLOT(processUpdateReadyReadStandardError()) );
	connect( m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processUpdateReadyReadStandardOutput()) );
	connect( m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processUpdateFinished(int,QProcess::ExitStatus)) );
	m_process->setWorkingDirectory( m_updatePath );
	m_process->start( xinxConfig->toolsPath()["cvs"], QStringList() << "-z9" << "update" );
}

/* RCS_CVS */

RCS_CVS::RCS_CVS() {
	d = new PrivateRCS_CVS( this );
	
	connect( d->m_watcher, SIGNAL(fileChanged(QString)), this, SIGNAL(stateChanged(QString)) );
}

RCS_CVS::~RCS_CVS() {
	delete d;
}

RCS::rcsState RCS_CVS::status( const QString & path ) {
	QString localPath = QDir::fromNativeSeparators( path );
	if( d->m_entries.count( localPath ) == 0 ) 
		d->reloadDir( QFileInfo( localPath ).absolutePath() );
		
	if( d->m_entries.count( localPath ) == 0 ) 
		return RCS::Unknown;
	else
		return d->m_entries[ localPath ].status();
}

void RCS_CVS::update( const QString & path ) {
	// TODO : Update	
	if( d->m_process && ( d->m_process->state() != QProcess::NotRunning )  ) {
		throw ProcessExecutedException();
	} else
		d->callUpdate( path );
}

#include "rcs_cvs.moc"
