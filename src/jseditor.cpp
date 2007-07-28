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

#include "jseditor.h"

#include <QCompleter>
#include <QDomElement>
#include <QKeyEvent>
#include <QTextEdit>
#include <QTextBlock>
#include <QAbstractItemView>
#include <QScrollBar>

#include "jshighlighter.h"
#include "javascriptparser.h"
#include "javascriptfilecontent.h"
#include "javascriptmodelcompleter.h"

#define EOW			"~!@$#%^&*()+{}|\"<>?,/;'[]\\="

/* PrivateJSEditor */

class PrivateJSEditor {
public:
	PrivateJSEditor( JSEditor * parent );
	virtual ~PrivateJSEditor();
	
	JavaScriptParser * m_parser;
	JavascriptFileContent * m_model;
	JavascriptModelCompleter * m_modelCompleter;
	QCompleter * m_completer;
private:
	JSEditor * m_parent;
};

PrivateJSEditor::PrivateJSEditor( JSEditor * parent ) {
	m_parent    = parent;
	m_parser    = NULL;
	m_model     = NULL;
	m_completer = NULL;
}

PrivateJSEditor::~PrivateJSEditor() {
	delete m_model;
	delete m_parser;
}

/* JSEditor */

JSEditor::JSEditor( QWidget * parent ) : TextEditor( parent ) {
	d = new PrivateJSEditor( this );
	new JsHighlighter( document() );

	d->m_completer = new QCompleter( this );
	connect( d->m_completer, SIGNAL(activated(const QModelIndex&)), this, SLOT(insertCompletion(const QModelIndex&)) );
}

JSEditor::~JSEditor() {
	delete d;
}

void JSEditor::updateModel() {
	JavaScriptParser * parser = NULL;
	try {
		parser = new JavaScriptParser( toPlainText() );
		emit deleteModel();
		d->m_completer->setModel( NULL );
		delete d->m_model; d->m_model = NULL;
		delete d->m_parser; d->m_parser = NULL;
		d->m_parser = parser;
		d->m_model  = new JavascriptFileContent( d->m_parser, this );
		d->m_modelCompleter = new JavascriptModelCompleter( d->m_parser, this );
		emit createModel();
		d->m_completer->setModel( d->m_modelCompleter );
	} catch( JavaScriptParserException e ) {
		delete parser;
		throw e;
	}
}

QAbstractItemModel * JSEditor::model() {
	return d->m_model;
}

void JSEditor::insertCompletion( const QModelIndex& index ) {
	QTextCursor tc = textCursor();
	QCompleter * c = d->m_completer;
	QString completion = c->completionModel()->data( index ).toString();
	
	for( int i = 0; i < c->completionPrefix().length(); i++ ) tc.deletePreviousChar();
	tc.insertText( completion );

	setTextCursor( tc );
}

void JSEditor::keyPressEvent( QKeyEvent * e ) {
	QCompleter * c = d->m_completer;
	
	if ( c && c->popup()->isVisible() ) {
		// The following keys are forwarded by the completer to the widget
		switch (e->key()) {
		case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
			e->ignore();
			return; // let the completer do default behavior
		default:
            break;
        }
     }

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if (!c || !isShortcut) // dont process the shortcut when we have a completer
		TextEditor::keyPressEvent( e );
		
	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);

	if (!c || (ctrlOrShift && e->text().isEmpty()))
		return;

    static QString eow(EOW); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor( textCursor() );

    if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 2 || eow.contains(e->text().right(1)))) {
       c->popup()->hide();
       return;
    }

    if ( completionPrefix != c->completionPrefix() ) {
        c->setCompletionPrefix( completionPrefix );
        c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
    }

	QRect cr = cursorRect();
	cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
	c->complete(cr); // popup it up!
}
