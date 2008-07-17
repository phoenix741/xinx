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
#include "webserviceseditor.h"
#include "webservices.h"
#include "texteditor.h"
#include "xslproject.h"
#include "borderlayout.h"
#include <xmlprettyprinter.h>
#include <xmltexteditor.h>

// Qt header
#include <QToolButton>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QDomDocument>

/* WebServicesEditor */

WebServicesEditor::WebServicesEditor( QWidget *parent ) : TextFileEditor( new XmlTextEditor(), parent ) {
	m_oldParamValue = QString();

	m_updateButton = new QToolButton( this );
	m_updateButton->setIcon( QIcon(":/images/reload.png") );
	m_updateButton->setText( tr("Update WebServices List") );
	m_runButton = new QToolButton( this );
	m_runButton->setIcon( QIcon(":/images/run.png") );
	m_runButton->setText( tr("Call the service") );

	QLabel * label1 = new QLabel( tr("WebServices : "), this );
	m_servicesList = new QComboBox( this );

	QLabel * label2 = new QLabel( tr("Action : "), this );
	m_actionList = new QComboBox( this );

	QLabel * label3 = new QLabel( tr("Parameter : "), this );
	m_paramList = new QComboBox( this );
	m_paramList->setEditable( true );

	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget( m_updateButton );
	hbox->addSpacing( 10 );
	hbox->addWidget( label1 );
	hbox->addWidget( m_servicesList );
	hbox->addSpacing( 10 );
	hbox->addWidget( label2 );
	hbox->addWidget( m_actionList );
	hbox->addSpacing( 10 );
	hbox->addWidget( label3 );
	hbox->addWidget( m_paramList );
	hbox->addSpacing( 10 );
	hbox->addWidget( m_runButton );
	//hbox->addStretch();

	borderLayout()->add( hbox, BorderLayout::North );

	connect( WebServicesManager::self(), SIGNAL(changed()), this, SLOT(webServicesChanged()) );
	connect( m_servicesList, SIGNAL(activated(int)), this, SLOT(webServicesActivated(int)) );
	connect( m_actionList, SIGNAL(activated(int)), this, SLOT(webServicesParamActivated(int)) );
	connect( m_paramList, SIGNAL(activated(int)), this, SLOT(webServicesValueActivated()) );

	connect( m_updateButton, SIGNAL(clicked()), WebServicesManager::self(), SLOT(updateWebServicesList()) );
	connect( m_runButton, SIGNAL(clicked()), this, SLOT(run()) );

	connect( m_paramList->lineEdit(), SIGNAL(editingFinished()), this, SLOT(paramListEditingFinished()) );

	loadServicesList();
	loadActionsList( m_servicesList->currentIndex() );
	loadValuesList( m_actionList->currentIndex() );

	setModified( false );
	updateActions();
}

WebServicesEditor::~WebServicesEditor() {

}

