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
#include "editors/contentviewtexteditor.h"
#include "core/xinxconfig.h"
#include "editors/xinxcodeedit.h"
#include "contentview/contentviewnode.h"
#include "contentview/contentviewparser.h"
#include "contentview/contentviewmodel.h"

/* ContentViewTextEditor */

ContentViewTextEditor::ContentViewTextEditor( ContentViewParser * parser, XinxCodeEdit * editor, QWidget *parent ) : TextFileEditor( editor, parent ), m_model( 0 ), m_parser( parser ) {
	m_rootNode = new ContentViewNode( "root", -1 );
	if( parser ) {
		parser->setAttachId( (unsigned long)m_rootNode );
	}
	m_keyTimer = new QTimer();
	m_keyTimer->setSingleShot( true );
	m_keyTimer->setInterval( XINXConfig::self()->config().editor.automaticModelRefreshTimeout );

	connect( m_keyTimer, SIGNAL(timeout()), this, SLOT(updateModel()) );
	connect( textEdit()->document(), SIGNAL(contentsChanged()), this, SLOT(textChanged()) );
	disconnect( textEdit()->document(), SIGNAL(contentsChanged()), this, SIGNAL(contentChanged()) );
}

ContentViewTextEditor::~ContentViewTextEditor() {
	delete m_model;
	m_rootNode->deleteInstance( (unsigned long)m_rootNode );
}

void ContentViewTextEditor::loadFromFile( const QString & fileName ) {
	m_rootNode->setFileName( fileName );
	TextFileEditor::loadFromFile( fileName );
}

void ContentViewTextEditor::saveToFile( const QString & fileName ) {
	m_rootNode->setFileName( fileName );
	TextFileEditor::saveToFile( fileName );
}

void ContentViewTextEditor::loadFromDevice( QIODevice & d ) {
	try {
		m_parser->loadFromDevice( m_rootNode, &d );
		textEdit()->setErrors( QList<int>() );
		setMessage( QString() );
	} catch( ContentViewException e ) {
		textEdit()->setErrors( QList<int>() << e.getLine() );
		setMessage( e.getMessage() );
	}
	d.reset();
	TextFileEditor::loadFromDevice( d );
}

QAbstractItemModel * ContentViewTextEditor::model() const {
	return contentViewModel();
}

ContentViewModel * ContentViewTextEditor::contentViewModel() const {
	if( ( ! m_model ) && m_rootNode )
		m_model = new ContentViewModel( m_rootNode );
	return m_model;
}

void ContentViewTextEditor::setParser( ContentViewParser * parser ) {
	delete m_model;
	m_parser = parser;
}

void ContentViewTextEditor::updateModel() {
	try {
		m_parser->loadFromContent( m_rootNode, textEdit()->toPlainText() );
		emit contentChanged();
		textEdit()->setErrors( QList<int>() );
		setMessage( QString() );
	} catch( ContentViewException e ) {
		textEdit()->setErrors( QList<int>() << e.getLine() );
		setMessage( e.getMessage() );
	}
}

void ContentViewTextEditor::textChanged() {
	m_keyTimer->start();
}

ContentViewParser * ContentViewTextEditor::parser() const {
	return m_parser;
}

ContentViewNode * ContentViewTextEditor::rootNode() const {
	return m_rootNode;
}

