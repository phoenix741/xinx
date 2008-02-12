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
#include "editor.h"
#include "xslproject.h"

// Qt header
#include <QAction>

/* PrivateEditor */

class PrivateEditor {
public:
	PrivateEditor( Editor * parent );
	virtual ~PrivateEditor();
	
	QAction * m_undoAction,
	        * m_redoAction,
	        * m_cutAction,
	        * m_copyAction,
	        * m_pasteAction;
	
	int m_bookmarkNumber;
private:
	Editor * m_parent;
};

PrivateEditor::PrivateEditor( Editor * parent ) : m_bookmarkNumber( 0 ), m_parent( parent ) {
	m_undoAction = new QAction( QIcon(":/images/undo.png"), Editor::tr("&Undo"), m_parent );
	m_undoAction->setEnabled( false );
	QObject::connect( m_undoAction, SIGNAL(triggered()), m_parent, SLOT(undo()) );
	QObject::connect( m_parent, SIGNAL(undoAvailable(bool)), m_undoAction, SLOT(setEnabled(bool)) );
	m_redoAction = new QAction( QIcon(":/images/redo.png"), Editor::tr("&Redo"), m_parent );
	m_redoAction->setEnabled( false );
	QObject::connect( m_redoAction, SIGNAL(triggered()), m_parent, SLOT(redo()) );
	QObject::connect( m_parent, SIGNAL(redoAvailable(bool)), m_redoAction, SLOT(setEnabled(bool)) );

	m_cutAction = new QAction( QIcon(":/images/editcut.png"), Editor::tr("&Cut"), m_parent );
	m_cutAction->setEnabled( false );
	QObject::connect( m_cutAction, SIGNAL(triggered()), m_parent, SLOT(cut()) );
	QObject::connect( m_parent, SIGNAL(copyAvailable(bool)), m_cutAction, SLOT(setEnabled(bool)) );
	m_copyAction = new QAction( QIcon(":/images/editcopy.png"), Editor::tr("C&opy"), m_parent );
	m_copyAction->setEnabled( false );
	QObject::connect( m_copyAction, SIGNAL(triggered()), m_parent, SLOT(copy()) );
	QObject::connect( m_parent, SIGNAL(copyAvailable(bool)), m_copyAction, SLOT(setEnabled(bool)) );
	m_pasteAction = new QAction( QIcon(":/images/editpaste.png"), Editor::tr("&Paste"), m_parent );
	m_pasteAction->setEnabled( true );
	QObject::connect( m_pasteAction, SIGNAL(triggered()), m_parent, SLOT(paste()) );
	QObject::connect( m_parent, SIGNAL(pasteAvailable(bool)), m_pasteAction, SLOT(setEnabled(bool)) );
}

PrivateEditor::~PrivateEditor() {
	delete m_undoAction;
	delete m_redoAction;
	delete m_cutAction;
	delete m_copyAction;
	delete m_pasteAction;
}

/* Editor */

Editor::Editor( QWidget * parent )  : QFrame( parent ) {
	d = new PrivateEditor( this );
	
	setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	setLineWidth( 2 );
}

Editor::Editor( const Editor & editor ) : QFrame( qobject_cast<QWidget*>( editor.parent() ) ) {
	d = new PrivateEditor( this );

	setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	setLineWidth( 2 );
}

Editor::~Editor() {
	delete d;
}

void Editor::firstBookmark() {
	if( bookmarkCount() > 0 )
		gotoBookmarkAt( 0 );
}

bool Editor::previousBookmark() {
	if( d->m_bookmarkNumber == 0 )
		return false;
	if( d->m_bookmarkNumber >= bookmarkCount() )
		d->m_bookmarkNumber = bookmarkCount();
	gotoBookmarkAt( d->m_bookmarkNumber - 1 );
	return true;
}

bool Editor::nextBookmark() {
	if( d->m_bookmarkNumber == bookmarkCount() - 1 )
		return false;
	if( d->m_bookmarkNumber < 0 )
		d->m_bookmarkNumber = -1;
	gotoBookmarkAt( d->m_bookmarkNumber + 1 );
	return true;
}

void Editor::lastBookmark() {
	if( bookmarkCount() > 0 )
		gotoBookmarkAt( bookmarkCount() - 1 );
}

void Editor::gotoBookmarkAt( int i ) {
	d->m_bookmarkNumber = i;
}

QAction * Editor::undoAction() {
	return d->m_undoAction;
}

QAction * Editor::redoAction() {
	return d->m_redoAction;
}

QAction * Editor::cutAction() {
	return d->m_cutAction;
}

QAction * Editor::copyAction() {
	return d->m_copyAction;
}

QAction * Editor::pasteAction() {
	return d->m_pasteAction;
}

QIcon Editor::icon() const {
	return QIcon( ":/images/typeunknown.png" );
}

void Editor::serialize( XSLProjectSessionEditor * data, bool content ) {
	Q_UNUSED( content );
	data->writeProperty( "ClassName", metaObject()->className() ); // Store the class name
}

void Editor::deserialize( XSLProjectSessionEditor * data ) {
	Q_UNUSED( data );
	// Dont't read the class name, already read.
}

Editor * Editor::deserializeEditor( XSLProjectSessionEditor * data ) {
	QString name = data->readProperty( "ClassName" ).toString();

	int id = QMetaType::type( name.toAscii() );
	if( id != -1 ) {
		void * editorPtr = QMetaType::construct( id );
		if( editorPtr ) {
			Editor * editor = static_cast<Editor*>( editorPtr );
			editor->deserialize( data );
			return editor;
		} else
			return NULL;
	} else
		return NULL;
}

