/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Qt header
#include <QBuffer>
#include <QFile>
#include <QIODevice>
#include <QHttp>
#include <QMessageBox>
#include <QApplication>
#include <QUrl>
#include <QIcon>
#include <QDomElement>
#include <QPair>
#include <QProgressDialog>

// Xinx header
#include <core/xinxcore.h>
#include <core/xinxproject.h>
#include "webservices.h"
#include "wsdl.h"

/* Static member */

WebServicesManager * WebServicesManager::s_self = 0;

/* Parameter */

Parameter::Parameter( QString paramString, QString paramType ) {
	m_paramString = paramString;
	m_paramType = paramType;
}

Parameter::~Parameter() {

}

const QString & Parameter::paramString() const {
	return m_paramString;
}

const QString & Parameter::paramType() const {
	return m_paramType;
}

/* Operations */

Operation::Operation( QString name ) {
	m_name = name;
}

Operation::~Operation() {
	qDeleteAll( m_inputParam );
	qDeleteAll( m_outputParam );
}

QString Operation::name() {
	return m_name;
}

const QList<Parameter*> & Operation::inputParam() {
	return m_inputParam;
}

const QList<Parameter*> & Operation::outputParam() {
	return m_outputParam;
}

QString Operation::encodingStyle() {
	return m_encodingStyle;
}

QString Operation::namespaceString() {
	return m_namespaceString;
}

/* WebServices */

WebServices::WebServices( const QString & wsdlLink, const QString & wsdlContent, QObject * parent ) : QObject( parent ), m_wsdlLink( wsdlLink ), m_wsdlContent( wsdlContent ) {
	connect( &http, SIGNAL(responseReady()), SLOT(readResponse()) );
}

WebServices::~WebServices() {
	qDeleteAll( m_list );
}

QString WebServices::name() {
	return m_wsdl.name();
}

const QList<Operation*> & WebServices::operations() {
	return m_list;
}

void WebServices::loadFromElement( const QDomElement & element ) {
	qDeleteAll( m_list );
	m_list.clear();

	m_wsdl = WSDL( element );

	foreach( const WSDLService & service, m_wsdl.services() ) {
		QString tnsBinding = service.port().binding();
		tnsBinding = tnsBinding.mid( tnsBinding.indexOf( ":" ) + 1 );

		WSDLBinding binding = m_wsdl.bindings()[ tnsBinding ];
		QString tnsType = binding.type();
		tnsType = tnsType.mid( tnsType.indexOf( ":" ) + 1 );

		WSDLPortType portType = m_wsdl.portTypes()[ tnsType ];

		foreach( const WSDLOperation & operation, portType.operations() ) {
			Operation * wsOperation = new Operation( operation.name() );

			foreach( const WSDLOperation & bindingOperation, binding.operations() ) {
				if( bindingOperation.name() == operation.name() ) {
					wsOperation->m_encodingStyle = bindingOperation.inputEncodingStyle();
					wsOperation->m_namespaceString = bindingOperation.inputNamespace();
				}
			}

			QString tnsInputMessage = operation.inputMessage();
			tnsInputMessage = tnsInputMessage.mid( tnsInputMessage.indexOf( ":" ) + 1 );

			WSDLMessage inputMessage = m_wsdl.messages()[ tnsInputMessage ];
			foreach( const WSDLPart & part, inputMessage.parts() ) {
				Parameter * param = new Parameter( part.name(), part.type() );
				wsOperation->m_inputParam.append( param );
			}


			QString tnsOutputMessage = operation.outputMessage();
			tnsOutputMessage = tnsOutputMessage.mid( tnsOutputMessage.indexOf( ":" ) + 1 );

			WSDLMessage outputMessage = m_wsdl.messages()[ tnsOutputMessage ];
			foreach( const WSDLPart & part, outputMessage.parts() ) {
				Parameter * param = new Parameter( part.name(), part.type() );
				wsOperation->m_outputParam.append( param );
			}

			m_list.append( wsOperation );
		}
	}

	emit updated();
}

void WebServices::loadFromContent( const QString & wsdlContent ) {
	QDomDocument document;
	QString errorStr;
	int errorLine;
	int errorColumn;
	if (!document.setContent( wsdlContent, true, &errorStr, &errorLine, &errorColumn)) {
		QMessageBox::information(qApp->activeWindow(), QObject::tr("WSDL WebServices file"), QObject::tr("Parse error at line %1, column %2:\n%3")
		                         .arg(errorLine)
		                         .arg(errorColumn)
		                         .arg(errorStr));
		return;
	}

	loadFromElement( document.documentElement() );
}

void WebServices::readResponse() {
	const QtSoapMessage & response = http.getResponse();
	if( response.isFault() ) {
		emit soapError( response.faultString().value().toString() );
		return;
	}

	const QtSoapType & res = response.returnValue();
	if( ! res.isValid() ) {
		emit soapError( tr("Invalid return value") );
		return;
	}

	QHash<QString,QString> hashResponse;
	if( res.count() > 0 ) {
		QDomDocument document;
		QDomElement rootElement = res.toDomElement( document );
		QDomElement childElement = rootElement.firstChildElement();
		while( ! childElement.isNull() ) {
			hashResponse[ childElement.nodeName() ] = childElement.text();
			childElement = childElement.nextSiblingElement();
		}
	} else {
		hashResponse[ res.name().name() ] = res.value().toString();
	}

	emit soapResponse( hashResponse );
}

