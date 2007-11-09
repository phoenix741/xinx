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

/* PrivateEditorThread */

class PrivateEditorThread {
public:
	PrivateEditorThread( EditorThread * parent );
	~PrivateEditorThread();
	
	QString m_content;
	QMutex m_contentMutex;
	QWaitCondition m_waitCondition;
private:
	EditorThread * m_parent;
};

/* EditorThread */

EditorThread::EditorThread( QObject * parent ) : XinxThread( parent ) {
	
}

EditorThread::~EditorThread() {
	
}
	
void EditorThread::threadrun() {
	forever {
		m_contentMutex.lock();
		m_waitCondition.wait( &m_contentMutex );
		
		// ICI AJOUTER TRAITEMENT
		
		m_contentMutex.unlock();
	}
}

void EditorThread::reloadEditorContent( const QString & content ) {
	QMutexLocker( &m_contentMutex );
	m_content = content;
	m_waitCondition.wakeAll();
}
