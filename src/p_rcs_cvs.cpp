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

#include "p_rcs_cvs.h"

/* RCS_CVSEntry */

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

PrivateRCS_CVS::PrivateRCS_CVS( RCS_CVS * parent ) {
	m_parent = parent;
	m_watcher = new QFileSystemWatcher( parent );
	m_thread = NULL;
	connect( m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(watcherFileChanged(QString)) );
}

PrivateRCS_CVS::~PrivateRCS_CVS() {
	if( m_thread ) delete m_thread;
	delete m_watcher;
}

RCS::FilesOperation PrivateRCS_CVS::operationsOfRecursivePath( const QString & path ) {
	if( QFileInfo( path ).isDir() ) {
		RCS::FilesOperation files = operationsOfPath( path );
		QStringList infolist = QDir( path ).entryList( QDir::Dirs );
		foreach( QString fileName, infolist ) {
			QString file = QDir( path ).absoluteFilePath ( fileName );
			files += operationsOfRecursivePath( file );
		}
		return files;
	} else {
		RCS::FilesOperation files = operationsOfPath( QFileInfo( path ).absolutePath() );
		foreach( RCS::FileOperation file, files ) {
			if( file.first == path ) 
				return RCS::FilesOperation() << file;
		}
		return RCS::FilesOperation() << qMakePair( path, RCS::Nothing );
	}
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

/*
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
}*/

void PrivateRCS_CVS::callUpdate( const QStringList & path ) {
	if( m_thread ) delete m_thread;
	m_thread = new CVSUpdateThread( this, path );
	m_thread->start();
}

void PrivateRCS_CVS::callCommit( const QStringList & path, const QString & message ) {
	
}

void PrivateRCS_CVS::callAdd( const QStringList & path ) {
	if( m_thread ) delete m_thread;
	m_thread = new CVSAddThread( this, path );
	m_thread->start();
}

void PrivateRCS_CVS::callRemove( const QStringList & path ) {
	if( m_thread ) delete m_thread;
	m_thread = new CVSRemoveThread( this, path );
	m_thread->start();
}

/*
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
*/

/* CVSThread */

CVSThread::CVSThread( PrivateRCS_CVS * parent ) : QThread() {
	qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
	m_privateParent = parent;
	m_parent = m_privateParent->m_parent;
	m_process = NULL;
}

CVSThread::~CVSThread() {
	delete m_process;
}

void CVSThread::processLine( bool error, const QString & line ) {
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

void CVSThread::processReadOutput() {
	m_process->setReadChannel( QProcess::StandardOutput );
	while( m_process->canReadLine() ) 
		processLine( true, m_process->readLine() );
	m_process->setReadChannel( QProcess::StandardError );
	while( m_process->canReadLine() )
		processLine( false, m_process->readLine() );
}

void CVSThread::callCVS( const QString & path, const QStringList & options ) {
	if( ! m_process ) {
		m_process = new QProcess();
		connect( m_process, SIGNAL(readyReadStandardError()), this, SLOT(processReadOutput()) );
		connect( m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processReadOutput()) );
		connect( m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processReadOutput()) );
	}
	emit m_parent->log( RCS::Debug, QString("Working dir : %1").arg( path ) );
	m_process->setWorkingDirectory( path );
	emit m_parent->log( RCS::Debug, QString("%1 %2").arg( global.m_xinxConfig->toolsPath()["cvs"] ).arg( options.join( " " ) ) );
	m_process->start( global.m_xinxConfig->toolsPath()["cvs"], options );
	while( ! m_process->waitForFinished() );
	processReadOutput();
}

void CVSThread::abort() {
#ifdef Q_WS_WIN
	if( GenerateConsoleCtrlEvent( CTRL_BREAK_EVENT, m_process->pid()->dwProcessId ) != 0 )
		perror( "GenerateConsoleCtrlEvent" );
	if( GenerateConsoleCtrlEvent( CTRL_C_EVENT, m_process->pid()->dwProcessId ) != 0 )
		perror( "GenerateConsoleCtrlEvent" );
	if( ! m_process->waitForFinished( 1000 ) ) {
		emit m_parent->log( RCS::Error, tr("I'M A PROCESS KILLER") );
		m_process->kill();
	}
#endif
	m_process->terminate();
}

