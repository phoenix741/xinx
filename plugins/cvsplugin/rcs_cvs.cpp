/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "rcs_cvs.h"
#include "cvsthread.h"
#include <core/xinxconfig.h>
#include <plugins/xinxpluginsloader.h>

// Qt header
#include <QDir>

/* RCS_CVS */

RCS_CVS::RCS_CVS( const QString & base ) : RCS( base ) {
	m_entriesList = new EntriesList();
	m_watcher     = new QFileSystemWatcher( this );
	m_cvs         = XINXConfig::self()->getTools( "cvs" );
	connect( m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(entriesStateChanged(QString)) );
}

RCS_CVS::~RCS_CVS() {
	delete m_entriesList;
}

RCS::struct_rcs_infos RCS_CVS::infos( const QString & path ) {
	RCS::struct_rcs_infos rcsInfos;
	QString localPath = QDir::fromNativeSeparators( path );
	EntriesLine e    = m_entriesList->status( localPath );
	rcsInfos.state   = e.status( QFileInfo( localPath ).absolutePath() );
	rcsInfos.rcsDate = e.date;
	rcsInfos.version = e.version;
	updateEntries();
	return rcsInfos;
}

void RCS_CVS::searchFileToAddOrRemove( const QStringList & path, QStringList & toAdd, QStringList & toRemove ) {
}

void RCS_CVS::update( const QStringList & path ) {
	Q_ASSERT( ( ! m_process ) || ( m_process->state() == QProcess::NotRunning ) );

	try {
		QStringList parameters;
		if( ! m_settings->config().progressMessages.isEmpty() )
			parameters << m_settings->config().progressMessages;
		parameters << QString("-z%1").arg( m_settings->config().compressionLevel ) << "update";
		if( m_settings->config().pruneEmptyDirectories )
			parameters << "-P";
		if( m_settings->config().createDirectories )
			parameters << "-d";

		callCVS( path, parameters );

		emit log( RCS::LogApplication, tr("Update terminated") );
	} catch( ToolsNotDefinedException e ) {
		emit log( RCS::LogError, e.getMessage() );
	}
}

void RCS_CVS::updateToRevision( const QString & path, const QString & revision, QByteArray * content ) {
	Q_ASSERT( ( ! m_process ) || ( m_process->state() == QProcess::NotRunning ) );

	try {
		QStringList parameters;
		if( ! m_settings->config().progressMessages.isEmpty() )
			parameters << m_settings->config().progressMessages;
		parameters << QString("-z%1").arg( m_settings->config().compressionLevel ) << "update";
		if( content != NULL )
			parameters << "-p";
		parameters << "-r" << revision;

		m_content = content;

		if( m_paths.size() == 1 && QFileInfo( m_paths[ 0 ] ).exists() && !QFileInfo( m_paths[ 0 ] ).isDir() )
			callCVS( QStringList() << path, parameters );

		emit log( RCS::LogApplication, tr("Update to revision %1 terminated").arg( m_revision ) );
	} catch( ToolsNotDefinedException e ) {
		emit log( RCS::LogError, e.getMessage() );
	}
}

void RCS_CVS::commit( const QStringList & path, const QString & message ) {
	Q_ASSERT( ( ! m_process ) || ( m_process->state() == QProcess::NotRunning ) );

	try {
		if( m_thread ) delete m_thread;

		m_thread = new CVSCommitThread( path, message );
		connect( m_thread, SIGNAL(log(RCS::rcsLog,QString)), this, SIGNAL(log(RCS::rcsLog,QString)) );
		connect( m_thread, SIGNAL(operationTerminated()), this, SIGNAL(operationTerminated()) );
		m_thread->start();
	} catch( ToolsNotDefinedException e ) {
		emit log( RCS::LogError, e.getMessage() );
	}
}

void RCS_CVS::add( const QStringList & path ) {
	Q_ASSERT( ( ! m_process ) || ( m_process->state() == QProcess::NotRunning ) );

	try {
		if( m_thread ) delete m_thread;

		m_thread = new CVSAddThread( path );
		connect( m_thread, SIGNAL(log(RCS::rcsLog,QString)), this, SIGNAL(log(RCS::rcsLog,QString)) );
		connect( m_thread, SIGNAL(operationTerminated()), this, SIGNAL(operationTerminated()) );
		m_thread->start();
	} catch( ToolsNotDefinedException e ) {
		emit log( RCS::LogError, e.getMessage() );
	}
}

