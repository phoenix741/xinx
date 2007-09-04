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

/* RCS_CVS */

RCS_CVS::RCS_CVS() {
	d = new PrivateRCS_CVS( this );
	
	connect( d->m_watcher, SIGNAL(fileChanged(QString)), this, SIGNAL(stateChanged(QString)) );
}

RCS_CVS::~RCS_CVS() {
	delete d;
}

RCS::FilesOperation RCS_CVS::operations( const QStringList & path ) {
	return d->operationsOfRecursivePath( path );
}

QVariant RCS_CVS::infos( const QString & path, enum RCS::rcsInfos info ) {
	QString localPath = QDir::fromNativeSeparators( path );
	if( d->m_entries.count( localPath ) == 0 ) 
		d->reloadDir( QFileInfo( localPath ).absolutePath() );
		
	if( d->m_entries.count( localPath ) > 0 ) {
		if( info == RCS::rcsVersions ) {
			return d->m_entries[ localPath ].m_cvsVersion;
		} else
		if( info == RCS::rcsDate ) {
			return d->m_entries[ localPath ].m_cvsDate;
		} else
		if( info == RCS::rcsFileDate ) {
			return d->m_entries[ localPath ].m_fileDate;
		} 
	}
	return QVariant();
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

void RCS_CVS::update( const QStringList & path ) {
	if( d->m_thread && d->m_thread->isRunning() ) {
		throw ProcessExecutedException();
	} else
		d->callUpdate( path );
}

void RCS_CVS::commit( const FilesOperation & path, const QString & message ) {
	if( d->m_thread && d->m_thread->isRunning() ) {
		throw ProcessExecutedException();
	} else
		d->callCommit( path, message );
}

void RCS_CVS::add( const QStringList & path ) {
	if( d->m_thread && d->m_thread->isRunning() ) {
		throw ProcessExecutedException();
	} else
		d->callAdd( path );
}

void RCS_CVS::remove( const QStringList & path ) {
	if( d->m_thread && d->m_thread->isRunning() ) {
		throw ProcessExecutedException();
	} else
		d->callRemove( path );
}

void RCS_CVS::abort() {
	if( d->m_thread )
		qobject_cast<CVSThread*>(d->m_thread)->abort();
}

void RCS_CVS::updateToRevision( const QString & path, const QString & revision, QString * content ) {
	if( d->m_thread && d->m_thread->isRunning() ) {
		throw ProcessExecutedException();
	} else
		d->callUpdateToRevision( path, revision, content );
}