/* CVSUpdateThread */

CVSUpdateThread::CVSUpdateThread( PrivateRCS_CVS * parent, QStringList paths ) : CVSThread( parent ) {
	m_paths = paths;
}

void CVSUpdateThread::callUpdate( const QString & path, const QStringList & files ) {
	QStringList parameters;
	if( ! global.m_xinxConfig->cvsProgressMessages().isEmpty() )
		parameters << global.m_xinxConfig->cvsProgressMessages();
	parameters << QString("-z%1").arg( global.m_xinxConfig->cvsCompressionLevel() ) << "update";
	if( global.m_xinxConfig->cvsPruneEmptyDirectories() )
		parameters << "-P";
	if( global.m_xinxConfig->cvsCreateDirectories() )
		parameters << "-d";

	parameters << files;

	callCVS( path, parameters );
}

void CVSUpdateThread::run() {
	m_paths.sort();
	
	int i = 0;
	QString path;
	QStringList files;
	
	do {
		files.clear();
		QFileInfo info = QFileInfo( m_paths.at( i ) );
		if( info.isDir() )
			path = m_paths.at( i );
		else {
			path = info.absolutePath();
			files << info.fileName();
		}
		i++;
		QFileInfo infoNext; 
		while( ( i < m_paths.size() ) && ( ( infoNext = QFileInfo( m_paths.at( i ) ) ).absolutePath() == path ) ) {
			files << infoNext.fileName();
			i++;
		}
		callUpdate( path, files );
	} while( i < m_paths.size() );
	
	emit m_parent->log( RCS::Debug, tr("Update terminated") );
	emit m_parent->operationTerminated();
}

/* CVSAddThread */

CVSAddThread::CVSAddThread( PrivateRCS_CVS * parent, QStringList paths ) : CVSThread( parent ) {
	m_paths = paths;
}

void CVSAddThread::callAdd( const QString & path, const QStringList & files ) {
	QStringList parameters;
	parameters << "add";
	parameters << files;
	callCVS( path, parameters );
}

void CVSAddThread::run() {
	m_paths.sort();
	
	int i = 0;
	QString path;
	QStringList files;
	
	do {
		files.clear();
		QFileInfo info = QFileInfo( m_paths.at( i ) );
		if( info.isDir() )
			path = m_paths.at( i );
		else {
			path = info.absolutePath();
			files << info.fileName();
		}
		i++;
		QFileInfo infoNext; 
		while( ( i < m_paths.size() ) && ( ( infoNext = QFileInfo( m_paths.at( i ) ) ).absolutePath() == path ) ) {
			files << infoNext.fileName();
			i++;
		}
		callAdd( path, files );
	} while( i < m_paths.size() );
	
	emit m_parent->log( RCS::Debug, tr("Add terminated") );
	emit m_parent->operationTerminated();
}

/* CVSRemoveThread */

CVSRemoveThread::CVSRemoveThread( PrivateRCS_CVS * parent, QStringList paths ) : CVSThread( parent ) {
	m_paths = paths;
}

void CVSRemoveThread::callRemove( const QString & path, const QStringList & files ) {
	QStringList parameters;
	parameters << "remove";
	parameters << files;
	callCVS( path, parameters );
}

void CVSRemoveThread::run() {
	m_paths.sort();
	
	int i = 0;
	QString path;
	QStringList files;
	
	do {
		files.clear();
		QFileInfo info = QFileInfo( m_paths.at( i ) );
		if( info.isDir() )
			path = m_paths.at( i );
		else {
			path = info.absolutePath();
			files << info.fileName();
		}
		i++;
		QFileInfo infoNext; 
		while( ( i < m_paths.size() ) && ( ( infoNext = QFileInfo( m_paths.at( i ) ) ).absolutePath() == path ) ) {
			files << infoNext.fileName();
			i++;
		}
		callRemove( path, files );
	} while( i < m_paths.size() );
	
	emit m_parent->log( RCS::Debug, tr("Remove terminated") );
	emit m_parent->operationTerminated();
}

