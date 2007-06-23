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
#include <QApplication>

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
	if( ! m_fileInfo.isDir() ) {
		if( date.contains( "+conflict" ) )
			m_hasConflict = true;
		else
			m_hasConflict = false;
	
		m_cvsDate = QDateTime::fromString( date.remove( "+conflict" ) ); // , "ddd MMM d hh:mm:ss yyyy"
		m_cvsDate.setTimeSpec( Qt::UTC );
	} else
		m_cvsDate = QDateTime();
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
	
	void callCVS( const QString & path, const QStringList & options );
	void callUpdate( const QString & path );
	
//	void callUpdate( const QString & path );
	void callCommit( const QString & path, const QString & message );
	void callAdd( const QString & path );
	void callRemove( const QString & path );

	RCS::FilesOperation operationsOfPath( const QString & path );
	RCS::FilesOperation operationsOfRecursivePath( const QString & path );
	QHash<QString,RCS_CVSEntry> loadEntryFile( const QString & entry );
	QHash<QString,RCS_CVSEntry> loadEntryDir( const QString & dir );
public slots:
	void watcherFileChanged ( const QString & path );
	
	void processUpdateReadyReadStandardOutput();
	void processFinished( int exitCode, QProcess::ExitStatus exitStatus );
	void processUpdateFinished( int exitCode, QProcess::ExitStatus exitStatus );
	void processCommitFinished( int exitCode, QProcess::ExitStatus exitStatus );
	void processAddFinished( int exitCode, QProcess::ExitStatus exitStatus );
	void processRemoveFinished( int exitCode, QProcess::ExitStatus exitStatus );
private:
	void processLine( bool error, const QString & line );

	RCS_CVS * m_parent;
	QString m_updatePath;
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

RCS::FilesOperation PrivateRCS_CVS::operationsOfRecursivePath( const QString & path ) {
	RCS::FilesOperation files = operationsOfPath( path );
	QStringList infolist = QDir( path ).entryList( QDir::Dirs );
	foreach( QString fileName, infolist ) {
		QString file = QDir( path ).absoluteFilePath ( fileName );
		files += operationsOfRecursivePath( file );
	}
	return files;
}

RCS::FilesOperation PrivateRCS_CVS::operationsOfPath( const QString & path ) {
	QHash<QString,RCS_CVSEntry> cvsEntries = loadEntryDir( path );
	RCS::FilesOperation files;
	foreach( QString path, cvsEntries.keys() ) {
		RCS::rcsState status = cvsEntries[path].status();
		if( status != RCS::Updated ) {
			RCS::FileOperation file;
			file.first = path;
			if( status == RCS::LocallyRemoved )
				file.second = RCS::RemoveAndCommit;
			else
				file.second = RCS::Commit;
			files.append( file );
		}
	}
	QStringList existFile =	QDir( path ).entryList();
	foreach( QString fileName, existFile ) {
		QString file = QDir( path ).absoluteFilePath ( fileName );
		if( cvsEntries.keys().indexOf( file ) == 0 ) {
			RCS::FileOperation file;
			file.first = path;
			file.second = RCS::AddAndCommit;
			files.append( file );
		}
			
	}
	return files;
}


QHash<QString,RCS_CVSEntry> PrivateRCS_CVS::loadEntryFile( const QString & entry ) {
	QDir entriesPath = QFileInfo( entry ).dir();
	entriesPath.cdUp();
	QString path = entriesPath.absolutePath();
		
	QFile entriesFile( entry );
	entriesFile.open( QIODevice::ReadOnly | QIODevice::Text );
	QTextStream entriesTextStream( &entriesFile );
	QString entryLine;
	
	QHash<QString,RCS_CVSEntry> result;
	
	while( ! ( entryLine = entriesTextStream.readLine() ).isNull() ) {
		QStringList entryField = entryLine.split( "/" );
		
		if( entryField.size() >= 4 ) {
			RCS_CVSEntry cvsEntry( path );
			cvsEntry.setFileName( entryField[1] );
			cvsEntry.m_cvsVersion  = entryField[2];
			cvsEntry.setDate( entryField[3] );
		
			QString entryPath = QDir( path ).absoluteFilePath( cvsEntry.m_fileName );
			result[ entryPath ] = cvsEntry;  
		}
	}
	return result;
}

