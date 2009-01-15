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
#include "xmlfileeditor.h"
#include "xmlprettyprinter.h"
#include "xsllistview.h"
#include "xmltexteditor.h"

// Qt header
#include <QXmlStreamReader>
#include <QTextCodec>

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

