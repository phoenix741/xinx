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
#include "searchfilethread.h"
#include <xinxpluginsloader.h>

// Qt header
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QApplication>

/* SearchFileThread */

SearchFileThread::SearchFileThread( QObject * parent ) : XinxThread( parent ) {
}

SearchFileThread::~SearchFileThread() {
}

void SearchFileThread::search() {
	if( ! m_to.isNull() )
		QMessageBox::information( qApp->activeWindow(), tr("Not supported"), tr("Replacement on multiple file is not yet supported") );

	start( QThread::IdlePriority );
}

void SearchFileThread::testFile( const QString & path ) {
	emit test( path );

	QFile file( path );
	if( ! file.open( QIODevice::ReadOnly | QIODevice::Text ) ) return;
	int line = 0;
	QString text;
	while( !file.atEnd() ) {
		text = file.readLine(); line ++;

		Qt::CaseSensitivity cs = Qt::CaseInsensitive;
		if( m_options.testFlag( AbstractEditor::MATCH_CASE ) )
			cs = Qt::CaseSensitive;

		QString from = m_from;
		if( m_options.testFlag( AbstractEditor::WHOLE_WORDS ) && ! m_options.testFlag( AbstractEditor::REGULAR_EXPRESSION ) )
			from = "\\b" + m_from + "\\b";

		bool contains;
		if( m_options.testFlag( AbstractEditor::WHOLE_WORDS ) || m_options.testFlag( AbstractEditor::REGULAR_EXPRESSION ) )
			contains = text.contains( QRegExp( from ) );
		else
			contains = text.contains( from, cs );

		if( contains )
			emit find( path, text.trimmed(), line );
	}
}

void SearchFileThread::searchRecursive( const QString & path ) {
	QDir pathObject( path );
	QStringList directory = pathObject.entryList( QDir::Dirs );

	foreach( QString pathName, directory ) {
		if( ( pathName != "." ) && ( pathName != ".." ) )
		searchRecursive( pathObject.absoluteFilePath( pathName ) );
	}

	QStringList files = pathObject.entryList( XinxPluginsLoader::self()->managedFilters(), QDir::Files );

	foreach( QString fileName, files ) {
		testFile( pathObject.absoluteFilePath( fileName ) );
	}
}

void SearchFileThread::threadrun() {
	searchRecursive( m_path );
	emit end();
	this->deleteLater();
}

void SearchFileThread::setPath( const QString & path ) {
	m_path = path;
}

void SearchFileThread::setSearchString( const QString & from, const QString & to, const AbstractEditor::SearchOptions & options ) {
	m_from    = from;
	m_to      = to;
	m_options = options;
}

