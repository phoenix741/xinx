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

/* PrivateWebServicesEditor */

class PrivateWebServicesEditor : public QObject {
	Q_OBJECT
public:
	PrivateWebServicesEditor( WebServicesEditor * parent );
	~PrivateWebServicesEditor();
	
	void store( const QString & );
	void restore( const QString & );

	QString m_serviceName, m_operationName, m_oldParamValue;
	QComboBox * m_servicesList, * m_paramList, * m_actionList;
	QHash<QString,QString> m_paramValues;
	bool m_isModified;

public slots:
	void webServicesChanged();
	void webServicesActivated( int );
	void webServicesParamActivated( int );
	void webServicesValueActivated();

public:
	void loadServicesList();
	void loadActionsList( int index );
	void loadValuesList( int index );

private:
	WebServicesEditor * m_parent;
};

PrivateWebServicesEditor::PrivateWebServicesEditor( WebServicesEditor * parent ) {
	m_parent = parent;
}

PrivateWebServicesEditor::~PrivateWebServicesEditor() {
	
}

void PrivateWebServicesEditor::loadServicesList() {
	m_servicesList->clear();
	foreach( WebServices * ed, *(global.m_webServices) ) 
		if( ! ed->name().isEmpty() ) 
			m_servicesList->addItem( QIcon(":/images/services.png"), ed->name(), qVariantFromValue( (void*)ed ) );

	int findIndex = m_servicesList->findText( m_serviceName );
	if( ( ! m_serviceName.isEmpty() ) && ( findIndex == -1 ) ) {
		m_servicesList->addItem( QIcon(":/images/services.png"), m_serviceName );
		m_servicesList->setItemData( m_servicesList->count() - 1, Qt::gray, Qt::ForegroundRole );
		m_servicesList->setCurrentIndex( m_servicesList->count() - 1 );
	} else if( findIndex >= 0 ) {
		m_servicesList->setCurrentIndex( findIndex );
	} else if( m_servicesList->count() > 0 )
		m_servicesList->setCurrentIndex( 0 );
}

void PrivateWebServicesEditor::loadActionsList( int index ) {
	m_actionList->clear();
	if( ( index >= 0 ) && ( m_servicesList->itemData( index ).isValid() ) ) {
		WebServices * ed = (WebServices*)( m_servicesList->itemData( index ).value<void*>() );
		foreach( Operation * op, ed->operations() ) 
			m_actionList->addItem( QIcon(":/images/action.png"), op->name(), qVariantFromValue( (void*)op ) );
	}
	
	int findIndex = m_actionList->findText( m_operationName );
	if( ( ! m_operationName.isEmpty() ) && ( findIndex == -1 ) ) {
		m_actionList->addItem( QIcon(":/images/action.png"), m_operationName );
		m_actionList->setItemData( m_actionList->count() - 1, Qt::gray, Qt::ForegroundRole );
		m_actionList->setCurrentIndex( m_actionList->count() - 1 );
	} else if( findIndex >= 0 ) {
		m_actionList->setCurrentIndex( findIndex );
	} else if( m_actionList->count() > 0 )
		m_actionList->setCurrentIndex( 0 );
}

void PrivateWebServicesEditor::loadValuesList( int index ) {
	QString param = m_paramList->currentText();
	
	m_paramList->clear(); 
	if( ( index >= 0 ) && ( m_actionList->itemData( index ).isValid() ) ) {
		Operation * op = (Operation*)( m_actionList->itemData( index ).value<void*>() );
		foreach( Parameter * param, op->inputParam() ) 
			m_paramList->addItem( QIcon(":/images/serviceparam.png"), param->paramString() );
	}
	
	foreach( QString param, m_paramValues.keys() ) {
		if( ( !param.isEmpty() ) && m_paramList->findText( param ) == -1 ) {
			m_paramList->addItem( QIcon(":/images/serviceparam.png"), param );
			m_paramList->setItemData( m_paramList->count() - 1, Qt::gray, Qt::ForegroundRole );
		}
	}
	
	if( ! param.isEmpty() ) {
		int paramIndex = m_paramList->findText( param );
		if( paramIndex != -1 )
			m_paramList->setCurrentIndex( paramIndex );
	} else if( m_paramList->count() > 0 )
		m_paramList->setCurrentIndex( 0 );
}

