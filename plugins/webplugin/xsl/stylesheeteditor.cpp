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
#include "stylesheeteditor.h"
#include "xmlprettyprinter.h"
#include "xslmodelcompleter.h"
#include "xsllistview.h"
#include "xsltexteditor.h"

// Qt header
#include <QMessageBox>
#include <QCompleter>
#include <QSortFilterProxyModel>

/* StyleSheetContainer */

StyleSheetEditor::StyleSheetEditor( QWidget *parent ) : ItemModelFileEditor( m_parser = new XSLFileContentParser(), new XslTextEditor(), parent ) {
	m_list = new XslContentElementList( dynamic_cast<XSLFileContentParser*>( m_parser ) );
	dynamic_cast<XslTextEditor*>( textEdit() )->setParser( m_list );
	m_completionModel = new XSLCompletionModel( XSLCompletionModel::Xsl, m_list );

	QCompleter * completer = new QCompleter( textEdit() );
	completer->setModel( m_completionModel );
	textEdit()->setCompleter( completer );
}

StyleSheetEditor::~StyleSheetEditor() {
	setParser( 0 );
	delete m_completionModel;
	dynamic_cast<XslTextEditor*>( textEdit() )->setParser( 0 );
	delete m_list;
	delete m_parser;
}

QTextCodec * StyleSheetEditor::codec() const {
	if( dynamic_cast<XSLFileContentParser*>( m_parser ) && dynamic_cast<XSLFileContentParser*>( m_parser )->codec() )
		return dynamic_cast<XSLFileContentParser*>( m_parser )->codec();
	else
		return ItemModelFileEditor::codec();
}

QString StyleSheetEditor::defaultFileName() const {
	return tr( "noname.xsl" );
}

QIcon StyleSheetEditor::icon() const {
	return QIcon( ":/images/typexsl.png" );
}

void StyleSheetEditor::searchWord( const QString & word ) {
	foreach( FileContentElement * element, m_list->list() ) {
		if( element->name() == word ) {
			emit open( element->filename(), element->line() );
			return;
		}
	}
	QMessageBox::information( this, tr("Search Word"), tr("Word %1 not found").arg( word ) );
}

bool StyleSheetEditor::autoIndent() {
	try {
		XMLPrettyPrinter prettyPrinter;
		prettyPrinter.setText( textEdit()->toPlainText() );
		prettyPrinter.process();

		textEdit()->textCursor().beginEditBlock();
		textEdit()->editor()->selectAll();
		textEdit()->textCursor().removeSelectedText();
		textEdit()->textCursor().movePosition( 1, QDocumentCursor::Start );
		textEdit()->textCursor().insertText( prettyPrinter.getResult() );
		textEdit()->textCursor().endEditBlock();
	} catch( XMLPrettyPrinterException e ) {
		setMessage( e.getMessage() );
		return false;
	}
	return true;
}

XSLCompletionModel * StyleSheetEditor::completionModel() const {
	return m_completionModel;
}
