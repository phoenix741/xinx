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

#include <assert.h>

#include "webservices.h"
#include "xslproject.h"
#include "wsdl.h"
#include "soap.h"
#include "connectionwebservicesdialogimpl.h"

WebServices::WebServices( const QString & link, QObject * parent ) : QObject( parent ), m_link ( link ) {
}

void WebServices::loadFromElement( const QDomElement & element ) {
	m_list.clear();
	
	m_wsdl = WSDL( element );
	
	foreach( WSDLService service, m_wsdl.services() ) {
		QString tnsBinding = service.port().binding();
		tnsBinding = tnsBinding.mid( tnsBinding.indexOf( ":" ) + 1 );
		
		WSDLBinding binding = m_wsdl.bindings()[ tnsBinding ];
		QString tnsType = binding.type();
		tnsType = tnsType.mid( tnsType.indexOf( ":" ) + 1 );
		
		WSDLPortType portType = m_wsdl.portTypes()[ tnsType ];
		
		foreach( WSDLOperation operation, portType.operations() ) {
			Operation wsOperation ( operation.name() );
			
			foreach( WSDLOperation bindingOperation, binding.operations() ) {
				if( bindingOperation.name() == operation.name() ) {
					wsOperation.m_encodingStyle = bindingOperation.inputEncodingStyle();
					wsOperation.m_namespaceString = bindingOperation.inputNamespace();
				}
			}
			
			QString tnsInputMessage = operation.inputMessage();
			tnsInputMessage = tnsInputMessage.mid( tnsInputMessage.indexOf( ":" ) + 1 );
			
			WSDLMessage inputMessage = m_wsdl.messages()[ tnsInputMessage ];
			foreach( WSDLPart part, inputMessage.parts() ) {
				Parameter param( part.name(), part.type() );
				wsOperation.m_inputParam.append( param );
			}
			
			
			QString tnsOutputMessage = operation.outputMessage();
			tnsOutputMessage = tnsOutputMessage.mid( tnsOutputMessage.indexOf( ":" ) + 1 );

			WSDLMessage outputMessage = m_wsdl.messages()[ tnsOutputMessage ];
			foreach( WSDLPart part, outputMessage.parts() ) {
				Parameter param( part.name(), part.type() );
				wsOperation.m_outputParam.append( param );
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
	dlg.setHost( wsdlUrl.host(), wsdlUrl.port() );
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

void WebServices::call( Operation op, const QStringList & param ) {
	Envelop soapEnvelop( op.encodingStyle(), op.namespaceString() , op.name() );
	QString query;
	
	for( int i = 0; i < op.inputParam().count(); i++ ) {
		soapEnvelop.setParam( op.inputParam()[i].paramString(), op.inputParam()[i].paramType (), param[i] );
		query += op.inputParam()[i].paramString() + "=\n" + param[i] + "\n";
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
		
		emit soapResponse( query, document.toString(), QString(), QString() );
	}
}


/* WebServicesModel */

WebServicesModel::WebServicesModel( QObject *parent, WebServicesList * list ) : QAbstractItemModel( parent ), m_list( list ) {
	assert( list != NULL );
	
	foreach( WebServices * ws, *m_list ) 
		connect( ws, SIGNAL(updated()), this, SIGNAL(layoutChanged()) );
}

WebServicesModel::~WebServicesModel() {
	
}
	
QVariant WebServicesModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid()) return QVariant();

	if( index.internalPointer() ) {
		if( role == Qt::DecorationRole && index.column() == 0 ) {
			return QIcon(":/CVpublic_slot.png");
		} 	
		if( role == Qt::DisplayRole && index.column() == 0 ) {
			WebServices * services = static_cast<WebServices*>( index.internalPointer() );
			Operation operation = services->operations().at( index.row() );
			return operation.name();
		}
	} else {
		if( role == Qt::DecorationRole && index.column() == 0 ) {
			return QIcon(":/CVstruct.png");
		} 
		if( role == Qt::DisplayRole && index.column() == 0 ) {
			WebServices * services = m_list->at( index.row() );
			return services->name();
		}
	}
	
	return QVariant();
}
	
Qt::ItemFlags WebServicesModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;	
}
	
QModelIndex WebServicesModel::index(int row, int column, const QModelIndex &parent) const {
	if( column > 0 ) return QModelIndex();
		
	if( parent.isValid() ) {
		return createIndex( row, 0, m_list->at( parent.row() ) );
	} else
		return createIndex( row, 0 );
}
	
QModelIndex WebServicesModel::parent(const QModelIndex &index) const {
	if ( !index.isValid() )
		return QModelIndex();

	if( index.internalPointer() ) {
		WebServices * services = static_cast<WebServices*>(index.internalPointer());
		return createIndex( m_list->indexOf( services ), 0 );	
	} else
		return QModelIndex();

}
	
int WebServicesModel::rowCount(const QModelIndex &parent) const {
	if( parent.isValid() ) {
		if( ! parent.internalPointer() )
			return m_list->at( parent.row() )->operations().count();
		else
			return 0;		
	} else
		return m_list->count();
}

int WebServicesModel::columnCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	
	return 1;
}

void WebServicesModel::reset( WebServicesList * list ) {
	assert( ( m_list != NULL ) || ( list != NULL ) );
	if( list ) m_list = list;
		
	foreach( WebServices * ws, *m_list ) 
		connect( ws, SIGNAL(updated()), this, SIGNAL(layoutChanged()) );
	
	QAbstractItemModel::reset();
}
