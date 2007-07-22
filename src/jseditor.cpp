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

#include "jshighlighter.h"
#include "javascriptparser.h"
#include "javascriptfilecontent.h"

/* PrivateJSEditor */

class PrivateJSEditor {
public:
	PrivateJSEditor( JSEditor * parent );
	virtual ~PrivateJSEditor();
	
	JavaScriptParser * m_parser;
	JavascriptFileContent * m_model;
private:
	JSEditor * m_parent;
};

PrivateJSEditor::PrivateJSEditor( JSEditor * parent ) {
	m_parent = parent;
	m_parser = NULL;
	m_model  = NULL;
}

PrivateJSEditor::~PrivateJSEditor() {
	delete m_model;
	delete m_parser;
}

/* JSEditor */

JSEditor::JSEditor( QWidget * parent ) : TextEditor( parent ) {
	d = new PrivateJSEditor( this );
	new JsHighlighter( document() );
}

JSEditor::~JSEditor() {
	delete d;
}

void JSEditor::updateModel() {
	JavaScriptParser * parser = new JavaScriptParser( toPlainText() );
	emit deleteModel();
	delete d->m_model; d->m_model = NULL;
	delete d->m_parser; d->m_parser = NULL;
	d->m_parser = parser;
	d->m_model  = new JavascriptFileContent( d->m_parser, this );
	emit createModel();
}

QAbstractItemModel * JSEditor::model() {
	return d->m_model;
}
