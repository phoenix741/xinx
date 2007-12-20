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

#include "jsfileeditor.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QMessageBox>

/* PrivateJSFileEditor */

class PrivateJSFileEditor {
public:
	PrivateJSFileEditor( JSFileEditor * parent );
	virtual ~PrivateJSFileEditor();
	
private:
	JSFileEditor * m_parent;
};

PrivateJSFileEditor::PrivateJSFileEditor( JSFileEditor * parent ) {
	m_parent = parent;
}

PrivateJSFileEditor::~PrivateJSFileEditor() {
}

/* JSFileEditor */

Q_DECLARE_METATYPE( JSFileEditor );

JSFileEditor::JSFileEditor( QWidget *parent ) : FileEditor( parent ) {
	d = new PrivateJSFileEditor( this );
	setFileType( FileEditor::JSFileType );
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

QIcon JSFileEditor::icon() {
	return QIcon( ":/images/typejs.png" );
}
