/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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
#include "editorthread.h"

// Qt header
#include <QMutex>
#include <QWaitCondition>
#include <QAbstractItemModel>

/* PrivateEditorThread */

class PrivateEditorThread {
public:
	PrivateEditorThread( EditorThread * parent );
	~PrivateEditorThread();
	
	QString m_content;
	mutable QMutex m_contentMutex;
	mutable QWaitCondition m_waitCondition;
	
	FileContentParser * m_element;
private:
	EditorThread * m_parent;
};

PrivateEditorThread::PrivateEditorThread( EditorThread * parent ) : m_parent( parent ) {
	
}

PrivateEditorThread::~PrivateEditorThread() {
	
}

/* EditorThread */

EditorThread::EditorThread( QObject * parent ) : XinxThread( parent ) {
	d = new PrivateEditorThread( this );
	d->m_element = NULL;
}

EditorThread::~EditorThread() {
	delete d;
}

void EditorThread::threadrun() {
	forever {
		d->m_contentMutex.lock();
		d->m_waitCondition.wait( &d->m_contentMutex );
		try {
			d->m_element->loadFromContent( d->m_content );
		} catch( ... ) {
			
		}
		
		d->m_contentMutex.unlock();
	}
}

void EditorThread::reloadEditorContent( const QString & content ) {
	QMutexLocker( &d->m_contentMutex );
	d->m_content = content;
	d->m_waitCondition.wakeAll();
}

FileContentParser* & EditorThread::parser() {
	return d->m_element;
}