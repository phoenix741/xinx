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
#include "htmlfileeditor.h"
#include "xmlprettyprinter.h"
#include "xslcompletionnodemodel.h"
#include "xslcontentviewparser.h"
#include "xmltexteditor.h"
#include <contentview/contentviewnode.h>

// Qt header
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QCompleter>
#include <QTextCodec>

/* HtmlFileEditor */

HtmlFileEditor::HtmlFileEditor( QWidget *parent ) : ContentViewTextEditor( 0, new XmlTextEditor(), parent ) {
	m_completionModel = new XslCompletionNodeModel( rootNode(), this );
	m_completionModel->setCompleteTags( XslCompletionNodeModel::Html );

	QCompleter * completer = new QCompleter( textEdit() );
	completer->setModel( m_completionModel );
	textEdit()->setCompleter( completer );
	qobject_cast<XslTextEditor*>( textEdit() )->setModel( m_completionModel );
}

HtmlFileEditor::~HtmlFileEditor() {
	ContentViewParser * p = parser();
	setParser( 0 );
	qobject_cast<XslTextEditor*>( textEdit() )->setModel( 0 );
	delete m_completionModel;
	delete p;
}

XslCompletionNodeModel * HtmlFileEditor::completionModel() const {
	return m_completionModel;
}

QString HtmlFileEditor::defaultFileName() const {
	return tr( "noname.html" );
}

QIcon HtmlFileEditor::icon() const {
	return QIcon( ":/images/typehtml.png" );
}

QTextCodec * HtmlFileEditor::codec() const {
	return QTextCodec::codecForHtml( textEdit()->toPlainText().toLocal8Bit(), TextFileEditor::codec() );
}

bool HtmlFileEditor::autoIndent() {
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

/* XmlFileEditor */

XmlFileEditor::XmlFileEditor( QWidget *parent ) : TextFileEditor( new XmlTextEditor(), parent ), m_codec( 0 ) {
}

XmlFileEditor::~XmlFileEditor() {
}

QTextCodec * XmlFileEditor::codec() const {
	if( m_codec )
		return m_codec;
	else
		return TextFileEditor::codec();
}

QString XmlFileEditor::defaultFileName() const {
	return tr( "noname.xml" );
}

QIcon XmlFileEditor::icon() const {
	return QIcon( ":/images/typexml.png" );
}

bool XmlFileEditor::autoIndent() {
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

void XmlFileEditor::loadFromDevice( QIODevice & d ) {
	{
		QXmlStreamReader reader( &d );
		while( ! reader.atEnd() ) {
			reader.readNext();

			if( reader.isStartDocument() ) {
				m_codec = QTextCodec::codecForName( reader.documentEncoding().toString().toLatin1() );
				break;
			}

			if( reader.isStartElement() ) {
				m_codec = TextFileEditor::codec();
				break;
			}
		}
	}
	d.reset();
	TextFileEditor::loadFromDevice( d );
}

/* StyleSheetEditor */

StyleSheetEditor::StyleSheetEditor( QWidget *parent ) : ContentViewTextEditor( new XslContentViewParser(), new XslTextEditor(), parent ) {
	connect( textEdit(), SIGNAL(positionInEditorChanged(QModelIndex)), this, SIGNAL(positionInEditorChanged(QModelIndex)) );

	m_completionModel = new XslCompletionNodeModel( rootNode(), this );
	m_completionModel->setCompleteTags( XslCompletionNodeModel::NoTags );

	QCompleter * completer = new QCompleter( textEdit() );
	completer->setModel( m_completionModel );
	textEdit()->setCompleter( completer );
	qobject_cast<XslTextEditor*>( textEdit() )->setModel( m_completionModel );
}

StyleSheetEditor::~StyleSheetEditor() {
	ContentViewParser * p = parser();
	setParser( 0 );
	qobject_cast<XslTextEditor*>( textEdit() )->setModel( 0 );
	delete m_completionModel;
	delete p;
}

QTextCodec * StyleSheetEditor::codec() const {
	if( dynamic_cast<XslContentViewParser*>( parser() ) && dynamic_cast<XslContentViewParser*>( parser() )->codec() )
		return dynamic_cast<XslContentViewParser*>( parser() )->codec();
	else
		return ContentViewTextEditor::codec();
}

XslCompletionNodeModel * StyleSheetEditor::completionModel() const {
	return m_completionModel;
}

QString StyleSheetEditor::defaultFileName() const {
	return tr( "noname.xsl" );
}

QIcon StyleSheetEditor::icon() const {
	return QIcon( ":/images/typexsl.png" );
}

void StyleSheetEditor::searchWord( const QString & word ) {
	if( ContentViewNode * n = m_completionModel->nodeOfWord( word ) ) {
		emit open( n->fileName(), n->line() );
		return;
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
