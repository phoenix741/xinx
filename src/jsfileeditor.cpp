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
#include "jsfileeditor.h"
#include "javascriptparser.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QMessageBox>

/* PrivateJSFileEditor */

class PrivateJSFileEditor {
public:
	PrivateJSFileEditor( JSFileEditor * parent );
	virtual ~PrivateJSFileEditor();
	
	JavaScriptParser * m_parser;
private:
	JSFileEditor * m_parent;
};

PrivateJSFileEditor::PrivateJSFileEditor( JSFileEditor * parent ) {
	m_parent = parent;
	m_parser = NULL;
}

PrivateJSFileEditor::~PrivateJSFileEditor() {
	delete m_parser;
}

/* JSFileEditor */

JSFileEditor::JSFileEditor( QWidget *parent ) : FileEditor( new JSEditor( parent ), parent ) {
	d = new PrivateJSFileEditor( this );
}

JSFileEditor::~JSFileEditor() {
	delete d;
}

QString JSFileEditor::getSuffix() const {
	if( getFileName().isEmpty() ) 
		return "js";
	else
		return FileEditor::getSuffix();
}

QAbstractItemModel * JSFileEditor::model() {
	try {
		JavaScriptParser * parser = new JavaScriptParser( textEdit()->toPlainText() );
		delete d->m_parser; d->m_parser = NULL;
		d->m_parser = parser;
	} catch( JavaScriptParserException e ) {
		setMessage( tr("Error JS at line %1").arg( e.m_line ) );
	}
	
	return NULL;
}
