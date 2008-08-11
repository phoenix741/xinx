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

/* SearchFileThread */

SearchFileThread::SearchFileThread( QObject * parent ) : QThread( parent ) {
}

SearchFileThread::~SearchFileThread() {
}

void SearchFileThread::search() {
	start( QThread::IdlePriority );
}

void SearchFileThread::searchRecursive( const QString & path ) {
}

void SearchFileThread::run() {
	searchRecursive( m_path );
}

void SearchFileThread::setPath( const QString & path ) {
	m_path = path;
}

void SearchFileThread::setSearchString( const QString & from, const QString & to, const AbstractEditor::SearchOptions & options ) {
	m_from    = from;
	m_to      = to;
	m_options = options;
}

