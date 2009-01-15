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

// Xinx header
#include "serviceresultdialogimpl.h"

#include <xinxpluginsloader.h>
#include <xinxconfig.h>
#include <xinxlanguagefactory.h>

// QCodeEdit header
#include <qlanguagedefinition.h>

/* ServiceResultDialogImpl */

ServiceResultDialogImpl::ServiceResultDialogImpl( QWidget * parent, Qt::WFlags f ) : QDialog(parent, f) {
	setupUi(this);

	m_inputStreamTextEdit->setHighlighter( "XML" );
	m_outputStreamTextEdit->setHighlighter( "XML" );

	connect( m_inputComboBox, SIGNAL(activated(QString)), this, SLOT(inputComboChanged(QString)) );
	connect( m_outputComboBox, SIGNAL(activated(QString)), this, SLOT(outputComboChanged(QString)) );
}

ServiceResultDialogImpl::~ServiceResultDialogImpl() {

}

void ServiceResultDialogImpl::setInputStreamText( const QHash<QString,QString> & text ) {
	m_input = text;
	m_inputComboBox->addItems( text.keys() );
	m_inputStreamTextEdit->setPlainText( text.values().at( 0 ) );
}

void ServiceResultDialogImpl::setOutputStreamText( const QHash<QString,QString> & text ) {
	m_output = text;
	m_outputComboBox->addItems( text.keys() );
	m_outputStreamTextEdit->setPlainText( text.values().at( 0 ) );
}

void ServiceResultDialogImpl::inputComboChanged( QString value ) {
	m_inputStreamTextEdit->setPlainText( m_input.value( value ) );
}

void ServiceResultDialogImpl::outputComboChanged( QString value ) {
	m_outputStreamTextEdit->setPlainText( m_output.value( value ) );
}

