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
#include "exceptions.h"

// Qt header
#include <QDir>
#include <QFile>
#include <QFileSystemWatcher>
#include <QTextStream>
#include <QStringList>
#include <QTimer>

/* File Watcher */

QFileSystemWatcher * watcher = NULL;

/* CVSFileEntry */

CVSFileEntry::CVSFileEntry() : m_fileName( QString() ) {
	XINX_TRACE( "CVSFileEntry", "()" );
}

CVSFileEntry::CVSFileEntry( const QString & filename, const QString & version ) {
	XINX_TRACE( "CVSFileEntry", QString( "( %1, %2 )" ).arg( filename ).arg( version ) );
	setFileName( filename );
	setVersion( version );
}

CVSFileEntry::CVSFileEntry( const QString & path, const QString & filename, const QString & version ) {
	XINX_TRACE( "CVSFileEntry", QString( "( %1, %2, %3 )" ).arg( path ).arg( filename ).arg( version ) );
	setFileName( path, filename );
	setVersion( version );
}

CVSFileEntry::~CVSFileEntry() {
	XINX_TRACE( "~CVSFileEntry", "()" );
	if( ! m_fileName.isEmpty() )
		watcherInstance()->removePath( m_fileName );
	deleteInstance();
}

void CVSFileEntry::setFileName( const QString & path, const QString & filename ) {
	XINX_TRACE( "CVSFileEntry::setFileName", QString( "( %1, %2 )" ).arg( path ).arg( filename ) );
	m_fileName = QDir( path ).absoluteFilePath( filename );
	setFileName( m_fileName );
}

void CVSFileEntry::setFileName( const QString & filename ) {
	XINX_TRACE( "CVSFileEntry::setFileName", QString( "( %1 )" ).arg( filename ) );
	XINX_ASSERT( ! filename.isEmpty() );
	
	QFileSystemWatcher * watcher = watcherInstance();
	if( ! m_fileName.isEmpty() )
		watcher->removePath( m_fileName );
	m_fileName = filename;

	init(); 
	getFileDate();

	if( m_fileInfo.exists() )
		watcher->addPath( m_fileName );
	connect( watcher, SIGNAL(fileChanged(QString)), this, SLOT(slotFileChanged(QString)) );
}

void CVSFileEntry::slotFileChanged( const QString & file ) {
	XINX_TRACE( "CVSFileEntry::slotFileChanged", QString( "( %1 )" ).arg( file ) );
	if( file == m_fileName ) {
		getFileDate();
		refreshStatus();
	}
}

void CVSFileEntry::init() {
	XINX_TRACE( "CVSFileEntry::init", "()" );

	m_fileInfo = QFileInfo( m_fileName );
	m_fileDate = QDateTime();
}

const QDateTime & CVSFileEntry::getFileDate() {
	XINX_TRACE( "CVSFileEntry::getFileDate", "()" );

	if( ! m_fileInfo.isDir() ) {
		m_fileInfo.refresh();
		m_fileDate = m_fileInfo.lastModified().toUTC();
		
		/* Clear ms */
		QTime modifiedTime = m_fileDate.time(); 
		modifiedTime = QTime(modifiedTime.hour(), modifiedTime.minute(), modifiedTime.second());
		
		m_fileDate.setTime( modifiedTime );
	} else
		m_fileDate = QDateTime();
	
	return m_fileDate;
}

const QString & CVSFileEntry::getVersion() const {
	XINX_TRACE( "CVSFileEntry::getVersion", "()" );
	return m_cvsVersion;
}

const QDateTime & CVSFileEntry::getCVSDate() const {
	XINX_TRACE( "CVSFileEntry::getCVSDate", "()" );
	return m_cvsDate;
}

const QString & CVSFileEntry::getFileName() const {
	XINX_TRACE( "CVSFileEntry::getFileName", "()" );
	return m_fileName;
}


void CVSFileEntry::setCVSFileDate( QString date ) {
	XINX_TRACE( "CVSFileEntry::setCVSFileDate", QString( "( %1 )" ).arg( date ) );
	if( ! m_fileInfo.isDir() ) {
		if( date.contains( "+" ) ) {
			m_cvsDate = QDateTime::fromString( date.section( '+', -1, -1 ).simplified() );
			m_hasConflict = true;
		} else {
			m_hasConflict = false;
			m_cvsDate = QDateTime::fromString( date.simplified() ); // , "ddd MMM d hh:mm:ss yyyy"
			m_cvsDate.setTimeSpec( Qt::UTC );
		}
	} else
		m_cvsDate = QDateTime();
	
	refreshStatus();
}

