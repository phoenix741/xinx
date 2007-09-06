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
#include <QFile>
#include <QFileSystemWatcher>
#include <QTextStream>
#include <QStringList>

/* File Watcher */

QFileSystemWatcher * watcher = NULL;

/* CVSFileEntry */

CVSFileEntry::CVSFileEntry() : m_fileName( QString() ) {
}

CVSFileEntry::CVSFileEntry( const QString & filename, const QString & version ) {
	setFileName( filename );
	setVersion( version );
}

CVSFileEntry::CVSFileEntry( const QString & path, const QString & filename, const QString & version ) {
	setFileName( path, filename );
	setVersion( version );
}

CVSFileEntry::~CVSFileEntry() {
	if( ! m_fileName.isEmpty() )
		watcherInstance()->removePath( m_fileName );
	deleteInstance();
}

void CVSFileEntry::setFileName( const QString & path, const QString & filename ) {
	m_fileName = QDir( path ).absoluteFilePath( filename );
	setFileName( m_fileName );
}

void CVSFileEntry::setFileName( const QString & filename ) {
	QFileSystemWatcher * watcher = watcherInstance();
	if( ! m_fileName.isEmpty() )
		watcher->removePath( m_fileName );
	m_fileName = filename;

	init(); 
	getFileDate();

	watcher->addPath( filename );
	connect( watcher, SIGNAL(fileChanged(QString)), this, SLOT(slotFileChanged(QString)) );
}

void CVSFileEntry::slotFileChanged( const QString & file ) {
	if( file == m_fileName ) {
		getFileDate();
		refreshStatus();
	}
}

void CVSFileEntry::init() {
	m_fileInfo = QFileInfo( m_fileName );
	m_fileDate = QDateTime();
}

const QDateTime & CVSFileEntry::getFileDate() {
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
	return m_cvsVersion;
}

const QDateTime & CVSFileEntry::getCVSDate() const {
	return m_cvsDate;
}

const QString & CVSFileEntry::getFileName() const {
	return m_fileName;
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
	refreshStatus();
}

void CVSFileEntry::setVersion( const QString & version ) {
	m_cvsVersion = version;
}

void CVSFileEntry::refreshStatus() {
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
	return m_status;
}

QFileSystemWatcher * CVSFileEntry::watcherInstance() {
	if( ! watcher )
		watcher = new QFileSystemWatcher();
	return watcher;
}

void CVSFileEntry::deleteInstance() {
	if( watcher->files().size() == 0 )
		delete watcher;
}

/* CVSFileEntryList */

CVSFileEntryList::CVSFileEntryList( const QString & path ) : m_path( path ) {
	m_entries = QDir( m_path ).absoluteFilePath( "CVS/Entries" );

	loadEntriesFile();
	QFileSystemWatcher * instance = CVSFileEntry::watcherInstance();
	instance->addPath( m_entries );
	instance->addPath( m_path );
	connect( instance, SIGNAL(fileChanged(QString)), this, SLOT(entriesChanged(QString)) );
	connect( instance, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)) );
}

CVSFileEntryList::~CVSFileEntryList() {
	QFileSystemWatcher * instance = CVSFileEntry::watcherInstance();
	instance->removePath( m_entries );
	instance->removePath( m_path );
	qDeleteAll( m_directoryList.values() );
	qDeleteAll( values() );
	CVSFileEntry::deleteInstance();
}

void CVSFileEntryList::entriesChanged( const QString & filename ) {
	if( filename == m_entries ) {
		loadEntriesFile();
	}
}

void CVSFileEntryList::directoryChanged( const QString & filename ) {
	if( filename == m_path ) {
		loadEntriesFile();
		emit fileChanged( filename );
	}
}

CVSFileEntryList * CVSFileEntryList::path( const QString & filename ) {
	CVSFileEntryList * list = NULL;
	QString rel = QDir( m_path ).relativeFilePath( filename );
	if( rel.contains( '/' ) ) 
		rel = rel.left( rel.indexOf( '/' ) );
	if( m_directoryList.count( rel ) == 0 ) {
		list = new CVSFileEntryList( QDir( m_path ).absoluteFilePath( rel ) );
		m_directoryList[rel] = list; 
		connect( list, SIGNAL(fileChanged(QString)), this, SIGNAL(fileChanged(QString)) );
	} else
		list = m_directoryList[rel];
	return list;
}

CVSFileEntry * CVSFileEntryList::object( const QString & filename ) {
	QString rel = QDir( m_path ).relativeFilePath( filename );
	if( rel.contains('/') ) {
		CVSFileEntryList * list = path( filename );
		return list->object( filename );
	} else if( count( rel ) == 0 ) {
		return NULL;
	} else {
		return (*this)[ rel ];
	}
}

RCS::rcsState CVSFileEntryList::status( const QString & filename ) {
	CVSFileEntry * entry = object( filename );
	if( entry )
		return entry->status();
	return RCS::Unknown;
}

void CVSFileEntryList::loadEntriesFile() {
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

