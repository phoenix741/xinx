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


/* XmlFileEditor */

XmlFileEditor::XmlFileEditor( QWidget *parent ) : TextFileEditor( new XmlTextEditor(), parent ) {
}

XmlFileEditor::~XmlFileEditor() {
}

QTextCodec * XmlFileEditor::codec() const { // TODO: Use XML file codec
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
		textEdit()->setPlainText( prettyPrinter.getResult() );
	} catch( XMLPrettyPrinterException e ) {
		setMessage( e.getMessage() );
		return false;
	}
	return true;
}

