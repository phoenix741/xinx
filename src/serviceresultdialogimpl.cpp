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

#include "serviceresultdialogimpl.h"

#include "xmlhighlighter.h"

class PrivateServiceResultDialogImpl {
public:
	PrivateServiceResultDialogImpl( ServiceResultDialogImpl * parent );
	
	QHash<QString,QString> m_input;
	QHash<QString,QString> m_output;
private:
	ServiceResultDialogImpl * m_parent;	
};

ServiceResultDialogImpl::ServiceResultDialogImpl( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f) {
	setupUi(this);
	d = new PrivateServiceResultDialogImpl( this );
	
	new XmlHighlighter( m_inputStreamTextEdit->document() );
	new XmlHighlighter( m_outputStreamTextEdit->document() );
}

virtual ServiceResultDialogImpl::~ServiceResultDialogImpl() {
	delete d;
}

void ServiceResultDialogImpl::setInputStreamText( const QHash<QString,QString> & text ) {
	d->m_input = text;
	m_inputStreamTextEdit->setText( text.at(1) );
}

void ServiceResultDialogImpl::setOutputStreamText( const QHash<QString,QString> & text ) {
	d->m_output = text;
	m_outputStreamTextEdit->setText( text.at(1) );
}
