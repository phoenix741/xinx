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

// Xinx header
#include <core/xinxcore.h>
#include <core/xinxproject.h>
#include "webservices.h"
#include "wsdl.h"
#include "connectionwebservicesdialogimpl.h"
#include "serviceresultdialogimpl.h"

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

WebServices::WebServices( const QString & link, QObject * parent ) : QObject( parent ) {
	m_link = link;
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

typedef
	QPair<QString,QString> ParamStr;

void WebServices::askWSDL( QWidget * parent ) {
	QUrl wsdlUrl( m_link );
	QBuffer buffer;
//	QFile buffer( "c:\\temp.wsdl" );
	buffer.open( QIODevice::ReadWrite );

	ConnectionWebServicesDialogImpl dlg( parent );
	if( wsdlUrl.port() >= 0 )
		dlg.setHost( wsdlUrl.host(), wsdlUrl.port() );
	else
		dlg.setHost( wsdlUrl.host() );
	QString query = wsdlUrl.path();
	if( wsdlUrl.hasQuery() ) {
		query += "?";
		foreach( const ParamStr & param, wsdlUrl.queryItems() ) {
			query += param.first;
			if( !param.second.isEmpty() )
				query += "=" + param.second;
		}
	}
	if( dlg.get( query, &buffer ) ) {
		buffer.seek( 0 );

		QDomDocument document;
		QString errorStr;
		int errorLine;
		int errorColumn;
		if (!document.setContent( &buffer, true, &errorStr, &errorLine, &errorColumn)) {
			QMessageBox::information(qApp->activeWindow(), QObject::tr("WSDL WebServices file"), QObject::tr("Parse error at line %1, column %2:\n%3")
																						.arg(errorLine)
	        			                      											.arg(errorColumn)
 																						.arg(errorStr));
		    return;
		}

		loadFromElement( document.documentElement() );
	}
}

void WebServices::readResponse() {
	const QtSoapMessage & response = http.getResponse();
	if( response.isFault() ) {
		emit soapError( response.faultString().value().toString() );
		return;
	}

	QMessageBox::information( 0, tr("Test"), response.toXmlString( 2 ) );

	const QtSoapType & res = response.returnValue();
	if( ! res.isValid() ) {
		emit soapError( tr("Invalid return value") );
		return;
	}

	QMessageBox::information( 0, tr("Test"), res.value().toString() );

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
	/*

	Envelop soapEnvelop( op->encodingStyle(), op->namespaceString() , op->name() );
	QString query;

	for( int i = 0; i < op->inputParam().count(); i++ ) {
		soapEnvelop.setParam( op->inputParam()[i]->paramString(), op->inputParam()[i]->paramType(), param[ op->inputParam()[i]->paramString() ] );
		query += op->inputParam()[i]->paramString() + "=\n" + param[ op->inputParam()[i]->paramString() ] + "\n";
	}

	QUrl queryUrl( m_wsdl.services()[0].port().addressLocation() );
	QBuffer obuffer;
	obuffer.open( QIODevice::ReadWrite );

//	query = soapEnvelop.toString();
	QByteArray ibuffer = soapEnvelop.toString().toUtf8();
	ibuffer.truncate( ibuffer.size() - 1 );

	QHttpRequestHeader header( "POST", queryUrl.path() );
	header.setValue( "Host", queryUrl.host() );
	header.setValue( "Connection", "Keep-Alive" );
	header.setContentLength( ibuffer.size() );
	header.setContentType( "text/xml" );

	ConnectionWebServicesDialogImpl dlg;
	dlg.setHost( queryUrl.host(), queryUrl.port() );
	if( dlg.request( &header, &ibuffer, &obuffer ) ) {
		obuffer.seek( 0 );

		QDomDocument document;
		QString errorStr;
		int errorLine;
		int errorColumn;
		if (!document.setContent( &obuffer, true, &errorStr, &errorLine, &errorColumn)) {
			QMessageBox::information(qApp->activeWindow(), QObject::tr("Invok WebServices file"), QObject::tr("Parse error at line %1, column %2:\n%3")
																						.arg(errorLine)
	        			                      											.arg(errorColumn)
																						.arg(errorStr));
		    return;
		}

		Envelop soapResult( document.toString(), op->name() + "Response" );
		QHash<QString,QString> response;
		QStringList params = soapResult.getParams();
		foreach( const QString & param, params ) {
			QPair<QString,QString> pair = soapResult.getParam( param );
			response[ param ] = pair.first;
		}

		emit soapResponse( param, response, soapResult.getErrorCode(), soapResult.getErrorString() );
	}*/
}

/* WebServicesManager */

WebServicesManager::WebServicesManager() : QObject(), WebServicesList() {

}

WebServicesManager::WebServicesManager( const WebServicesManager & manager ) : QObject(), WebServicesList( manager ) {

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
	bool enabled = project && project->options().testFlag( XinxProject::hasWebServices );

	qDeleteAll( *this );
	clear();
	emit changed();

	if( enabled ) {
		QStringList serveurWeb = XINXProjectManager::self()->project()->readProperty( "webServiceLink" ).toString().split(";;",QString::SkipEmptyParts);
		foreach( const QString & link, serveurWeb ) {
			WebServices * ws = new WebServices( link, this );
			append( ws );
			ws->askWSDL( qApp->activeWindow() );
			connect( ws, SIGNAL(soapResponse(QHash<QString,QString>,QHash<QString,QString>,QString,QString)), this, SLOT(webServicesReponse(QHash<QString,QString>,QHash<QString,QString>,QString,QString)) );
		}
	}

	emit changed();
}

void WebServicesManager::updateWebServicesList() {
	setProject( XINXProjectManager::self()->project() );
}

void WebServicesManager::webServicesReponse( QHash<QString,QString> query, QHash<QString,QString> response, QString errorCode, QString errorString ) {
	if( ! ( errorString.isEmpty() && errorCode.isEmpty() ) ) {
		QMessageBox::warning( qApp->activeWindow(), tr("WebServices Error"), tr("Web services has error code %1 : %2").arg( errorCode ).arg( errorString ) );
	} else {
		ServiceResultDialogImpl * dlg = new ServiceResultDialogImpl( qApp->activeWindow() );
		dlg->setInputStreamText( query );
		dlg->setOutputStreamText( response );
		dlg->show();
	}
}
