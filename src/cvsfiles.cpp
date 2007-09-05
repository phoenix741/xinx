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
#include "cvsfiles.h"

// Qt header
#include <QDir>
#include <QFileSystemWatcher>

/* CVSFileEntry */

CVSFileEntry::CVSFileEntry() : m_fileName( QString() ), m_watcher(0) {
}

CVSFileEntry::CVSFileEntry( const QString & filename, const QString & version ) : m_watcher(0) {
	setFileName( filename );
	setVersion( version );
}

CVSFileEntry::CVSFileEntry( const QString & path, const QString & filename, const QString & version ) : m_watcher(0) {
	setFileName( path, filename );
	setVersion( version );
}

CVSFileEntry::~CVSFileEntry() {
	delete m_watcher;
}


void CVSFileEntry::setFileName( const QString & path, const QString & filename ) {
	m_fileName = QDir( path ).absoluteFilePath( filename );
	setFileName( m_fileName );
}

void CVSFileEntry::setFileName( const QString & filename ) {
	m_fileName = filename;

	delete m_watcher;
	m_watcher = new QFileSystemWatcher( this );
	connect( m_watcher, SIGNAL(fileChanged(QString)), this, SIGNAL(fileChanged()) );
	m_watcher->addPath( m_fileName );

	init();
	getFileDate();
}

void CVSFileEntry::init() {
	m_fileInfo = QFileInfo( m_fileName );
	m_fileDate = QDateTime();
}

void CVSFileEntry::getFileDate() {
	if( ! m_fileInfo.isDir() ) {
		m_fileDate = m_fileInfo.lastModified().toUTC();
		
		/* Clear ms */
		QTime modifiedTime = m_fileDate.time(); 
		modifiedTime = QTime(modifiedTime.hour(), modifiedTime.minute(), modifiedTime.second());
		
		m_fileDate.setTime( modifiedTime );
	}
}

void CVSFileEntry::setCVSFileDate( QString date ) {
	if( ! m_fileInfo.isDir() ) {
		if( date.contains( "+" ) ) {
			m_cvsDate = QDateTime();
			m_hasConflict = true;
		} else {
			m_hasConflict = false;
			m_cvsDate = QDateTime::fromString( date.simplified() ); // , "ddd MMM d hh:mm:ss yyyy"
			m_cvsDate.setTimeSpec( Qt::UTC );
		}
	} else
		m_cvsDate = QDateTime();
}

void CVSFileEntry::setVersion( const QString & version ) {
	m_cvsVersion = version;
}

RCS::rcsState CVSFileEntry::status() {
	if( ! m_fileInfo.exists() ) {
		if( ( m_cvsVersion.size() > 0 ) && ( m_cvsVersion.at( 0 ) == '-' ) )
			return RCS::LocallyRemoved;
		else
			return RCS::NeedsCheckout;
	}
	if( m_cvsVersion == "0" ) return RCS::LocallyAdded;
	if( ! m_fileInfo.isDir() ) {
		if( m_hasConflict )
			return RCS::FileHadConflictsOnMerge;
		else if( m_cvsDate.isNull() || ( ( m_cvsDate != m_fileDate ) && ( m_cvsDate != m_fileDate.addSecs( -3600 ) ) ) )  /// \todo Find a better way to correct this line.
//		else if( m_cvsDate.isNull() || ( m_cvsDate != m_fileDate ) )  
			return RCS::LocallyModified;
		else		
			return RCS::Updated;
	} else
		return RCS::Updated;
}


