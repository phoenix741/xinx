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

#include "globals.h"
#include "webserviceseditor.h"
#include "xmleditor.h"
#include "webservices.h"
#include "xslproject.h"
#include "xinxconfig.h"

#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QDomDocument>

WebServicesEditor::WebServicesEditor( QWidget *parent ) :
	FileEditor( new XMLEditor( parent ), parent ) {
	
	m_oldParamValue = QString();
	
	QLabel * label1 = new QLabel( tr("WebServices : "), this );
	m_servicesList = new QComboBox( this );

	QLabel * label2 = new QLabel( tr("Action : "), this );
	m_actionList = new QComboBox( this );
		
	QLabel * label3 = new QLabel( tr("Parameter : "), this );	
	m_paramList = new QComboBox( this );

	webServicesChanged();
	
	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget( label1 );
	hbox->addWidget( m_servicesList );
	hbox->addSpacing( 10 );
	hbox->addWidget( label2 );
	hbox->addWidget( m_actionList );
	hbox->addSpacing( 10 );
	hbox->addWidget( label3 );
	hbox->addWidget( m_paramList );
	hbox->addStretch();
	
	m_vbox->insertLayout( 0, hbox );
	
	connect( &global, SIGNAL(webServicesChanged()), this, SLOT(webServicesChanged()) );
	connect( m_servicesList, SIGNAL(activated(int)), this, SLOT(webServicesActivated(int)) );
	connect( m_actionList, SIGNAL(activated(int)), this, SLOT(webServicesParamActivated(int)) );
	connect( m_paramList, SIGNAL(activated(int)), this, SLOT(webServicesValueActivated()) );

	isModified = false;
}

QString WebServicesEditor::getSuffix() const {
	if( getFileName().isEmpty() ) 
		return "fws";
	else
		return FileEditor::getSuffix();
}

WebServices * WebServicesEditor::service() {
	int index = m_servicesList->currentIndex();
	if( index >= 0 )
		return static_cast<WebServices*>(m_servicesList->itemData( index ).value<void*>());
	else
		return NULL;
}

Operation * WebServicesEditor::operation() {
	int index = m_actionList->currentIndex();
	if( index >= 0 )
		return (Operation*)(m_actionList->itemData( index ).value<void*>());
	else
		return NULL;
}


void WebServicesEditor::webServicesChanged() {
	m_servicesList->clear();
	foreach( WebServices * ed, *(global.m_webServices) ) {
		m_servicesList->addItem( QIcon(":/services.png"), ed->name(), qVariantFromValue( (void*)ed ) );
	}
	webServicesActivated( m_servicesList->currentIndex() );
}

void WebServicesEditor::webServicesActivated( int index ) {
	m_actionList->clear();
	isModified = true;
	if( index >= 0 ) {
		WebServices * ed = (WebServices*)(m_servicesList->itemData( index ).value<void*>());
		foreach( Operation * op, ed->operations() ) {
			m_actionList->addItem( QIcon(":/action.png"), op->name(), qVariantFromValue( (void*)op ) );
		}
		webServicesParamActivated( m_actionList->currentIndex() );
	}
}

void WebServicesEditor::webServicesParamActivated( int index ) {
	m_paramList->clear(); 
	isModified = true;
	if( index >= 0 ) {
		Operation * op = (Operation*)( m_actionList->itemData( index ).value<void*>() );
		foreach( Parameter * param, op->inputParam() ) {
			m_paramList->addItem( QIcon(":/serviceparam.png"), param->paramString() );
		}
		webServicesValueActivated();
	}
}

void WebServicesEditor::webServicesValueActivated() {
	isModified = true;
	QString paramStr = m_paramList->currentText();
	if( ! m_oldParamValue.isEmpty() )
		store( m_oldParamValue );
	restore( paramStr );	
	m_oldParamValue = paramStr;
}

void WebServicesEditor::store( const QString & paramStr ) {
	m_paramValues[ paramStr ] = textEdit()->toPlainText();
	isModified = isModified || textEdit()->document()->isModified();
}

void WebServicesEditor::restore( const QString & paramStr ) {
	textEdit()->setPlainText( m_paramValues[ paramStr ] );
	textEdit()->document()->setModified( isModified );
	m_view->updateModel();
}

class WrongFwsFormatException {};

