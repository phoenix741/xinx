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
		textEdit()->setPlainText( prettyPrinter.getResult() );
	} catch( XMLPrettyPrinterException e ) {
		setMessage( e.getMessage() );
		return false;
	}
	return true;
}

XSLCompletionModel * StyleSheetEditor::completionModel() const {
	return m_completionModel;
}
