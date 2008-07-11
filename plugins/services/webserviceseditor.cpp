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

// Qt header
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

/* WebServicesEditor */

WebServicesEditor::WebServicesEditor( QWidget *parent ) : TextFileEditor( XXX, parent ) {
	m_oldParamValue = QString();

	QLabel * label1 = new QLabel( tr("WebServices : "), this );
	m_servicesList = new QComboBox( this );

	QLabel * label2 = new QLabel( tr("Action : "), this );
	m_actionList = new QComboBox( this );

	QLabel * label3 = new QLabel( tr("Parameter : "), this );
	m_paramList = new QComboBox( this );

	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget( label1 );
	hbox->addWidget( m_servicesList );
	hbox->addSpacing( 10 );
	hbox->addWidget( label2 );
	hbox->addWidget( m_actionList );
	hbox->addSpacing( 10 );
	hbox->addWidget( label3 );
	hbox->addWidget( m_paramList );
	//hbox->addStretch();

	borderLayout()->add( hbox, BorderLayout::North );

	connect( WebServicesManager::self(), SIGNAL(changed()), this, SLOT(webServicesChanged()) );
	connect( m_servicesList, SIGNAL(activated(int)), this, SLOT(webServicesActivated(int)) );
	connect( m_actionList, SIGNAL(activated(int)), this, SLOT(webServicesParamActivated(int)) );
	connect( m_paramList, SIGNAL(activated(int)), this, SLOT(webServicesValueActivated()) );

	loadServicesList();
	loadActionsList( m_servicesList->currentIndex() );
	loadValuesList( m_actionList->currentIndex() );

	setModified( false );
}

WebServicesEditor::~WebServicesEditor() {

}

const QHash<QString,QString> & WebServicesEditor::values() {
	store( m_paramList->currentText() );

	return m_paramValues;
}

QString WebServicesEditor::getSuffix() const {
	if( lastFileName().isEmpty() )
		return "fws";
	else
		return FileEditor::getSuffix();
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

void WebServicesEditor::loadFile( const QString & fileName ){
	/* TODO : Le plugins sur les
	if( ! fileName.isEmpty() ) m_fileName = fileName;

	QFile file( getFileName() );
	if ( ! file.open( QFile::ReadOnly | QFile::Text ) ) {
		QMessageBox::warning(this, tr( "XINX" ), tr( "Cannot read file %1:\n%2." )
																.arg( getFileName() )
																.arg( file.errorString() ) );
		return;
	}

	m_paramValues.clear(); m_oldParamValue = QString();
	try {
		QDomDocument document;
		if( ! document.setContent( &file, false ) ) throw WrongFwsFormatException( "Can't read content of file" );

		QDomElement element = document.documentElement();
		if( element.tagName() != "webservice" ) throw WrongFwsFormatException( "Not a WebServices file" );

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
	} catch(WrongFwsFormatException) {
		file.seek( 0 );
		QTextStream in( &file );
		QApplication::setOverrideCursor( Qt::WaitCursor );
		m_view->setPlainText( in.readAll() );
	}

	setModified( false );

	QApplication::restoreOverrideCursor();
	*/
}

bool WebServicesEditor::saveFile( const QString & fileName ){
	/*
	if( ( fileName == m_fileName ) || fileName.isEmpty() ) createBackup( m_fileName );
	if( ! fileName.isEmpty() ) setFileName( fileName );

	QApplication::setOverrideCursor(Qt::WaitCursor);

	setIsSaving( true );

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
		activateWatcher();
		return false;
	}

	QTextStream out( &file );
	document.save( out, 3 );
	file.flush();
	file.close();

	emit modificationChanged( false );
	setModified( false );

	setIsSaving( false );

	QApplication::restoreOverrideCursor();

	return true;
	*/
}

void WebServicesEditor::serialize( XSLProjectSessionEditor * data, bool content ) {
	/*
	Editor::serialize( data, content );
	store( m_paramList->currentText() );

	data->writeProperty( "FileName", QVariant( m_fileName ) );
	data->writeProperty( "Service", QVariant( m_servicesList->currentText() ) );
	data->writeProperty( "Action", QVariant( m_actionList->currentText() ) );
	data->writeProperty( "Param", QVariant( m_paramList->currentText() ) );
	data->writeProperty( "Position", QVariant( m_view->textCursor().position() ) );
	data->writeProperty( "Modified", QVariant( isModified() ) );

	int i = 0;
	if( content && m_view->document()->isModified() ) {
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
	*/
}

void WebServicesEditor::deserialize( XSLProjectSessionEditor * data ) {
	/*
	int position = 0;
	bool isModified = false;
	QString key, value, param;

	m_paramValues.clear(); m_oldParamValue = QString();

	Editor::deserialize( data );

	m_fileName = data->readProperty( "FileName" ).toString();
	setSuffix( QFileInfo( m_fileName ).suffix() );

	position  = data->readProperty( "Position" ) .toInt();
	isModified = data->readProperty( "Modified" ).toBool();

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
		if( ! m_fileName.isEmpty() )
			loadFile( m_fileName );
	}

	loadServicesList();
	loadActionsList( m_servicesList->currentIndex() );
	loadValuesList( m_actionList->currentIndex() );
	if( ! param.isEmpty() )
		m_paramList->setCurrentIndex( m_paramList->findText( param ) );
	restore( m_paramList->currentText() );

	if( m_paramValues.keys().count() > 0 )
		m_view->document()->setModified( isModified );

	QTextCursor tc = m_view->textCursor();
	tc.setPosition( position );
	m_view->setTextCursor( tc );
	*/
}

void WebServicesEditor::setModified( bool modified ) {
	bool needEmit = modified != isModified();

	m_isModified = modified;
	m_view->document()->setModified( modified );

	if( needEmit )
		emit modificationChanged( modified );
}

bool WebServicesEditor::isModified() {
	return m_isModified || m_view->document()->isModified();
}

QIcon WebServicesEditor::icon() {
	return QIcon( ":/images/typefws.png" );
}

void WebServicesEditor::loadServicesList() {
	m_servicesList->clear();
	foreach( WebServices * ed, *(WebServicesManager::self()) )
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

void WebServicesEditor::loadActionsList( int index ) {
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

void WebServicesEditor::loadValuesList( int index ) {
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
	m_paramValues[ paramStr ] = textEdit()->toPlainText();
	setModified( isModified() );
}

void WebServicesEditor::restore( const QString & paramStr ) {
	textEdit()->setPlainText( m_paramValues[ paramStr ] );
}