void WebServices::call( Operation * op, const QHash<QString,QString> & param ) {
	Q_ASSERT( op );

	m_namespace = op->namespaceString();

	QtSoapMessage request;
	request.setMethod( QtSoapQName( op->name(), m_namespace ) );

	QHashIterator<QString,QString> i( param );
	while( i.hasNext() ) {
		i.next();
		request.addMethodArgument( i.key(), op->namespaceString(), i.value() );
	}

	QUrl queryUrl( m_wsdl.services()[0].port().addressLocation() );
	http.setHost( queryUrl.host(), queryUrl.port() );

	http.submitRequest( request, queryUrl.path() );
}

/* WebServicesManager */

WebServicesManager::WebServicesManager() : QObject(), WebServicesList(), m_isUpdate( false ){
	m_http = new QHttp( this );
	m_httpDialog = new QProgressDialog( qApp->activeWindow() );
	m_httpDialog->setLabelText( tr("Load Web Services List ...") );
	m_httpDialog->setValue( 0 );
	connect( m_httpDialog, SIGNAL( canceled() ), m_http, SLOT( abort() ) );
	connect( m_http, SIGNAL( done(bool) ), this, SLOT( responseReady() ) );
}

WebServicesManager::WebServicesManager( const WebServicesManager & manager ) : QObject(), WebServicesList( manager ), m_isUpdate( false ) {
	m_http = new QHttp( this );
	m_httpDialog = new QProgressDialog( qApp->activeWindow() );
	m_httpDialog->setLabelText( tr("Load Web Services List ...") );
	m_httpDialog->setValue( 0 );
	connect( m_httpDialog, SIGNAL( canceled() ), m_http, SLOT( abort() ) );
	connect( m_http, SIGNAL( done(bool) ), this, SLOT( responseReady() ) );
}

WebServicesManager::~WebServicesManager() {
	if( this == s_self ) {
		s_self = 0;
		qDeleteAll( *this );
	}
}

WebServicesManager * WebServicesManager::self() {
	if( s_self == 0 ) {
		s_self = new WebServicesManager();
		connect( XINXProjectManager::self(), SIGNAL(changed()), s_self, SLOT(updateWebServicesList()) );
		XINXStaticDeleter::self()->addObject( s_self );
	}
	return s_self;
}

void WebServicesManager::setProject( XinxProject * project ) {
	if( m_isUpdate ) return ;
	bool enabled = project && project->options().testFlag( XinxProject::hasWebServices );

	qDeleteAll( *this );
	clear();
	emit changed();

	if( enabled ) {
		QHash<QString,QString> wsdlContent;
		int index = 0;
		QString link;

		QStringList serveurWeb = XINXProjectManager::self()->project()->readProperty( "webServiceLink" ).toString().split( ";;", QString::SkipEmptyParts );
		while( ! ( link = XINXProjectManager::self()->project()->readProperty( QString( "webServiceLink_%1" ).arg( index ) ).toString() ).isEmpty() ) {
			if( index == 0 )
				serveurWeb.clear();

			serveurWeb.append( link );
			wsdlContent[ link ] = XINXProjectManager::self()->project()->readProperty( QString( "webServiceContent_%1" ).arg( index ) ).toString();

			index++;
		}

		m_httpDialog->setMaximum( serveurWeb.count() );
		m_httpDialog->setValue( 0 );
		m_httpDialog->show();

		m_isUpdate = true;
		foreach( QString link, serveurWeb ) {
			if( wsdlContent[ link ].isEmpty() ) {
				m_hasFinished = false;
				m_httpString = link;
				QUrl queryUrl( link );
				m_http->setHost( queryUrl.host(), queryUrl.port() );
				m_http->get( queryUrl.path() );
				while( ! m_hasFinished )
					qApp->processEvents();
			} else {
				append( new WebServices( link, wsdlContent[ link ], this ) );
				m_httpDialog->setValue( m_httpDialog->value() + 1 );
			}
		}
		m_isUpdate = false;
		m_httpDialog->reset();
	}

	emit changed();
}

void WebServicesManager::responseReady() {
	if( m_http->error() == QHttp::NoError ) {
		QString content = m_http->readAll();
		if( ! content.isEmpty() ) {
			XINXProjectManager::self()->project()->writeProperty( QString( "webServiceContent_%1" ).arg( m_httpDialog->value() ), content );
			append( new WebServices( m_httpString, content, this ) );
		}
	} else {
		QMessageBox::critical( qApp->activeWindow(), tr( "WSDL WebServices file" ), tr( "Can't load WSDL of link %1 : %2" ).arg( m_httpString ).arg( m_http->errorString() ) );
	}

	m_httpDialog->setValue( m_httpDialog->value() + 1 );
	m_hasFinished = true;
}

void WebServicesManager::updateWebServicesList() {
	setProject( XINXProjectManager::self()->project() );
}


