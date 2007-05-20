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

#include "xmlfileeditor.h"
#include "xmleditor.h"
#include "xsleditor.h"

XMLFileEditor::XMLFileEditor( QWidget *parent, XSLProject * project, TextEditor * textEditor ) : FileEditor( textEditor ? textEditor : new XMLEditor( parent ), parent, project ) {
	
}

XMLFileEditor::~XMLFileEditor() {
	
}

QString XMLFileEditor::getSuffix() const {
	if( getFileName().isEmpty() ) 
		return "xml";
	else
		return FileEditor::getSuffix();
}


XSLFileEditor::XSLFileEditor( QWidget *parent, XSLProject * project ) : XMLFileEditor( parent, project, new XSLEditor( parent, project ) ) {
	connect( m_view, SIGNAL(hasError(QString)), this, SLOT(setMessage(QString)) );
}

XSLFileEditor::~XSLFileEditor() {
	
}

QString XSLFileEditor::getSuffix() const {
	if( getFileName().isEmpty() ) 
		return "xsl";
	else
		return XMLFileEditor::getSuffix();
}