void WebServicesEditor::loadFile( const QString & fileName ){
	if( ! fileName.isEmpty() ) m_fileName = fileName;

	QFile file( getFileName() );
	if ( ! file.open( QFile::ReadOnly | QFile::Text ) ) {
		QMessageBox::warning(this, tr( "XINX" ), tr( "Cannot read file %1:\n%2." )
																.arg( getFileName() )
																.arg( file.errorString() ) );
		return;
	}

	try {
		QDomDocument document;
		if( ! document.setContent( &file, false ) )
			throw WrongFwsFormatException();
		QDomElement element = document.documentElement();
		if( element.tagName() != "webservice" )
			throw WrongFwsFormatException();
		
		QString services = element.attribute( "service" );
		QString action = element.attribute( "action" );
		
		QDomElement param = element.firstChildElement();
		while( ! param.isNull() ) {
			QString paramStr = param.tagName();
			QString paramValue;
			QDomNode text = param.firstChild();
			while( ! text.isNull() ) {
				if( text.isText() ) {
					paramValue += text.toText().data();
					break;
				}
				text = text.nextSibling();
			}
			
			m_paramValues[ paramStr ] = paramValue;
			
			param = param.nextSiblingElement();
		}
		
		m_servicesList->setCurrentIndex( m_servicesList->findText( services ) );
		m_actionList->setCurrentIndex( m_actionList->findText( action ) );
		restore( m_paramList->currentText() );
	} catch(WrongFwsFormatException) {
		file.seek( 0 );
		QTextStream in( &file );
		QApplication::setOverrideCursor( Qt::WaitCursor );
		m_view->setPlainText( in.readAll() );
		m_view->updateModel();
	}

	isModified = false;
	
	QApplication::restoreOverrideCursor();
}

bool WebServicesEditor::saveFile( const QString & fileName ){
	if( ( fileName == m_fileName ) || fileName.isEmpty() ) createBackup( m_fileName ); 
	if( ! fileName.isEmpty() ) setFileName( fileName );
	
	QApplication::setOverrideCursor(Qt::WaitCursor);
	/*
	QTextStream out( &file );
	out << m_view->toPlainText();
	m_view->updateModel();
	*/
	store( m_paramList->currentText() );
	
	QDomDocument document;
	QDomElement element = document.createElement( "webservice" );
	document.appendChild( element );
	element.setAttribute( "service", m_servicesList->currentText() );
	element.setAttribute( "action", m_actionList->currentText() );
	
	foreach( QString param, m_paramValues.keys() ) {
		QDomElement paramElement = document.createElement( param ) ;
		element.appendChild( paramElement );
				
		QDomText text = document.createTextNode( m_paramValues[param] );
		paramElement.appendChild( text );
	}

	QFile file( getFileName() );
	if ( ! file.open( QFile::WriteOnly | QFile::Text ) ) {
		QMessageBox::warning(this, tr( "XINX" ), tr( "Cannot write file %1:\n%2." )
																.arg( fileName )
																.arg( file.errorString() ) );
		return false;
	}

	QTextStream out( &file );
	document.save( out, 3 );
	
	isModified = false;

	QApplication::restoreOverrideCursor();

	return true;	
}

void WebServicesEditor::serializeEditor( QDomElement & element, bool content ) {
	store( m_paramList->currentText() );

	element.setAttribute( "class", metaObject()->className() );
	element.setAttribute( "filename", m_fileName );
	element.setAttribute( "position", m_view->textCursor().position() );

	element.setAttribute( "service", m_servicesList->currentText() );
	element.setAttribute( "action", m_actionList->currentText() );
	element.setAttribute( "param", m_paramList->currentText() );

	if( content && m_view->document()->isModified() ) {
		element.setAttribute( "ismodified", isModified );

		foreach( QString param, m_paramValues.keys() ) {
			QDomElement paramElement = global.m_project->sessionDocument().createElement( param ) ;
			element.appendChild( paramElement );
				
			QDomText text = global.m_project->sessionDocument().createTextNode( m_paramValues[param] );
			paramElement.appendChild( text );
		}
	}
}

void WebServicesEditor::deserializeEditor( const QDomElement & element ) {
	m_fileName = element.attribute( "filename" );
	
	bool content = false;
	QDomElement params = element.firstChildElement();
	while( !params.isNull() ) {
		content = true;
		QString name = params.tagName();
		
		QString plainText;
		QDomNode node = params.firstChild();
		while( ! node.isNull() ) {
			if( node.isText() ) {
				plainText += node.toText().data();
			}				
			node = node.nextSibling();
		}
		
		m_paramValues[ name ] = plainText;
		
		params = params.nextSiblingElement();
	}
		
	if( content ) {
		isModified = (bool)(element.attribute( "ismodified" ).toInt());
	} else {
		if( !m_fileName.isEmpty() )
			loadFile( m_fileName );
	}

	QString services = element.attribute( "service" );
	QString action = element.attribute( "action" );
	QString param = element.attribute( "param" );
	
	if( !( services.isEmpty() || action.isEmpty() || param.isEmpty() ) ) {
		m_servicesList->setCurrentIndex( m_servicesList->findText( services ) );
		m_actionList->setCurrentIndex( m_actionList->findText( action ) );
		m_paramList->setCurrentIndex( m_paramList->findText( param ) );
		restore( m_paramList->currentText() );
	}

	QTextCursor tc = m_view->textCursor();
	tc.setPosition( element.attribute( "position" ).toInt() );
	m_view->setTextCursor( tc );
}
