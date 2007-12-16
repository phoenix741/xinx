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
#include "texteditor.h"
#include "filetypeinterface.h"
#include "private/p_filetypeinterface.h"

/* PrivateFileTypeInterface */

PrivateFileTypeInterface::PrivateFileTypeInterface( FileTypeInterface * parent ) : m_textEdit( 0 ), m_parent( parent ) {
	m_keyTimer.setSingleShot( true );
	m_keyTimer.setInterval( 1000 );
	connect( &m_keyTimer, SIGNAL(timeout()), this, SLOT(keyUpdated()) );
}

PrivateFileTypeInterface::~PrivateFileTypeInterface() {
	
}

void PrivateFileTypeInterface::keyUpdated() {
	emit m_parent->canUpdateModel();
}

void PrivateFileTypeInterface::keyPressEvent( QKeyEvent *e ) {
	if(!e->text().isEmpty()) {
		m_keyTimer.start();
	}
}

/* FileTypeInterface */

FileTypeInterface::FileTypeInterface( TextEditor * parent ) {
	d = new PrivateFileTypeInterface( this );
	d->m_textEdit = parent;

	connect( parent, SIGNAL(execKeyPressEvent(QKeyEvent*)), d, SLOT(keyPressEvent(QKeyEvent*)) );
}

FileTypeInterface::~FileTypeInterface() {
	delete d;
}

TextEditor * FileTypeInterface::textEdit() {
	return d->m_textEdit;
}

