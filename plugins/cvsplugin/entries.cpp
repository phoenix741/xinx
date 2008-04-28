/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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
#include "entries.h"

// Qt header
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>

/* EntriesLine */

EntriesLine::EntriesLine() : hasConflict( false ) {
	
}

EntriesLine::EntriesLine( const QString & line ) {
	QStringList entry = line.split('/');
	if( ( entry.size() >= 1 ) && ( entry.at( 0 ).size() >= 1 ) ) type = entry.at( 0 ).at( 0 );
	if( entry.size() >= 2 ) filename = entry.at( 1 );
	if( type != 'D' ) { 
		if( entry.size() >= 3 ) version = entry.at( 2 );
		if( ( entry.size() >= 4 ) && ( ! entry.at( 3 ).isEmpty() ) ) {
			if( entry.at( 3 ).contains( '+' ) ) {
				date = QDateTime::fromString( entry.at( 3 ).section( '+', -1, -1 ).simplified() );// , "ddd MMM d hh:mm:ss yyyy"
				hasConflict = true;
			} else {
				date = QDateTime::fromString( entry.at( 3 ).simplified() );// , "ddd MMM d hh:mm:ss yyyy"
				hasConflict = false;
			}
			date.setTimeSpec( Qt::UTC );
		}
	}
}

RCS::rcsState EntriesLine::status( const QString & path ) const {
	QFileInfo info( QDir( path ).absoluteFilePath( filename ) );
	QDateTime fileDate = info.lastModified().toUTC();
	
	/* Clear ms */
	QTime modifiedTime = fileDate.time(); 
	modifiedTime = QTime( modifiedTime.hour(), modifiedTime.minute(), modifiedTime.second() );
	fileDate.setTime( modifiedTime );
	
	if( ! info.exists() ) {
		if( ( version.size() > 0 ) && ( version.at( 0 ) == '-' ) )
			return RCS::LocallyRemoved;
		return RCS::NeedsCheckout;
	} 

	if( version == "0" ) 
		return RCS::LocallyAdded;
	
	if( info.isDir() ) 
		return RCS::Updated;
	
	if( hasConflict )
		return RCS::FileHadConflictsOnMerge;
//		else if( m_cvsDate.isNull() || ( ( m_cvsDate != m_fileDate ) && ( m_cvsDate != m_fileDate.addSecs( -3600 ) ) && ( m_cvsDate.addSecs( -3600 ) != m_fileDate ) ) )  /// \todo Find a better way to correct this line.
	
	if( date.isNull() || ( date != fileDate ) )  
		return RCS::LocallyModified;

	return RCS::Updated;
}

/* EntriesFile */

EntriesFile::EntriesFile() {
	
}

EntriesFile::EntriesFile( const QString & file ) {
	QDir pathDir = QFileInfo( file ).absoluteDir();
	pathDir.cdUp(); 
	path = pathDir.path();
	fileDate = QFileInfo( file ).lastModified();
	
	QFile entriesFile( file );
	if( entriesFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
		QTextStream entriesTextStream( &entriesFile );
		QString entryLine;
	
		while( ! ( entryLine = entriesTextStream.readLine() ).isNull() ) {
			EntriesLine e( entryLine );
			insert( e.filename, e );
		}
	}
	
}

const EntriesLine EntriesFile::status( const QString & path ) const {
	QString filename = QFileInfo( path ).fileName();
	return value( filename );
}

/* EntriesList */

const EntriesFile EntriesList::value( const QString & key ) {
	return value( key, EntriesFile( key ) );
}

const EntriesFile EntriesList::value( const QString & key, const EntriesFile & defaultValue ) {
	EntriesFile entries = QHash<QString,EntriesFile>::value( key, defaultValue );
	if( entries.fileDate == QFileInfo( key ).lastModified() )
		return entries;
	else { 
		 (*this)[ key ] = EntriesFile( key );
		 return QHash<QString,EntriesFile>::value( key, defaultValue );
	}
}

const EntriesLine EntriesList::status( const QString & filename ) {
	Q_ASSERT( QFileInfo( filename ).isAbsolute() );
	
	QString path = QFileInfo( filename ).absolutePath();
	QString cvsEntries = QDir( path ).absoluteFilePath( "CVS/Entries" );
	EntriesFile file;
	if( contains( cvsEntries ) ) {
		file = value( cvsEntries );
	} else {
		file = EntriesFile( cvsEntries );
		insert( cvsEntries, file );
	}
	return file.status( filename );
}