bool WebServicesEditor::autoIndent() {
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

const QHash<QString,QString> & WebServicesEditor::values() {
	store( m_paramList->currentText() );

	return m_paramValues;
}

WebServices * WebServicesEditor::service() {
	int index = m_servicesList->currentIndex();
	if( ( index >= 0 ) && m_servicesList->itemData( index ).isValid() )
		return static_cast<WebServices*>( m_servicesList->itemData( index ).value<void*>() );
	else
		return NULL;
}

Operation * WebServicesEditor::operation() {
	int index = m_actionList->currentIndex();
	if( ( index >= 0 ) && m_actionList->itemData( index ).isValid() )
		return (Operation*)( m_actionList->itemData( index ).value<void*>() );
	else
		return NULL;
}

class WrongFwsFormatException : public XinxException {
public:
	WrongFwsFormatException( const QString & message ) : XinxException( message ) {
	}
};

void WebServicesEditor::loadFromDevice( QIODevice & d ) {
	m_paramValues.clear(); m_oldParamValue = QString();
	try {
		QDomDocument document;
		if( ! document.setContent( &d, false ) ) throw WrongFwsFormatException( tr("Can't read content of file") );

		QDomElement element = document.documentElement();
		if( element.tagName() != "webservice" ) throw WrongFwsFormatException( tr("Not a WebServices file") );

		m_serviceName = element.attribute( "service" );
		m_operationName = element.attribute( "action" );

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

		loadServicesList();
		loadActionsList( m_servicesList->currentIndex() );
		loadValuesList( m_actionList->currentIndex() );
		restore( m_paramList->currentText() );
	} catch( WrongFwsFormatException ) {
		d.seek( 0 );
		QTextStream in( &d );
		QApplication::setOverrideCursor( Qt::WaitCursor );
		textEdit()->setPlainText( in.readAll() );
	}
}

void WebServicesEditor::saveToDevice( QIODevice & d ) {
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

	QTextStream out( &d );
	document.save( out, 3 );
}

void WebServicesEditor::serialize( XSLProjectSessionEditor * data, bool content ) {
	AbstractFileEditor::serialize( data, content );
	store( m_paramList->currentText() );

	data->writeProperty( "Service", QVariant( m_servicesList->currentText() ) );
	data->writeProperty( "Action", QVariant( m_actionList->currentText() ) );
	data->writeProperty( "Param", QVariant( m_paramList->currentText() ) );
	data->writeProperty( "Position", QVariant( textEdit()->textCursor().position() ) );

	int i = 0;
	if( content && isModified() ) {
		foreach( QString param, m_paramValues.keys() ) {
			data->writeProperty( QString( "Key_%1" ).arg( i ), QVariant( param ) );
			data->writeProperty( QString( "Value_%1" ).arg( i++ ), QVariant( m_paramValues[ param ] ) );
		}
		data->writeProperty( "KeyCount", QVariant( i ) );
	}

	i = 0;
	foreach( int line, bookmarks() ) {
		data->writeProperty( QString( "Bookmark_%1" ).arg( i++ ), QVariant( line ) );
	}
	data->writeProperty( "BookmarkCount", QVariant( i ) );
}

void WebServicesEditor::deserialize( XSLProjectSessionEditor * data ) {
	int position = 0;
	QString key, value, param;

	m_paramValues.clear(); m_oldParamValue = QString();

	position  = data->readProperty( "Position" ) .toInt();

	int bc = data->readProperty( "BookmarkCount" ).toInt();
	for( int i = 0 ; i < bc; i++ ) {
		setBookmark( data->readProperty( QString( "Bookmark_%1" ).arg( i ) ).toInt(), true );
	}

	m_serviceName = data->readProperty( "Service" ).toString();
	m_operationName = data->readProperty( "Action" ).toString();
	param = data->readProperty( "Param" ).toString();

	int pc = data->readProperty( "KeyCount" ).toInt();
	for( int i = 0 ; i < pc; i++ ) {
		QString name  = data->readProperty( QString( "Key_%1" ).arg( i ) ).toString(),
				value = data->readProperty( QString( "Value_%1" ).arg( i ) ).toString();

		m_paramValues[ name ] = value;
	}

	if( m_paramValues.keys().count() == 0 ) {
		AbstractFileEditor::deserialize( data );

		if( ! lastFileName().isEmpty() )
			loadFromFile( lastFileName() );
	} else {
		AbstractFileEditor::deserialize( data );
	}

	loadServicesList();
	loadActionsList( m_servicesList->currentIndex() );
	loadValuesList( m_actionList->currentIndex() );
	if( ! param.isEmpty() )
		m_paramList->setCurrentIndex( m_paramList->findText( param ) );
	restore( m_paramList->currentText() );

	if( m_paramValues.keys().count() > 0 )
		textEdit()->document()->setModified( isModified() );

	QTextCursor tc = textEdit()->textCursor();
	tc.setPosition( position );
	textEdit()->setTextCursor( tc );
}

QIcon WebServicesEditor::icon() const {
	return QIcon( ":/services/images/typefws.png" );
}

void WebServicesEditor::paramListEditingFinished() {
	int index = m_paramList->findText( m_paramList->lineEdit()->text() );
	if( index >= 0 ) m_paramList->setItemIcon( index, QIcon(":/services/images/serviceparam.png") );
}

void WebServicesEditor::loadServicesList() {
	m_servicesList->clear();
	foreach( WebServices * ed, *(WebServicesManager::self()) )
		if( ! ed->name().isEmpty() )
			m_servicesList->addItem( QIcon(":/services/images/services.png"), ed->name(), qVariantFromValue( (void*)ed ) );

	int findIndex = m_servicesList->findText( m_serviceName );
	if( ( ! m_serviceName.isEmpty() ) && ( findIndex == -1 ) ) {
		m_servicesList->addItem( QIcon(":/services/images/services.png"), m_serviceName );
		m_servicesList->setItemData( m_servicesList->count() - 1, Qt::gray, Qt::ForegroundRole );
		m_servicesList->setCurrentIndex( m_servicesList->count() - 1 );
	} else if( findIndex >= 0 ) {
		m_servicesList->setCurrentIndex( findIndex );
	} else if( m_servicesList->count() > 0 )
		m_servicesList->setCurrentIndex( 0 );
}

void WebServicesEditor::loadActionsList( int index ) {
	m_actionList->clear();
	if( ( index >= 0 ) && ( m_servicesList->itemData( index ).isValid() ) ) {
		WebServices * ed = (WebServices*)( m_servicesList->itemData( index ).value<void*>() );
		foreach( Operation * op, ed->operations() )
			m_actionList->addItem( QIcon(":/services/images/action.png"), op->name(), qVariantFromValue( (void*)op ) );
	}

	int findIndex = m_actionList->findText( m_operationName );
	if( ( ! m_operationName.isEmpty() ) && ( findIndex == -1 ) ) {
		m_actionList->addItem( QIcon(":/services/images/action.png"), m_operationName );
		m_actionList->setItemData( m_actionList->count() - 1, Qt::gray, Qt::ForegroundRole );
		m_actionList->setCurrentIndex( m_actionList->count() - 1 );
	} else if( findIndex >= 0 ) {
		m_actionList->setCurrentIndex( findIndex );
	} else if( m_actionList->count() > 0 )
		m_actionList->setCurrentIndex( 0 );
}

void WebServicesEditor::loadValuesList( int index ) {
	QString param = m_paramList->currentText();

	m_paramList->clear();
	if( ( index >= 0 ) && ( m_actionList->itemData( index ).isValid() ) ) {
		Operation * op = (Operation*)( m_actionList->itemData( index ).value<void*>() );
		foreach( Parameter * param, op->inputParam() )
			m_paramList->addItem( QIcon(":/services/images/serviceparam.png"), param->paramString() );
	}

	foreach( QString param, m_paramValues.keys() ) {
		if( ( !param.isEmpty() ) && m_paramList->findText( param ) == -1 ) {
			m_paramList->addItem( QIcon(":/services/images/serviceparam.png"), param );
			m_paramList->setItemData( m_paramList->count() - 1, Qt::gray, Qt::ForegroundRole );
		}
	}

	if( ! param.isEmpty() ) {
		int paramIndex = m_paramList->findText( param );
		if( paramIndex != -1 )
			m_paramList->setCurrentIndex( paramIndex );
	} else if( m_paramList->count() > 0 )
		m_paramList->setCurrentIndex( 0 );

	updateActions();
}

void WebServicesEditor::webServicesChanged() {
	loadServicesList();
	loadActionsList( m_servicesList->currentIndex() );
	loadValuesList( m_actionList->currentIndex() );
}

void WebServicesEditor::webServicesActivated( int index ) {
	setModified( true );
	loadActionsList( index );
	loadValuesList( m_actionList->currentIndex() );
}

void WebServicesEditor::webServicesParamActivated( int index ) {
	setModified( true );
	loadValuesList( index );
}

void WebServicesEditor::webServicesValueActivated() {
	if( ! m_oldParamValue.isEmpty() ) store( m_oldParamValue );
	m_oldParamValue = m_paramList->currentText();
	restore( m_oldParamValue );
}

void WebServicesEditor::store( const QString & paramStr ) {
	if( textEdit()->document()->isModified() ) setModified(true);
	m_paramValues[ paramStr ] = textEdit()->toPlainText();
}

void WebServicesEditor::restore( const QString & paramStr ) {
	textEdit()->setPlainText( m_paramValues[ paramStr ] );
}

void WebServicesEditor::run() {
	service()->call( operation(), values() );
}

void WebServicesEditor::updateActions() {
	m_runButton->setEnabled( ( service() != NULL ) && ( operation() != NULL ) );
}