void CVSFileEntry::setVersion( const QString & version ) {
	XINX_TRACE( "CVSFileEntry::setVersion", QString( "( %1 )" ).arg( version ) );
	m_cvsVersion = version;
}

void CVSFileEntry::refreshStatus() {
	XINX_TRACE( "CVSFileEntry::refreshStatus", "()" );
	RCS::rcsState oldState = m_status;
	
	if( ! m_fileInfo.exists() ) {
		if( ( m_cvsVersion.size() > 0 ) && ( m_cvsVersion.at( 0 ) == '-' ) )
			m_status = RCS::LocallyRemoved;
		else
			m_status = RCS::NeedsCheckout;
	} else if( m_cvsVersion == "0" ) {
		m_status = RCS::LocallyAdded;
	} else if( ! m_fileInfo.isDir() ) {
		if( m_hasConflict )
			m_status = RCS::FileHadConflictsOnMerge;
		else if( m_cvsDate.isNull() || ( ( m_cvsDate != m_fileDate ) && ( m_cvsDate != m_fileDate.addSecs( -3600 ) ) ) )  /// \todo Find a better way to correct this line.
//		else if( m_cvsDate.isNull() || ( m_cvsDate != m_fileDate ) )  
			m_status = RCS::LocallyModified;
		else		
			m_status = RCS::Updated;
	} else
		m_status = RCS::Updated;

	if( m_status != oldState )
		emit fileChanged( m_fileName );
}

RCS::rcsState CVSFileEntry::status() {
	XINX_TRACE( "CVSFileEntry::status", "()" );
	return m_status;
}

QFileSystemWatcher * CVSFileEntry::watcherInstance() {
	XINX_TRACE( "CVSFileEntry::watcherInstance", "()" );
	if( ! watcher )
		watcher = new QFileSystemWatcher();
	return watcher;
}

void CVSFileEntry::deleteInstance() {
	XINX_TRACE( "CVSFileEntry::deleteInstance", "()" );
	if( watcher && ( watcher->files().size() == 0 ) && ( watcher->directories().size() == 0 ) ) {
		delete watcher;
		watcher = NULL;
	}
}

/* CVSFileEntryList */

CVSFileEntryList::CVSFileEntryList( const QString & path ) : m_path( path ) {
	XINX_TRACE( "CVSFileEntryList", QString( "( %1 )" ).arg( path ) );

	XINX_ASSERT( QFileInfo( path ).isDir() );
	XINX_ASSERT( QDir( path ).exists() );
	
	m_entries = QDir( m_path ).absoluteFilePath( "CVS/Entries" );

	loadEntriesFile();
	
	QFileSystemWatcher * instance = CVSFileEntry::watcherInstance();
	instance->addPath( m_entries );
	instance->addPath( m_path );
	connect( instance, SIGNAL(fileChanged(QString)), this, SLOT(entriesChanged(QString)) );
	connect( instance, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)) );
	
	m_refreshTimer = new QTimer( this );
	m_refreshTimer->setSingleShot( true );
	connect( m_refreshTimer, SIGNAL(timeout()), this, SLOT(loadEntriesFile()) );
}

CVSFileEntryList::~CVSFileEntryList() {
	XINX_TRACE( "~CVSFileEntryList", "()" );

	QFileSystemWatcher * instance = CVSFileEntry::watcherInstance();
	instance->removePath( m_entries );
	instance->removePath( m_path );
	qDeleteAll( m_directoryList.values() );
	qDeleteAll( values() );
	CVSFileEntry::deleteInstance();
}

void CVSFileEntryList::entriesChanged( const QString & filename ) {
	XINX_TRACE( "CVSFileEntryList::entriesChanged", QString( "( %1 )" ).arg( filename ) );
	if( filename == m_entries ) {
		m_refreshTimer->start( 1000 );
	}
}

void CVSFileEntryList::directoryChanged( const QString & filename ) {
	XINX_TRACE( "CVSFileEntryList::directoryChanged", QString( "( %1 )" ).arg( filename ) );
	if( filename == m_path ) {
		m_refreshTimer->start( 1000 );
	}
}