void PrivateWebServicesEditor::webServicesChanged() {
	loadServicesList();
	loadActionsList( m_servicesList->currentIndex() );
	loadValuesList( m_actionList->currentIndex() );
}

void PrivateWebServicesEditor::webServicesActivated( int index ) {
	m_isModified = true;	
	loadActionsList( index );
	loadValuesList( m_actionList->currentIndex() );
}

void PrivateWebServicesEditor::webServicesParamActivated( int index ) {
	m_isModified = true;
	loadValuesList( index );
}

void PrivateWebServicesEditor::webServicesValueActivated() {
	if( ! m_oldParamValue.isEmpty() ) store( m_oldParamValue );
	m_oldParamValue = m_paramList->currentText();
	restore( m_oldParamValue );	
}

void PrivateWebServicesEditor::store( const QString & paramStr ) {
	m_paramValues[ paramStr ] = m_parent->textEdit()->toPlainText();
	m_isModified = m_isModified || m_parent->textEdit()->document()->isModified();
}

void PrivateWebServicesEditor::restore( const QString & paramStr ) {
	m_parent->textEdit()->setPlainText( m_paramValues[ paramStr ] );
	m_parent->textEdit()->document()->setModified( m_isModified );
	m_parent->m_view->updateModel();
}

/* WebServicesEditor */

WebServicesEditor::WebServicesEditor( QWidget *parent ) : FileEditor( new XMLEditor( parent ), parent ) {
	d = new PrivateWebServicesEditor( this );
	
	d->m_oldParamValue = QString();
	
	QLabel * label1 = new QLabel( tr("WebServices : "), this );
	d->m_servicesList = new QComboBox( this );

	QLabel * label2 = new QLabel( tr("Action : "), this );
	d->m_actionList = new QComboBox( this );
		
	QLabel * label3 = new QLabel( tr("Parameter : "), this );	
	d->m_paramList = new QComboBox( this );

	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget( label1 );
	hbox->addWidget( d->m_servicesList );
	hbox->addSpacing( 10 );
	hbox->addWidget( label2 );
	hbox->addWidget( d->m_actionList );
	hbox->addSpacing( 10 );
	hbox->addWidget( label3 );
	hbox->addWidget( d->m_paramList );
	//hbox->addStretch();
	
	m_vbox->insertLayout( 0, hbox );
	
	connect( &global, SIGNAL(webServicesChanged()), d, SLOT(webServicesChanged()) );
	connect( d->m_servicesList, SIGNAL(activated(int)), d, SLOT(webServicesActivated(int)) );
	connect( d->m_actionList, SIGNAL(activated(int)), d, SLOT(webServicesParamActivated(int)) );
	connect( d->m_paramList, SIGNAL(activated(int)), d, SLOT(webServicesValueActivated()) );

	d->loadServicesList();
	d->loadActionsList( d->m_servicesList->currentIndex() );
	d->loadValuesList( d->m_actionList->currentIndex() );

	d->m_isModified = false;
}

WebServicesEditor::~WebServicesEditor() {
	delete d;
}

const QHash<QString,QString> & WebServicesEditor::values() const { 
	return d->m_paramValues; 
}

QString WebServicesEditor::getSuffix() const {
	if( getFileName().isEmpty() ) 
		return "fws";
	else
		return FileEditor::getSuffix();
}

WebServices * WebServicesEditor::service() {
	int index = d->m_servicesList->currentIndex();
	if( ( index >= 0 ) && d->m_servicesList->itemData( index ).isValid() )
		return static_cast<WebServices*>( d->m_servicesList->itemData( index ).value<void*>() );
	else
		return NULL;
}

Operation * WebServicesEditor::operation() {
	int index = d->m_actionList->currentIndex();
	if( ( index >= 0 ) && d->m_actionList->itemData( index ).isValid() )
		return (Operation*)( d->m_actionList->itemData( index ).value<void*>() );
	else
		return NULL;
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

	d->m_paramValues.clear(); d->m_oldParamValue = QString();
	try {
		QDomDocument document;
		if( ! document.setContent( &file, false ) ) throw WrongFwsFormatException();
			
		QDomElement element = document.documentElement();
		if( element.tagName() != "webservice" ) throw WrongFwsFormatException();
		
		d->m_serviceName = element.attribute( "service" );
		d->m_operationName = element.attribute( "action" );
		
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
			
			d->m_paramValues[ paramStr ] = paramValue;
			
			param = param.nextSiblingElement();
		}
		
		d->loadServicesList();
		d->loadActionsList( d->m_servicesList->currentIndex() );
		d->loadValuesList( d->m_actionList->currentIndex() );
		d->restore( d->m_paramList->currentText() );
	} catch(WrongFwsFormatException) {
		file.seek( 0 );
		QTextStream in( &file );
		QApplication::setOverrideCursor( Qt::WaitCursor );
		m_view->setPlainText( in.readAll() );
		m_view->updateModel();
	}

	d->m_isModified = false;
	
	QApplication::restoreOverrideCursor();
}

