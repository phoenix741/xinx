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
#include "itemmodelfileeditor.h"
#include "texteditor.h"
#include "numberbar.h"
#include "xinxconfig.h"

/* ItemModelFileEditor */

ItemModelFileEditor::ItemModelFileEditor( FileContentParser * element, TextEditor * editor, QWidget *parent ) : TextFileEditor( editor, parent ), m_model( 0 ), m_parser( element ) {
	m_keyTimer = new QTimer();
	m_keyTimer->setSingleShot( true );
	m_keyTimer->setInterval( XINXConfig::self()->config().editor.automaticModelRefreshTimeout );
	connect( m_keyTimer, SIGNAL(timeout()), this, SLOT(updateModel()) );
	connect( textEdit(), SIGNAL(textChanged()), this, SLOT(textChanged()) );
	disconnect( textEdit(), SIGNAL(textChanged()), this, SIGNAL(contentChanged()) );
}

ItemModelFileEditor::~ItemModelFileEditor() {
	delete m_model;
}

void ItemModelFileEditor::loadFromFile( const QString & fileName ) {
	dynamic_cast<FileContentElement*>( m_parser )->setFilename( fileName );
	TextFileEditor::loadFromFile( fileName );
}

void ItemModelFileEditor::saveToFile( const QString & fileName ) {
	dynamic_cast<FileContentElement*>( m_parser )->setFilename( fileName );
	TextFileEditor::saveToFile( fileName );
}

void ItemModelFileEditor::loadFromDevice( QIODevice & d ) {
	try {
		m_parser->loadFromDevice( &d );
		numbersBar()->setErrors( QList<int>() );
		setMessage( QString() );
	} catch( FileContentException e ) {
		numbersBar()->setErrors( QList<int>() << e.getLine() );
		setMessage( e.getMessage() );
	}
	d.reset();
	TextFileEditor::loadFromDevice( d );
}

QAbstractItemModel * ItemModelFileEditor::model()  const {
	if( ! m_model )
		m_model = new FileContentItemModel( dynamic_cast<FileContentElement*>( m_parser ) );
	return m_model;
}

void ItemModelFileEditor::setParser( FileContentParser * parser ) {
	delete m_model;
	m_parser = parser;
}

void ItemModelFileEditor::updateModel() {
	try {
		m_parser->loadFromContent( textEdit()->toPlainText() );
		emit contentChanged();
		numbersBar()->setErrors( QList<int>() );
		setMessage( QString() );
	} catch( FileContentException e ) {
		numbersBar()->setErrors( QList<int>() << e.getLine() );
		setMessage( e.getMessage() );
	}
}

void ItemModelFileEditor::textChanged() {
	m_keyTimer->start();
}

