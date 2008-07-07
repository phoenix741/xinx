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
#include "htmlfileeditor.h"
#include "xmlprettyprinter.h"
#include "xslmodelcompleter.h"
#include "xsllistview.h"
#include "xmltexteditor.h"

// Qt header
#include <QCompleter>

/* StyleSheetContainer */

HtmlFileEditor::HtmlFileEditor( QWidget *parent ) : TextFileEditor( new XmlTextEditor(), parent ) {
	m_completionModel = new XSLCompletionModel( XSLCompletionModel::Html, 0 );

	QCompleter * completer = new QCompleter( textEdit() );
	completer->setModel( m_completionModel );
	textEdit()->setCompleter( completer );
}

HtmlFileEditor::~HtmlFileEditor() {
	delete m_completionModel;
}

QString HtmlFileEditor::defaultFileName() const {
	return tr( "noname.html" );
}

QIcon HtmlFileEditor::icon() const {
	return QIcon( ":/images/typehtml.png" );
}

bool HtmlFileEditor::autoIndent() {
	try {
		XMLPrettyPrinter prettyPrinter;
		prettyPrinter.setText( textEdit()->toPlainText() );
		prettyPrinter.process();
		textEdit()->setPlainText( prettyPrinter.getResult() );
	} catch( XMLPrettyPrinterException e ) {
		setMessage( e.getMessage() );
		return false;
	}
	return true;
}

XSLCompletionModel * HtmlFileEditor::completionModel() const {
	return m_completionModel;
}
