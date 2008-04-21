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
#include "runsnipetdialogimpl.h"

// Qt header
#include <QMessageBox>
#include <QScriptEngine>

/* RunSnipetDialogImpl */

RunSnipetDialogImpl::RunSnipetDialogImpl( const Snipet & snipet, QWidget * parent, Qt::WFlags f) : QDialog(parent, f) {
	setupUi(this);
	
	m_text = snipet.text();
	
	m_paramGrid = new QGridLayout( m_paramGroupBox );

	m_labelName->setText( QString( "<b>%1</b>" ).arg( snipet.name() ) );
	m_descriptionLabel->setText( snipet.description() );
	foreach( QString params, snipet.params() ) {
		QLabel * label = new QLabel( params, m_paramGroupBox );
		QLineEdit * edit = new QLineEdit( m_paramGroupBox );

		m_paramGrid->addWidget( label, m_paramList.size(), 0 );
		m_paramGrid->addWidget( edit, m_paramList.size(), 1 );

		m_paramList.append( qMakePair( label, edit ) );
	}
	
	m_paramGroupBox->setVisible( m_paramList.size() > 0 );
	if( m_paramList.size() > 0 ) {
		m_paramList.at( 0 ).second->setFocus( Qt::OtherFocusReason );
	}
}

RunSnipetDialogImpl::~RunSnipetDialogImpl() {

}


QString RunSnipetDialogImpl::getResult() {
	/* Process arguments */
	QString text = m_text;
	for( int i = 0 ; i < m_paramList.size() ; i++ ) {
		text = text.arg( m_paramList.at( i ).second->text() );
	}
	/* Process script */
	QScriptEngine engine;
	QRegExp jsString( "<\\?.*\\?>" ); 
	jsString.setMinimal( true );
	int from = 0;
	QString processedString;
	while( jsString.indexIn( text, from ) >= 0 ) {
		processedString += text.mid( from, jsString.pos() - from ); 
		
		QString js = text.mid( jsString.pos() + 2, jsString.matchedLength() - 4 );
		if( js.at(0) == '=' ) {
			QScriptValue result = engine.evaluate( js.mid(1) );
			if( ! result.isError() )
				processedString += result.toString();
			else {
				QMessageBox::critical( this, tr("Script error"), result.toString() );
				return QString();
			}
		} else {
			engine.evaluate( js ); // Only for define variable or make pre-process.
			if( engine.hasUncaughtException() ) {
				QMessageBox::critical( this, tr("Script error"), engine.uncaughtException().toString() );
				return QString();
			}
		}
		from = jsString.pos() + jsString.matchedLength();
	}
	processedString += text.mid( from );
	
	return processedString;
}
