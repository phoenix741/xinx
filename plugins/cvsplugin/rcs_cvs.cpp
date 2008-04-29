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

// Xinx header
#include "rcs_cvs.h"
#include "cvsthread.h"
#include "xinxpluginsloader.h"
#include "xinxconfig.h"

// Qt header
#include <QDir>

/* RCS_CVS */

RCS_CVS::RCS_CVS( const QString & base ) : RCS( base ) {
	m_entriesList = new EntriesList();
//	connect( m_entries, SIGNAL(fileChanged(QString)), this, SIGNAL(stateChanged(QString)) );
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
	return rcsInfos;
}

void RCS_CVS::update( const QStringList & path ) {
	Q_ASSERT( !(m_thread && m_thread->isRunning()) );

	try {
		if( m_thread ) delete m_thread; 
		
		m_thread = new CVSUpdateThread( path );
		connect( m_thread, SIGNAL(log(RCS::rcsLog,QString)), this, SIGNAL(log(RCS::rcsLog,QString)) );
		connect( m_thread, SIGNAL(operationTerminated()), this, SIGNAL(operationTerminated()) );
		m_thread->start();
	} catch( ToolsNotDefinedException e ) {
		emit log( RCS::LogError, e.getMessage() );		
	}
}

void RCS_CVS::updateToRevision( const QString & path, const QString & revision, QString * content ) {
	Q_ASSERT( !(m_thread && m_thread->isRunning()) );

	try {
		if( m_thread ) delete m_thread; 

		m_thread = new CVSUpdateRevisionThread( path, revision, content );
		connect( m_thread, SIGNAL(log(RCS::rcsLog,QString)), this, SIGNAL(log(RCS::rcsLog,QString)) );
		connect( m_thread, SIGNAL(operationTerminated()), this, SIGNAL(operationTerminated()) );
		m_thread->start();
	} catch( ToolsNotDefinedException e ) {
		emit log( RCS::LogError, e.getMessage() );		
	}
}

void RCS_CVS::commit( const FilesOperation & path, const QString & message ) {
	Q_ASSERT( !(m_thread && m_thread->isRunning()) );

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
	Q_ASSERT( !(m_thread && m_thread->isRunning()) );

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
	Q_ASSERT( !(m_thread && m_thread->isRunning()) );

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
	if( m_thread )
		m_thread->abort();
}

RCS::FilesOperation RCS_CVS::operations( const QStringList & path ) {
	RCS::FilesOperation files;
	foreach( QString p, path ) {
		files += recursiveOperationOf( p );
	}
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
	
	QStringList files = QDir( path ).entryList( XinxPluginsLoader::self()->defaultProjectFilter(), QDir::Files );
	
	foreach( QString filename, files ) {
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
			op.first = e.filename;
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
			foreach( QString fileName, infolist ) 	
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