bool WebServicesEditor::saveFile( const QString & fileName ){
	if( ( fileName == m_fileName ) || fileName.isEmpty() ) createBackup( m_fileName ); 
	if( ! fileName.isEmpty() ) setFileName( fileName );
	
	QApplication::setOverrideCursor(Qt::WaitCursor);
	
	desactivateWatcher();
	
	d->store( d->m_paramList->currentText() );
	
	QDomDocument document;
	QDomElement element = document.createElement( "webservice" );
	document.appendChild( element );
	element.setAttribute( "service", d->m_servicesList->currentText() );
	element.setAttribute( "action", d->m_actionList->currentText() );
	
	foreach( QString param, d->m_paramValues.keys() ) {
		QDomElement paramElement = document.createElement( param ) ;
		element.appendChild( paramElement );
				
		QDomText text = document.createTextNode( d->m_paramValues[param] );
		paramElement.appendChild( text );
	}

	QFile file( getFileName() );
	if ( ! file.open( QFile::WriteOnly | QFile::Text ) ) {
		QMessageBox::warning(this, tr( "XINX" ), tr( "Cannot write file %1:\n%2." )
																.arg( fileName )
																.arg( file.errorString() ) );
		activateWatcher();
		return false;
	}

	QTextStream out( &file );
	document.save( out, 3 );
	file.flush();
	file.close();
	
	d->m_isModified = false;
	
	activateWatcher();

	QApplication::restoreOverrideCursor();

	return true;	
}

void WebServicesEditor::serializeEditor( QDomElement & element, bool content ) {
	d->store( d->m_paramList->currentText() );

	element.setAttribute( "class", metaObject()->className() );
	element.setAttribute( "filename", m_fileName );
	element.setAttribute( "position", m_view->textCursor().position() );

	element.setAttribute( "service", d->m_servicesList->currentText() );
	element.setAttribute( "action", d->m_actionList->currentText() );
	element.setAttribute( "param", d->m_paramList->currentText() );

	if( content && m_view->document()->isModified() ) {
		element.setAttribute( "ismodified", d->m_isModified );

		foreach( QString param, d->m_paramValues.keys() ) {
			QDomElement paramElement = global.m_project->sessionDocument().createElement( param ) ;
			element.appendChild( paramElement );
				
			QDomText text = global.m_project->sessionDocument().createTextNode( d->m_paramValues[param] );
			paramElement.appendChild( text );
		}
	}
}

void WebServicesEditor::deserializeEditor( const QDomElement & element ) {
	m_fileName = element.attribute( "filename" );
	
	d->m_paramValues.clear(); d->m_oldParamValue = QString();
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
		
		d->m_paramValues[ name ] = plainText;
		
		params = params.nextSiblingElement();
	}
		
	if( ( ! content ) && ( !m_fileName.isEmpty() ) ) 
		loadFile( m_fileName );

	d->m_serviceName = element.attribute( "service" );
	d->m_operationName = element.attribute( "action" );
	QString param = element.attribute( "param" );
	
	d->loadServicesList();
	d->loadActionsList( d->m_servicesList->currentIndex() );
	d->loadValuesList( d->m_actionList->currentIndex() );
	if( ! param.isEmpty() ) 
		d->m_paramList->setCurrentIndex( d->m_paramList->findText( param ) );
	d->restore( d->m_paramList->currentText() );

	if( content ) 
		d->m_isModified = (bool)(element.attribute( "ismodified" ).toInt());
	else 
		d->m_isModified = false;

	QTextCursor tc = m_view->textCursor();
	tc.setPosition( element.attribute( "position" ).toInt() );
	m_view->setTextCursor( tc );
}

#include "webserviceseditor.moc"
