/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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
#include "xinxcore.h"
#include "webservices.h"
#include "xslproject.h"
#include "wsdl.h"
#include "soap.h"
#include "connectionwebservicesdialogimpl.h"

/* Static member */

WebServicesManager * WebServicesManager::s_self = 0;

/* Parameter */

class PrivateParameter {
public:
	PrivateParameter( Parameter * parent );	
	
	QString m_paramString;
	QString m_paramType;
private:
	Parameter * m_parent;
};

PrivateParameter::PrivateParameter( Parameter * parent ) {
	m_parent = parent;
}

Parameter::Parameter( QString paramString, QString paramType ) {
	d = new PrivateParameter( this );
	d->m_paramString = paramString;
	d->m_paramType = paramType;
}

Parameter::~Parameter() {
	delete d;
}

const QString & Parameter::paramString() const { 
	return d->m_paramString;
}

const QString & Parameter::paramType() const { 
	return d->m_paramType;
}

/* Operations */

class PrivateOperation {
public:
	PrivateOperation( Operation * parent );
	~PrivateOperation();

	QString m_name;
	QString m_encodingStyle;
	QString m_namespaceString;
	
	QList<Parameter*> m_inputParam;
	QList<Parameter*> m_outputParam;
private:
	Operation * m_parent;	
friend class WebServices;
};

PrivateOperation::PrivateOperation( Operation * parent ) {
	m_parent = parent;
}

PrivateOperation::~PrivateOperation() {
	qDeleteAll( m_inputParam );
	qDeleteAll( m_outputParam );
}


Operation::Operation( QString name ) {
	d = new PrivateOperation( this );
	d->m_name = name;
}

Operation::~Operation() {
	delete d;
}

QString Operation::name() { 
	return d->m_name; 
}
	
const QList<Parameter*> & Operation::inputParam() { 
	return d->m_inputParam; 
}

const QList<Parameter*> & Operation::outputParam() { 
	return d->m_outputParam; 
}
	
QString Operation::encodingStyle() { 
	return d->m_encodingStyle; 
}

QString Operation::namespaceString() { 
	return d->m_namespaceString; 
}

/* WebServices */

class PrivateWebServices {
public:
	PrivateWebServices( WebServices * parent );
	~PrivateWebServices();

	WSDL m_wsdl;
	QString m_link;
	QList<Operation*> m_list;
private:	
	WebServices * m_parent;
};

PrivateWebServices::PrivateWebServices( WebServices * parent ) {
	m_parent = parent;	
}

PrivateWebServices::~PrivateWebServices() {
	qDeleteAll( m_list );
}

WebServices::WebServices( const QString & link, QObject * parent ) : QObject( parent ) {
	d = new PrivateWebServices( this );
	d->m_link = link;
}

WebServices::~WebServices() {
	delete d;
}

QString WebServices::name() { 
	return d->m_wsdl.name();
}

const QList<Operation*> & WebServices::operations() { 
	return d->m_list; 
}

void WebServices::loadFromElement( const QDomElement & element ) {
	qDeleteAll( d->m_list );
	d->m_list.clear();
	
	d->m_wsdl = WSDL( element );
	
	foreach( WSDLService service, d->m_wsdl.services() ) {
		QString tnsBinding = service.port().binding();
		tnsBinding = tnsBinding.mid( tnsBinding.indexOf( ":" ) + 1 );
		
		WSDLBinding binding = d->m_wsdl.bindings()[ tnsBinding ];
		QString tnsType = binding.type();
		tnsType = tnsType.mid( tnsType.indexOf( ":" ) + 1 );
		
		WSDLPortType portType = d->m_wsdl.portTypes()[ tnsType ];
		
		foreach( WSDLOperation operation, portType.operations() ) {
			Operation * wsOperation = new Operation( operation.name() );
			
			foreach( WSDLOperation bindingOperation, binding.operations() ) {
				if( bindingOperation.name() == operation.name() ) {
					wsOperation->d->m_encodingStyle = bindingOperation.inputEncodingStyle();
					wsOperation->d->m_namespaceString = bindingOperation.inputNamespace();
				}
			}
			
			QString tnsInputMessage = operation.inputMessage();
			tnsInputMessage = tnsInputMessage.mid( tnsInputMessage.indexOf( ":" ) + 1 );
			
			WSDLMessage inputMessage = d->m_wsdl.messages()[ tnsInputMessage ];
			foreach( WSDLPart part, inputMessage.parts() ) {
				Parameter * param = new Parameter( part.name(), part.type() );
				wsOperation->d->m_inputParam.append( param );
			}
			
			
			QString tnsOutputMessage = operation.outputMessage();
			tnsOutputMessage = tnsOutputMessage.mid( tnsOutputMessage.indexOf( ":" ) + 1 );

			WSDLMessage outputMessage = d->m_wsdl.messages()[ tnsOutputMessage ];
			foreach( WSDLPart part, outputMessage.parts() ) {
				Parameter * param = new Parameter( part.name(), part.type() );
				wsOperation->d->m_outputParam.append( param );
			}
			
			d->m_list.append( wsOperation );
		}
	}
	
	emit updated();
}

typedef
	QPair<QString,QString> ParamStr;

void WebServices::askWSDL( QWidget * parent ) {
	QUrl wsdlUrl( d->m_link );
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
		foreach( ParamStr param, wsdlUrl.queryItems() ) {
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

void WebServices::call( Operation * op, const QHash<QString,QString> & param ) {
	XINX_ASSERT( op );
	
	Envelop soapEnvelop( op->encodingStyle(), op->namespaceString() , op->name() );
	QString query;
	
	for( int i = 0; i < op->inputParam().count(); i++ ) {
		soapEnvelop.setParam( op->inputParam()[i]->paramString(), op->inputParam()[i]->paramType(), param[ op->inputParam()[i]->paramString() ] );
		query += op->inputParam()[i]->paramString() + "=\n" + param[ op->inputParam()[i]->paramString() ] + "\n";
	}

	QUrl queryUrl( d->m_wsdl.services()[0].port().addressLocation() );
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
		foreach( QString param, params ) {
			QPair<QString,QString> pair = soapResult.getParam( param );
			response[ param ] = pair.first;
		}
		
		emit soapResponse( param, response, soapResult.getErrorCode(), soapResult.getErrorString() );
	}
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
		XINXStaticDeleter::self()->addObject( s_self );
	}
	return s_self;
}

void WebServicesManager::listUpdated() {
	emit changed();
}



