/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "abstracteditor.h"
#include "xslproject.h"
#include "borderlayout.h"

// Qt header
#include <QAction>
#include <QLabel>
#include <QPushButton>

/* AbstractEditor */

AbstractEditor::AbstractEditor( QWidget * parent )  : QFrame( parent ) {
	init();
}

AbstractEditor::AbstractEditor( const AbstractEditor & editor ) : QFrame( qobject_cast<QWidget*>( editor.parent() ) ) {
	init();
}

void AbstractEditor::init() {
	m_undoAction = new QAction( QIcon(":/images/undo.png"), AbstractEditor::tr("&Undo"), this );
	m_undoAction->setEnabled( false );
	QObject::connect( m_undoAction, SIGNAL(triggered()), this, SLOT(undo()) );
	QObject::connect( this, SIGNAL(undoAvailable(bool)), m_undoAction, SLOT(setEnabled(bool)) );
	m_redoAction = new QAction( QIcon(":/images/redo.png"), AbstractEditor::tr("&Redo"), this );
	m_redoAction->setEnabled( false );
	QObject::connect( m_redoAction, SIGNAL(triggered()), this, SLOT(redo()) );
	QObject::connect( this, SIGNAL(redoAvailable(bool)), m_redoAction, SLOT(setEnabled(bool)) );

	m_cutAction = new QAction( QIcon(":/images/editcut.png"), AbstractEditor::tr("&Cut"), this );
	m_cutAction->setEnabled( false );
	QObject::connect( m_cutAction, SIGNAL(triggered()), this, SLOT(cut()) );
	QObject::connect( this, SIGNAL(copyAvailable(bool)), m_cutAction, SLOT(setEnabled(bool)) );
	m_copyAction = new QAction( QIcon(":/images/editcopy.png"), AbstractEditor::tr("C&opy"), this );
	m_copyAction->setEnabled( false );
	QObject::connect( m_copyAction, SIGNAL(triggered()), this, SLOT(copy()) );
	QObject::connect( this, SIGNAL(copyAvailable(bool)), m_copyAction, SLOT(setEnabled(bool)) );
	m_pasteAction = new QAction( QIcon(":/images/editpaste.png"), AbstractEditor::tr("&Paste"), this );
	m_pasteAction->setEnabled( true );
	QObject::connect( m_pasteAction, SIGNAL(triggered()), this, SLOT(paste()) );
	QObject::connect( this, SIGNAL(pasteAvailable(bool)), m_pasteAction, SLOT(setEnabled(bool)) );

	setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	setLineWidth( 2 );

	/* Widget for message */
	m_messageWidget = new QWidget( this );
	m_messageLabel = new QLabel( "erreur text" );
	QPushButton * messageClose = new QPushButton( tr("&Close"), this );

	QHBoxLayout * messageWidgetLayout = new QHBoxLayout( m_messageWidget );
	messageWidgetLayout->addWidget( m_messageLabel );
	messageWidgetLayout->addWidget( messageClose );
	messageWidgetLayout->setMargin( 0 );

	connect( messageClose, SIGNAL(clicked()), m_messageWidget, SLOT(hide()) );

	/* Layouts */
	BorderLayout * grid = new BorderLayout( this, 0, 0 );
	grid->addWidget( m_messageWidget, BorderLayout::South );

	setLayout( grid );

	/* Widget not visible */
	m_messageWidget->hide();
}

AbstractEditor::~AbstractEditor() {
	// Actions deleted as children
}

void AbstractEditor::setMessage( QString message ) {
	m_messageLabel->setText( message );
	if( message.isEmpty() )
		m_messageWidget->hide();
	else
		m_messageWidget->show();
}

BorderLayout * AbstractEditor::borderLayout() {
	return dynamic_cast<BorderLayout*>( layout() );
}

void AbstractEditor::firstBookmark() {
	if( bookmarkCount() > 0 )
		gotoBookmarkAt( 0 );
}

void AbstractEditor::lastBookmark() {
	if( bookmarkCount() > 0 )
		gotoBookmarkAt( bookmarkCount() - 1 );
}

QAction * AbstractEditor::undoAction() {
	return m_undoAction;
}

QAction * AbstractEditor::redoAction() {
	return m_redoAction;
}

QAction * AbstractEditor::cutAction() {
	return m_cutAction;
}

QAction * AbstractEditor::copyAction() {
	return m_copyAction;
}

QAction * AbstractEditor::pasteAction() {
	return m_pasteAction;
}

QIcon AbstractEditor::icon() const {
	return QIcon( ":/images/typeunknown.png" );
}

void AbstractEditor::serialize( XSLProjectSessionEditor * data, bool content ) {
	Q_UNUSED( content );
	data->writeProperty( "ClassName", metaObject()->className() ); // Store the class name
}

void AbstractEditor::deserialize( XSLProjectSessionEditor * data ) {
	Q_UNUSED( data );
	// Dont't read the class name, already read.
}

AbstractEditor * AbstractEditor::deserializeEditor( XSLProjectSessionEditor * data ) {
	QString name = data->readProperty( "ClassName" ).toString();

	int id = QMetaType::type( qPrintable( name ) );
	if( id != -1 ) {
		void * editorPtr = QMetaType::construct( id );
		if( editorPtr ) {
			AbstractEditor * editor = static_cast<AbstractEditor*>( editorPtr );
			editor->deserialize( data );
			return editor;
		}
	}
	return NULL;
}


