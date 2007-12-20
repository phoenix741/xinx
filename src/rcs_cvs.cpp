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
#include "private/p_rcs_cvs.h"
#include "cvsthread.h"
#include "cvsfiles.h"
#include "exceptions.h"

// Qt header
#include <QDir>

/* ProcessExecutedException */

ProcessExecutedException::ProcessExecutedException( QString message ) : XinxException( message ) {
	
}

/* PrivateRCS_CVS */

PrivateRCS_CVS::PrivateRCS_CVS( RCS_CVS * parent ) : m_thread( 0 ), m_parent( parent ) {
	XINX_TRACE( "PrivateRCS_CVS", QString( "( 0x%1 )" ).arg( (unsigned long)parent, 0, 16 ) );

	m_entries = new CVSFileEntryList( m_parent->getBasePath() );
	connect( m_entries, SIGNAL(fileChanged(QString)), m_parent, SIGNAL(stateChanged(QString)) );
}

PrivateRCS_CVS::~PrivateRCS_CVS() {
	XINX_TRACE( "~PrivateRCS_CVS", "()" );
	delete m_entries;
}

void PrivateRCS_CVS::callUpdate( const QStringList & path ) {
	XINX_TRACE( "PrivateRCS_CVS::callUpdate", QString( "( %1 )" ).arg( path.join(";") ) );

	if( m_thread ) delete m_thread;
	m_thread = new CVSUpdateThread( path );
	connect( m_thread, SIGNAL(log(RCS::rcsLog,QString)), m_parent, SIGNAL(log(RCS::rcsLog,QString)) );
	connect( m_thread, SIGNAL(operationTerminated()), m_parent, SIGNAL(operationTerminated()) );
	m_thread->start();
}

void PrivateRCS_CVS::callUpdateToRevision( const QString & path, const QString & revision, QString * content ) {
	XINX_TRACE( "PrivateRCS_CVS::callUpdateToRevision", QString( "( %1, %2, %3 )" ).arg( path ).arg( revision ).arg( (unsigned long)content, 0, 16 ) );

	if( m_thread ) delete m_thread;
	m_thread = new CVSUpdateRevisionThread( path, revision, content );
	connect( m_thread, SIGNAL(log(RCS::rcsLog,QString)), m_parent, SIGNAL(log(RCS::rcsLog,QString)) );
	connect( m_thread, SIGNAL(operationTerminated()), m_parent, SIGNAL(operationTerminated()) );
	m_thread->start();
}

void PrivateRCS_CVS::callCommit( const RCS::FilesOperation & path, const QString & message ) {
	XINX_TRACE( "PrivateRCS_CVS::callCommit", QString( "( path, %1 )" ).arg( message ) );

	if( m_thread ) delete m_thread;
	m_thread = new CVSCommitThread( path, message );
	connect( m_thread, SIGNAL(log(RCS::rcsLog,QString)), m_parent, SIGNAL(log(RCS::rcsLog,QString)) );
	connect( m_thread, SIGNAL(operationTerminated()), m_parent, SIGNAL(operationTerminated()) );
	m_thread->start();
}

void PrivateRCS_CVS::callAdd( const QStringList & path ) {
	XINX_TRACE( "PrivateRCS_CVS::callAdd", QString( "( %1 )" ).arg( path.join(";") ) );

	if( m_thread ) delete m_thread;
	m_thread = new CVSAddThread( path );
	connect( m_thread, SIGNAL(log(RCS::rcsLog,QString)), m_parent, SIGNAL(log(RCS::rcsLog,QString)) );
	connect( m_thread, SIGNAL(operationTerminated()), m_parent, SIGNAL(operationTerminated()) );
	m_thread->start();
}

void PrivateRCS_CVS::callRemove( const QStringList & path ) {
	XINX_TRACE( "PrivateRCS_CVS::callRemove", QString( "( %1 )" ).arg( path.join(";") ) );

	if( m_thread ) delete m_thread;
	m_thread = new CVSRemoveThread( path );
	connect( m_thread, SIGNAL(log(RCS::rcsLog,QString)), m_parent, SIGNAL(log(RCS::rcsLog,QString)) );
	connect( m_thread, SIGNAL(operationTerminated()), m_parent, SIGNAL(operationTerminated()) );
	m_thread->start();
}
	