CVSFileEntryList * CVSFileEntryList::path( const QString & filename ) {
	XINX_TRACE( "CVSFileEntryList::path", QString( "( %1 )" ).arg( filename ) );

	XINX_ASSERT( QDir( filename ).exists() );
	XINX_ASSERT( filename.contains( m_path ) ); 
	// 1. Quand clique sur bouton filtre

	CVSFileEntryList * list = NULL;
	QString rel = QDir( m_path ).relativeFilePath( filename ), dir = rel;
	if( dir.contains( '/' ) ) 
		dir = dir.left( dir.indexOf( '/' ) );
	if( dir.isEmpty() ) 
		return this;
	
	XINX_ASSERT( dir != "." );
	XINX_ASSERT( dir != ".." );
	/*if( ( dir == "." ) || (dir == ".." ) )
		return NULL;
	else*/
	if( m_directoryList.count( dir ) == 0 ) {
		list = new CVSFileEntryList( QDir( m_path ).absoluteFilePath( dir ) );
		m_directoryList[ dir ] = list; 
		connect( list, SIGNAL(fileChanged(QString)), this, SIGNAL(fileChanged(QString)) );
	} else
		list = m_directoryList[ dir ];
	
	if( dir == rel )
		return list;
	else
		return list->path( filename );
}

CVSFileEntry * CVSFileEntryList::object( const QString & filename ) {
	XINX_TRACE( "CVSFileEntryList::object", QString( "( %1 )" ).arg( filename ) );
	
	XINX_ASSERT( filename.contains( m_path ) );

	// The object can't know if itself is managed by CVS.
	if( filename == m_path ) return NULL; 

	// Split name to search basename in path.
	QFileInfo info = QFileInfo( filename ); 
	QString path = info.absolutePath(), basename = info.fileName();
	
	// To know if there are subdirectories
	QString rel = QDir( m_path ).relativeFilePath( path );
	CVSFileEntryList * list = NULL;
	
	if( ! rel.isEmpty() ) { // If we ask a sub directory
		list = this->path( path );
	} else { // If we ask it-self as directory.
		list = this;
	}
	/*if( ! list ) {
		qDebug() << "object : " << filename << " null because no dir" << endl;
		return NULL;
	} else*/
	XINX_ASSERT( list );

	if( list->count( basename ) == 0 ) {
		return NULL;
	} else {
		return list->value( basename );
	}
}

RCS::rcsState CVSFileEntryList::status( const QString & filename ) {
	XINX_TRACE( "CVSFileEntryList::status", QString( "( %1 )" ).arg( filename ) );

	XINX_ASSERT( filename.contains( m_path ) );

	CVSFileEntry * entry = object( filename );
	if( entry ) {
		return entry->status();
	}
	return RCS::Unknown;
}

void CVSFileEntryList::loadEntriesFile() {
	XINX_TRACE( "CVSFileEntryList::loadEntriesFile", "()" );

	if( ! QFileInfo( m_entries ).exists() ) { 
		qDeleteAll( values() );
		clear();
		return;	
	}
	QStringList keyList = keys();
	
	QFile entriesFile( m_entries );
	if( entriesFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
		QTextStream entriesTextStream( &entriesFile );
		QString entryLine;
	
		while( ! ( entryLine = entriesTextStream.readLine() ).isNull() ) {
			QStringList entryField = entryLine.split( "/" );
		
			if( entryField.size() >= 4 ) {
				if( count( entryField[1] ) == 0 ) {
					CVSFileEntry * cvsEntry = new CVSFileEntry( m_path, entryField[1], entryField[2] );
					cvsEntry->setCVSFileDate( entryField[3] );
					connect( cvsEntry, SIGNAL(fileChanged(QString)), this, SIGNAL(fileChanged(QString)) );
					(*this)[ entryField[1] ] = cvsEntry;  // Cache information
				} else {
					(*this)[ entryField[1] ]->setVersion( entryField[2] );
					(*this)[ entryField[1] ]->setCVSFileDate( entryField[3] );
				}
				keyList.removeAll( entryField[1] );
			}
		}
	}
	foreach( QString key, keyList ) {
		delete value( key );
		remove( key );
	}
	emit fileChanged( m_path );
}