void RCS_CVS::remove( const QStringList & path ) {
	Q_ASSERT( ( ! m_process ) || ( m_process->state() == QProcess::NotRunning ) );

	try {
		if( m_thread ) delete m_thread;

		m_thread = new CVSRemoveThread( path );
		connect( m_thread, SIGNAL(log(RCS::rcsLog,QString)), this, SIGNAL(log(RCS::rcsLog,QString)) );
		connect( m_thread, SIGNAL(operationTerminated()), this, SIGNAL(operationTerminated()) );
		m_thread->start();
	} catch( ToolsNotDefinedException e ) {
		emit log( RCS::LogError, e.getMessage() );
	}
}

void RCS_CVS::abort() {
	if( ( ! m_process ) || ( m_process->state() == QProcess::NotRunning ) ) return ;

	m_process->terminate();
#ifdef Q_WS_WIN
	if( GenerateConsoleCtrlEvent( CTRL_BREAK_EVENT, m_process->pid()->dwThreadId ) != 0 )
		perror( "GenerateConsoleCtrlEvent" );
	if( GenerateConsoleCtrlEvent( CTRL_C_EVENT, m_process->pid()->dwThreadId ) != 0 )
		perror( "GenerateConsoleCtrlEvent" );
	emit log( RCS::LogError, tr("I'M A PROCESS KILLER") );
	m_process->kill();
#endif
}

void RCS_CVS::entriesStateChanged( const QString & path ) {
	const EntriesFile & e = m_entriesList->value( path );
	foreach( const EntriesLine & l, e ) {
		emit stateChanged( QDir( e.path ).absoluteFilePath( l.filename ) );
	}
}

void RCS_CVS::updateEntries() {
	if( m_watcher->files().size() )
		m_watcher->removePaths( m_watcher->files() );
	foreach( const EntriesFile & e, *m_entriesList ) {
		if( e.size() > 0 )
			m_watcher->addPath( QDir( e.path ).absoluteFilePath( "CVS/Entries" ) );
	}
}

RCS::FilesOperation RCS_CVS::operations( const QStringList & path ) {
	RCS::FilesOperation files;
	foreach( const QString & p, path ) {
		files += recursiveOperationOf( p );
	}
	updateEntries();
	return files;
}

RCS::rcsOperation RCS_CVS::operationOfState( RCS::rcsState state ) {
	RCS::rcsOperation operation;
	switch( state ) {
	case RCS::LocallyModified:
	case RCS::LocallyAdded:
	case RCS::LocallyRemoved:
		operation = RCS::Commit;
		break;
	case RCS::NeedsCheckout:
		operation = RCS::RemoveAndCommit;
		break;
	case RCS::Unknown:
		operation = RCS::AddAndCommit;
		break;
	case RCS::NeedPatch:
	case RCS::UnresolvedConflict:
	case RCS::FileHadConflictsOnMerge:
	case RCS::Updated:
	default:
		operation = RCS::Nothing;
	}
	return operation;
}

RCS::FilesOperation RCS_CVS::operationOf( const QString & path ) {
	RCS::FilesOperation operations;

	QStringList files = QDir( path ).entryList( XinxPluginsLoader::self()->managedFilters(), QDir::Files );
	foreach( const QString & filename, files ) {
		QString filepath = QDir( path ).absoluteFilePath ( filename );
		if( ! QFileInfo( filepath ).isDir() ) {
			RCS::rcsState state = m_entriesList->status( filepath ).status( QFileInfo( filepath ).absolutePath() );
			RCS::rcsOperation operation = operationOfState( state );
			if( operation != RCS::Nothing ) {
				RCS::FileOperation file;
				file.first = filepath;
				file.second = operation;
				operations.append( file );
			}
		}
	}
	EntriesFile file = m_entriesList->value( QDir( path ).absoluteFilePath( "CVS/Entries" ) );
	foreach( const EntriesLine & e, file ) {
		RCS::rcsState state = e.status( path );
		if( ( state != RCS::NeedsCheckout ) && ( state != RCS::LocallyRemoved ) ) continue; // Géré au dessus
		RCS::rcsOperation operation = operationOfState( state );
		if( operation != RCS::Nothing ) {
			RCS::FileOperation op;
			op.first = QDir( path ).absoluteFilePath( e.filename );
			op.second = operation;
			operations.append( op );
		}
	}
	return operations;
}