RCS::rcsOperation PrivateRCS_CVS::operationOfState( RCS::rcsState state ) {
	XINX_TRACE( "PrivateRCS_CVS::operationOfState", QString( "( %1 )" ).arg( (int)state ) );

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

RCS::FilesOperation PrivateRCS_CVS::operationOf( const QString & path ) {
	XINX_TRACE( "PrivateRCS_CVS::operationOf", QString( "( %1 )" ).arg( path ) );

	RCS::FilesOperation operations;
	
	QStringList files = QDir( path ).entryList( DEFAULT_PROJECT_FILTRE, QDir::Files );
	
	foreach( QString filename, files ) {
		QString filepath = QDir( path ).absoluteFilePath ( filename );
		if( ! QFileInfo( filepath ).isDir() ) {
			RCS::rcsState state = m_entries->status( filepath );
			RCS::rcsOperation operation = operationOfState( state ); 
			if( operation != RCS::Nothing ) {
				RCS::FileOperation file;
				file.first = filepath;
				file.second = operation;
				operations.append( file );
			}
		}
	}
	CVSFileEntryList * entrie = m_entries->path( path );
	foreach( CVSFileEntry * file, entrie->values() ) {
		if( file->status() == RCS::NeedsCheckout ) {
			RCS::FileOperation op;
			op.first = file->getFileName();
			op.second = RCS::RemoveAndCommit;
			operations.append( op );
		}
	}
	return operations;
}

RCS::FilesOperation PrivateRCS_CVS::recursiveOperationOf( const QString & path ) {
	XINX_TRACE( "PrivateRCS_CVS::recursiveOperationOf", QString( "( %1 )" ).arg( path ) );

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
		RCS::rcsState state = m_entries->status( path );
		RCS::rcsOperation operation = operationOfState( state ); 
		file.first = path;
		file.second = operation;
		operations.append( file );
	}
	return operations;
}


/* RCS_CVS */

RCS_CVS::RCS_CVS( const QString & base ) : RCS( base ) {
	XINX_TRACE( "RCS_CVS", QString( "( %1 )" ).arg( base ) );

	d = new PrivateRCS_CVS( this );
}

RCS_CVS::~RCS_CVS() {
	XINX_TRACE( "~RCS_CVS", "()" );
	delete d;
}

RCS::FilesOperation RCS_CVS::operations( const QStringList & path ) {
	XINX_TRACE( "RCS_CVS::operations", QString( "( %1 )" ).arg( path.join(";") ) );

	RCS::FilesOperation files;
	foreach( QString p, path ) {
		files += d->recursiveOperationOf( p );
	}
	return files;
}

QVariant RCS_CVS::infos( const QString & path, enum RCS::rcsInfos info ) {
	XINX_TRACE( "RCS_CVS::infos", QString( "( %1, %2 )" ).arg( path ).arg( (int)info ) );

	QString localPath = QDir::fromNativeSeparators( path );
	CVSFileEntry * o = d->m_entries->object( localPath );
	if( o ) {
		if( info == RCS::rcsVersions ) {
			return o->getVersion();
		} else
		if( info == RCS::rcsDate ) {
			return o->getCVSDate();
		} else
		if( info == RCS::rcsFileDate ) {
			return o->getFileDate();
		} 
	}
	return QVariant();
}

RCS::rcsState RCS_CVS::status( const QString & path ) {
	XINX_TRACE( "RCS_CVS::status", QString( "( %1 )" ).arg( path ) );

	QString localPath = QDir::fromNativeSeparators( path );
	return d->m_entries->status( localPath );
}

void RCS_CVS::update( const QStringList & path ) {
	XINX_TRACE( "RCS_CVS::update", QString( "( %1 )" ).arg( path.join(";") ) );

	if( d->m_thread && d->m_thread->isRunning() ) {
		throw ProcessExecutedException( "Can't update" );
	} else
		d->callUpdate( path );
}

void RCS_CVS::commit( const FilesOperation & path, const QString & message ) {
	XINX_TRACE( "RCS_CVS::commit", QString( "( %1 )" ).arg( message ) );

	if( d->m_thread && d->m_thread->isRunning() ) {
		throw ProcessExecutedException( "Can't commit" );
	} else
		d->callCommit( path, message );
}

void RCS_CVS::add( const QStringList & path ) {
	XINX_TRACE( "RCS_CVS::add", QString( "( %1 )" ).arg( path.join(";") ) );

	if( d->m_thread && d->m_thread->isRunning() ) {
		throw ProcessExecutedException( "Can't add" );
	} else
		d->callAdd( path );
}

void RCS_CVS::remove( const QStringList & path ) {
	XINX_TRACE( "RCS_CVS::remove", QString( "( %1 )" ).arg( path.join(";") ) );

	if( d->m_thread && d->m_thread->isRunning() ) {
		throw ProcessExecutedException( "Can't remove" );
	} else
		d->callRemove( path );
}

void RCS_CVS::abort() {
	XINX_TRACE( "RCS_CVS::abort", "()" );

	if( d->m_thread )
		qobject_cast<CVSThread*>(d->m_thread)->abort();
}

void RCS_CVS::updateToRevision( const QString & path, const QString & revision, QString * content ) {
	XINX_TRACE( "RCS_CVS::updateToRevision", QString( "( %1, %2, %3 )" ).arg( path ).arg( revision ).arg( (unsigned long)content ) );

	if( d->m_thread && d->m_thread->isRunning() ) {
		throw ProcessExecutedException( "Can't update to revision" );
	} else
		d->callUpdateToRevision( path, revision, content );
}
