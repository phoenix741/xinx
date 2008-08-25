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

#ifndef _SEARCHFILETHREAD_H_
#define _SEARCHFILETHREAD_H_

// Xinx header
#include <abstracteditor.h>
#include <xinxthread.h>

class SearchFileThread : public XinxThread {
	Q_OBJECT
public:
	SearchFileThread( QObject * parent = 0 );
	virtual ~SearchFileThread();

	void setPath( const QString & path );
	void setSearchString( const QString & from, const QString & to, const AbstractEditor::SearchOptions & options );

	void search();
signals:
	void find( const QString & filename, const QString & lineText, int lineNumber );
	void test( const QString & filename );
	void end();
protected:
	virtual void threadrun();
private:
	void searchRecursive( const QString & path );
	void testFile( const QString & path );

	QString m_from, m_to, m_path;
	AbstractEditor::SearchOptions m_options;
};

#endif // _SEARCHFILETHREAD_H_
