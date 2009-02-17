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
#include "editors/itemmodelfileeditor.h"
#include "core/xinxconfig.h"
#include "editors/xinxcodeedit.h"

/* ItemModelFileEditor */

ItemModelFileEditor::ItemModelFileEditor( FileContentParser * element, XinxCodeEdit * editor, QWidget *parent ) : TextFileEditor( editor, parent ), m_model( 0 ), m_parser( element ) {
	m_keyTimer = new QTimer();
	m_keyTimer->setSingleShot( true );
	m_keyTimer->setInterval( XINXConfig::self()->config().editor.automaticModelRefreshTimeout );
	connect( m_keyTimer, SIGNAL(timeout()), this, SLOT(updateModel()) );
	connect( textEdit()->document(), SIGNAL(contentsChanged()), this, SLOT(textChanged()) );
	disconnect( textEdit()->document(), SIGNAL(contentsChanged()), this, SIGNAL(contentChanged()) );
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
		textEdit()->setErrors( QList<int>() );
		setMessage( QString() );
	} catch( FileContentException e ) {
		textEdit()->setErrors( QList<int>() << e.getLine() );
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
		textEdit()->setErrors( QList<int>() );
		setMessage( QString() );
	} catch( FileContentException e ) {
		textEdit()->setErrors( QList<int>() << e.getLine() );
		setMessage( e.getMessage() );
	}
}

void ItemModelFileEditor::textChanged() {
	m_keyTimer->start();
}