QHash<QString,RCS_CVSEntry> PrivateRCS_CVS::loadEntryDir( const QString & dir ) {
	// Read Entries file
	QString entries = QDir( dir ).absoluteFilePath( "CVS/Entries" );
	if( ! QFileInfo( entries ).exists() ) 
		entries = QDir( dir ).absoluteFilePath( ".cvs/Entries" );
	
	return loadEntryFile( entries );
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

void PrivateRCS_CVS::processLine( bool error, const QString & line ) {
	QString lline = line.simplified();
	if( lline.startsWith( "? " ) )
		emit m_parent->log( RCS::Warning, lline );
	else if( lline.startsWith( "M " ) )
		emit m_parent->log( RCS::Information, lline );
	else if( lline.startsWith( "U " ) )
		emit m_parent->log( RCS::Information, lline );
	else if( lline.startsWith( "P " ) )
		emit m_parent->log( RCS::Information, lline );
 	else if( lline.startsWith( "cvs" ) ) 
		emit m_parent->log( RCS::Debug, lline );
	else if( error )
		emit m_parent->log( RCS::Error, lline );
	else 
		emit m_parent->log( RCS::Information, lline );
}

void PrivateRCS_CVS::processUpdateReadyReadStandardOutput() {
	m_process->setReadChannel( QProcess::StandardOutput );
	while( m_process->canReadLine() ) 
		processLine( true, m_process->readLine() );
	m_process->setReadChannel( QProcess::StandardError );
	while( m_process->canReadLine() )
		processLine( false, m_process->readLine() );
}

void PrivateRCS_CVS::processFinished( int exitCode, QProcess::ExitStatus exitStatus ) {
	Q_UNUSED( exitCode );
	Q_UNUSED( exitStatus );
	QString reste = m_process->readAllStandardError();
	if( ! reste.isEmpty() ) 
		emit m_parent->log( RCS::Error, reste );
	reste = m_process->readAllStandardOutput();
	if( ! reste.isEmpty() ) 
		emit m_parent->log( RCS::Information, reste );
}

void PrivateRCS_CVS::callCVS( const QString & path, const QStringList & options ) {
	m_updatePath = path;
	if( ! m_process ) m_process = new QProcess( this );
	connect( m_process, SIGNAL(readyReadStandardError()), this, SLOT(processUpdateReadyReadStandardOutput()) );
	connect( m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processUpdateReadyReadStandardOutput()) );
	connect( m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)) );
	m_process->setWorkingDirectory( m_updatePath );
	m_process->start( global.m_xinxConfig->toolsPath()["cvs"], options );
	while( ! m_process->waitForFinished() ) {
		qApp->processEvents();
	}
}

void PrivateRCS_CVS::callUpdate( const QString & path ) {
	QStringList parameters;
	if( ! global.m_xinxConfig->cvsProgressMessages().isEmpty() )
		parameters << global.m_xinxConfig->cvsProgressMessages();
	parameters << QString("-z%1").arg( global.m_xinxConfig->cvsCompressionLevel() ) << "update";
	if( global.m_xinxConfig->cvsPruneEmptyDirectories() )
		parameters << "-P";
	if( global.m_xinxConfig->cvsCreateDirectories() )
		parameters << "-d";

	callCVS( path, parameters );
}

void PrivateRCS_CVS::processUpdateFinished( int exitCode, QProcess::ExitStatus exitStatus ) {
	Q_UNUSED( exitCode );
	Q_UNUSED( exitStatus );
	QString reste = m_process->readAllStandardError();
	if( ! reste.isEmpty() ) 
		emit m_parent->log( RCS::Error, reste );
	reste = m_process->readAllStandardOutput();
	if( ! reste.isEmpty() ) 
		emit m_parent->log( RCS::Information, reste );

	emit m_parent->log( RCS::Debug, tr("Update terminated") );
	
	m_process->disconnect();
	emit m_parent->updateTerminated();
}

void PrivateRCS_CVS::processCommitFinished( int exitCode, QProcess::ExitStatus exitStatus ) {
	Q_UNUSED( exitCode );
	Q_UNUSED( exitStatus );
	QString reste = m_process->readAllStandardError();
	if( ! reste.isEmpty() ) 
		emit m_parent->log( RCS::Error, reste );
	reste = m_process->readAllStandardOutput();
	if( ! reste.isEmpty() ) 
		emit m_parent->log( RCS::Information, reste );

	emit m_parent->log( RCS::Debug, tr("Commit terminated") );

	m_process->disconnect();
	emit m_parent->commitTerminated();
}

void PrivateRCS_CVS::processAddFinished( int exitCode, QProcess::ExitStatus exitStatus ) {
	Q_UNUSED( exitCode );
	Q_UNUSED( exitStatus );
	QString reste = m_process->readAllStandardError();
	if( ! reste.isEmpty() ) 
		emit m_parent->log( RCS::Error, reste );
	reste = m_process->readAllStandardOutput();
	if( ! reste.isEmpty() ) 
		emit m_parent->log( RCS::Information, reste );

	emit m_parent->log( RCS::Debug, tr("Add terminated") );

	m_process->disconnect();
	emit m_parent->addTerminated();
}

