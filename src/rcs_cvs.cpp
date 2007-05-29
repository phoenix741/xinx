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

#include <QFileSystemWatcher>
#include <QHash>
#include <QDateTime>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDir>

/* RCS_CVSEntry */

class RCS_CVSEntry {
public:
	RCS_CVSEntry();
	RCS_CVSEntry( const QString & path );
	
	void setFileName( const QString & filename );
	void setDate( const QString & date );
	void loadFileDate();
	RCS::rcsState status();
	
	QString m_path;
	QString m_fileName;

	QString m_cvsVersion;
	QDateTime m_cvsDate;
	
	QFileInfo m_fileInfo;
	QDateTime m_fileDate;	
};

RCS_CVSEntry::RCS_CVSEntry() {
	m_path = QString();
}

RCS_CVSEntry::RCS_CVSEntry( const QString & path ) {
	m_path = path;
}

void RCS_CVSEntry::setDate( const QString & date ) {
	m_cvsDate = QDateTime::fromString( date ); // , "ddd MMM d hh:mm:ss yyyy"
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
	if( ! m_fileInfo.isDir() ) {
		if( m_cvsDate < m_fileDate )
			return RCS::LocallyModified;
		else
			return RCS::Updated;
	} else
		return RCS::Unknown;
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
	
	void reloadDir( const QString & path );
	void reloadEntriesFile( const QString & path );
	void reloadEntriesFiles();
public slots:
	void directoryChanged ( const QString & path );
	void fileChanged ( const QString & path );
private:
	RCS_CVS * m_parent;
};

PrivateRCS_CVS::PrivateRCS_CVS( RCS_CVS * parent ) {
	m_parent = parent;
	m_watcher = new QFileSystemWatcher( parent );
	connect( m_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)) );
	connect( m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)) );
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

void PrivateRCS_CVS::directoryChanged( const QString & path ) {
}

void PrivateRCS_CVS::fileChanged ( const QString & path ) {
	if( m_entriesFile.contains( path ) )
		reloadEntriesFile( path );
	else {
		if( m_entries.count( path ) == 0 ) 
			reloadDir( QFileInfo( path ).absolutePath() );
		if( m_entries.count( path ) > 0 ) 
			m_entries[ path ].loadFileDate();
	}
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

#include "rcs_cvs.moc"