RCS::FilesOperation RCS_CVS::recursiveOperationOf( const QString & path ) {
	RCS::FilesOperation operations;
	if( QFileInfo( path ).isDir() ) {
		if( QFileInfo( QDir( path ).absoluteFilePath( "CVS/Entries" ) ).exists() ) {
			operations += operationOf( path );

			QStringList infolist = QDir( path ).entryList( QDir::Dirs | QDir::NoDotAndDotDot );
			foreach( const QString & fileName, infolist )
				if( fileName != "CVS" ) {
					QString file = QDir( path ).absoluteFilePath ( fileName );
					operations += recursiveOperationOf( file );
				}
		}
	} else {
		RCS::FileOperation file;
		RCS::rcsState state = m_entriesList->status( path ).status( QFileInfo( path ).absolutePath() );
		RCS::rcsOperation operation = operationOfState( state );
		file.first = path;
		file.second = operation;
		operations.append( file );
	}
	return operations;
}

void RCS_CVS::processLine( const QString & line ) {
	QString lline = line.simplified();

	if( lline.startsWith( "M " ) )
		emit log( RCS::LogLocallyModified, lline );
	else if( lline.startsWith( "A " ) )
		emit log( RCS::LogLocallyModified, lline );
	else if( lline.startsWith( "R " ) )
		emit log( RCS::LogLocallyModified, lline );
	else if( lline.startsWith( "U " ) )
		emit log( RCS::LogRemotlyModified, lline );
	else if( lline.startsWith( "P " ) )
		emit log( RCS::LogRemotlyModified, lline );
	else if( lline.startsWith( "C " ) )
		emit log( RCS::LogConflict, lline );
	else if( lline.startsWith( "? " ) )
		emit log( RCS::LogNotManaged, lline );
	else
		emit log( RCS::LogNormal, lline );
}

void RCS_CVS::processReadOutput() {
	if( ! m_content ) {
		m_process->setReadChannel( QProcess::StandardOutput );
		while( m_process->canReadLine() )
			processLine( m_process->readLine() );

		/* Process error */
		m_process->setReadChannel( QProcess::StandardError );
		while( m_process->canReadLine() )
			emit log( RCS::LogError, m_process->readLine().simplified() );
	} else {
		m_process->setReadChannel( QProcess::StandardOutput );
		while( m_process->canReadLine() ) {
			m_content->append( m_process->readLine() );
		}

		/* Process error */
		m_process->setReadChannel( QProcess::StandardError );
		while( m_process->canReadLine() )
			emit log( RCS::LogError, m_process->readLine().simplified() );
	}
}

void RCS_CVS::callCVS( QStringList paths, const QStringList & options ) {
	if( paths.size() <= 0 ) return;
	if( paths.size() > 1 )
		m_paths.sort();

	int i = 0;
	QString path;
	QStringList files;

	do {
		files.clear();
		QFileInfo info = QFileInfo( paths.at( i ) );
		if( info.isDir() ) {
			path = paths.at( i );
		} else {
			path = info.absolutePath();
			files << info.fileName();
		}
		i++;
		QFileInfo infoNext;
		while( ( i < paths.size() ) && ( ( infoNext = QFileInfo( paths.at( i ) ) ).absolutePath() == path ) ) {
			files << infoNext.fileName();
			i++;
		}

		/* Create process */
		QStringList processOptions = options;
		processOptions << files;

		m_process = new QProcess;

		emit log( RCS::LogApplication, QString("Working dir : %1").arg( path ) );
		m_process->setWorkingDirectory( path );

		emit log( RCS::LogApplication, QString("%1 %2").arg( m_cvs ).arg( processOptions.join( " " ) ).simplified() );
		m_process->start( m_cvs, processOptions, QIODevice::ReadWrite | QIODevice::Text );

		while( m_process->state() != QProcess::NotRunning ) {
			if( m_process->waitForReadyRead( 100 ) )
				processReadOutput();
		}
		processReadOutput();

		delete m_process;
	} while( i < paths.size() );
}