void PrivateRCS_CVS::processRemoveFinished( int exitCode, QProcess::ExitStatus exitStatus ) {
	Q_UNUSED( exitCode );
	Q_UNUSED( exitStatus );
	QString reste = m_process->readAllStandardError();
	if( ! reste.isEmpty() ) 
		emit m_parent->log( RCS::Error, reste );
	reste = m_process->readAllStandardOutput();
	if( ! reste.isEmpty() ) 
		emit m_parent->log( RCS::Information, reste );

	emit m_parent->log( RCS::Debug, tr("Remove terminated") );

	m_process->disconnect();
	emit m_parent->removeTerminated();
}
/*
void PrivateRCS_CVS::callUpdate( const QString & path ) {
	m_updatePath = path;
	if( ! m_process ) m_process = new QProcess( this );
	connect( m_process, SIGNAL(readyReadStandardError()), this, SLOT(processUpdateReadyReadStandardOutput()) );
	connect( m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processUpdateReadyReadStandardOutput()) );
	connect( m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processUpdateFinished(int,QProcess::ExitStatus)) );
	m_process->setWorkingDirectory( m_updatePath );
	QStringList parameters;
	if( ! global.m_xinxConfig->cvsProgressMessages().isEmpty() )
		parameters << global.m_xinxConfig->cvsProgressMessages();
	parameters << QString("-z%1").arg( global.m_xinxConfig->cvsCompressionLevel() ) << "update";
	if( global.m_xinxConfig->cvsPruneEmptyDirectories() )
		parameters << "-P";
	if( global.m_xinxConfig->cvsCreateDirectories() )
		parameters << "-d";
	m_process->start( global.m_xinxConfig->toolsPath()["cvs"], parameters );
}
*/
void PrivateRCS_CVS::callCommit( const QString & path, const QString & message ) {
	if( ! m_process ) m_process = new QProcess( this );
	connect( m_process, SIGNAL(readyReadStandardError()), this, SLOT(processUpdateReadyReadStandardOutput()) );
	connect( m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processUpdateReadyReadStandardOutput()) );
	connect( m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processCommitFinished(int,QProcess::ExitStatus)) );
	m_process->setWorkingDirectory( path );
	QStringList parameters;
	if( ! global.m_xinxConfig->cvsProgressMessages().isEmpty() )
		parameters << global.m_xinxConfig->cvsProgressMessages();
	parameters << QString("-z%1").arg( global.m_xinxConfig->cvsCompressionLevel() ) << "commit" << "-m" << message;
	m_process->start( global.m_xinxConfig->toolsPath()["cvs"], parameters );
}

void PrivateRCS_CVS::callAdd( const QString & path ) {
	if( ! m_process ) m_process = new QProcess( this );
	connect( m_process, SIGNAL(readyReadStandardError()), this, SLOT(processUpdateReadyReadStandardOutput()) );
	connect( m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processUpdateReadyReadStandardOutput()) );
	connect( m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processAddFinished(int,QProcess::ExitStatus)) );
	m_process->setWorkingDirectory( QFileInfo( path ).absolutePath() );
	m_process->start( global.m_xinxConfig->toolsPath()["cvs"], QStringList() << "add" << QFileInfo( path ).fileName() );
}

void PrivateRCS_CVS::callRemove( const QString & path ) {
	if( ! m_process ) m_process = new QProcess( this );
	connect( m_process, SIGNAL(readyReadStandardError()), this, SLOT(processUpdateReadyReadStandardOutput()) );
	connect( m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processUpdateReadyReadStandardOutput()) );
	connect( m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processRemoveFinished(int,QProcess::ExitStatus)) );
	m_process->setWorkingDirectory( QFileInfo( path ).absolutePath() );
	m_process->start( global.m_xinxConfig->toolsPath()["cvs"], QStringList() << "remove" << QFileInfo( path ).fileName() );
}

/* RCS_CVS */

RCS_CVS::RCS_CVS() {
	d = new PrivateRCS_CVS( this );
	
	connect( d->m_watcher, SIGNAL(fileChanged(QString)), this, SIGNAL(stateChanged(QString)) );
}

RCS_CVS::~RCS_CVS() {
	delete d;
}

RCS::FilesOperation RCS_CVS::operations( const QString & path ) {
	return d->operationsOfRecursivePath( path );
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
	if( d->m_process && ( d->m_process->state() != QProcess::NotRunning )  ) {
		throw ProcessExecutedException();
	} else
		d->callUpdate( path );
}

void RCS_CVS::commit( const QString & path, const QString & message ) {
	if( d->m_process && ( d->m_process->state() != QProcess::NotRunning )  ) {
		throw ProcessExecutedException();
	} else
		d->callCommit( path, message );
}

void RCS_CVS::add( const QString & path ) {
	if( d->m_process && ( d->m_process->state() != QProcess::NotRunning )  ) {
		throw ProcessExecutedException();
	} else
		d->callAdd( path );
}

void RCS_CVS::remove( const QString & path ) {
	if( d->m_process && ( d->m_process->state() != QProcess::NotRunning )  ) {
		throw ProcessExecutedException();
	} else
		d->callRemove( path );
}

void RCS_CVS::abort() {
#ifdef Q_WS_WIN
	if( GenerateConsoleCtrlEvent( CTRL_BREAK_EVENT, d->m_process->pid()->dwProcessId ) != 0 )
		perror( "GenerateConsoleCtrlEvent" );
	if( GenerateConsoleCtrlEvent( CTRL_C_EVENT, d->m_process->pid()->dwProcessId ) != 0 )
		perror( "GenerateConsoleCtrlEvent" );
	if( ! d->m_process->waitForFinished( 1000 ) ) {
		emit log( RCS::Error, tr("I'M A PROCESS KILLER") );
		d->m_process->kill();
	}
#endif
	d->m_process->terminate();
}


#include "rcs_cvs.moc"
