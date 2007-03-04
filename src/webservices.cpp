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
 
#include "webservices.h"
#include "xslproject.h"

#include <QtGui>
#include <QtXml>
#include <QHttp>

WebServices::WebServices( const QString & link, QObject * parent ) : QObject( parent ), m_requestId( -1 ), m_http( NULL ), m_response( NULL ) {
	m_response = new QBuffer( this );
//	m_response = new QFile( "c:\\temp.services" , this );
	m_http = new QHttp( this );

	askWSDL( link );
}

void WebServices::loadFromElement( const QDomElement & element ) {
	m_list.clear();

	// /definitions
	QDomElement xmlDefinition = element; //element.firstChild( "definitions" );
	m_name = xmlDefinition.attribute( "name" );

	// /definitions/binding
	QDomNodeList xmlBindings = xmlDefinition.elementsByTagName( "binding" );
	for( int i = 0; i < xmlBindings.size(); i++ ) {
		QDomElement xmlBinding = xmlBindings.at( i ).toElement();
			
		// /definitions/binding[@name="WSCRUDManagerBinding"]
		if( xmlBinding.attribute( "name" ) == ( m_name + "Binding" ) ) {
			QDomNodeList xmlOperations = xmlBinding.childNodes ();
				
			for( int j = 0; j < xmlOperations.size(); j++ ) {
				if( xmlOperations.at( j ).isElement() && xmlOperations.at( j ).localName() == "operation" ) {
					// /definitions/binding[@name="WSCRUDManagerBinding"]/operation
					QDomElement xmlOperation = xmlOperations.at( j ).toElement();
					QDomElement xmlSoapAction = xmlOperation.firstChildElement( "operation" );
					
					Operation operation( xmlOperation.attribute( "name" ), xmlSoapAction.attribute( "soapAction" ) );
					
					// /definitions/message
					QDomNodeList xmlMessages = xmlDefinition.elementsByTagName( "message" );
					for( int k = 0; k < xmlMessages.size(); k++ ) {
						QDomElement xmlMessage = xmlMessages.at( k ).toElement();
						if( xmlMessage.attribute( "name" ) == operation.name() + "Input" ) {
							QDomNodeList xmlParts = xmlMessage.elementsByTagName( "part" );
							
							for( int l = 0; l < xmlParts.size(); l++ ){
								QDomElement xmlPart = xmlParts.at( l ).toElement();
								operation.m_inputParam.append( xmlPart.attribute( "name" ) );
							}
						}
						if( xmlMessage.attribute( "name" ) == operation.name() + "Output" ) {
							QDomNodeList xmlParts = xmlMessage.elementsByTagName( "part" );
						
							for( int l = 0; l < xmlParts.size(); l++ ){
								QDomElement xmlPart = xmlParts.at( l ).toElement();
								operation.m_outputParam.append( xmlPart.attribute( "name" ) );
							}
						}
					}
				
					m_list.append( operation );
					
				}
			}
				
			break;
		}
	}
	
	emit updated();
}

void WebServices::httpRequestFinished ( int id, bool error ) {
	if( ( id == m_requestId ) && ( ! error ) ) {
		m_response->seek( 0 );
		
		QDomDocument document;
		QString errorStr;
		int errorLine;
		int errorColumn;  
		if (!document.setContent( m_response, true, &errorStr, &errorLine, &errorColumn)) {
			QMessageBox::information(qApp->activeWindow(), QObject::tr("WSDL WebServices file"), QObject::tr("Parse error at line %1, column %2:\n%3")
																						.arg(errorLine)
	        			                      											.arg(errorColumn)
																						.arg(errorStr));
		    return;
		}  
		
		loadFromElement( document.documentElement() );
	}
}

void WebServices::askWSDL( const QString & link ) {
	m_response->open( QIODevice::ReadWrite );

	QUrl wsdlUrl( link );
	m_http->setHost( wsdlUrl.host(), wsdlUrl.port() );
	connect( m_http, SIGNAL( requestFinished(int,bool) ), this, SLOT(httpRequestFinished(int,bool)) );

	m_requestId = m_http->get( wsdlUrl.path() + "?WSDL", m_response ); 
}


WebServicesModel::WebServicesModel( QObject *parent, XSLProject * project ) : QAbstractItemModel( parent ), m_project( project ) {
	for( int i = 0; i < m_project->webServices().count(); i++ ) {
		connect( m_project->webServices().at( i ), SIGNAL(updated()), this, SIGNAL(layoutChanged()) );
	}
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
			WebServices * services = m_project->webServices().at( index.row() );
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
		return createIndex( row, 0, m_project->webServices().at( parent.row() ) );
	} else
		return createIndex( row, 0 );
}
	
QModelIndex WebServicesModel::parent(const QModelIndex &index) const {
	if ( !index.isValid() )
		return QModelIndex();

	if( index.internalPointer() ) {
		WebServices * services = static_cast<WebServices*>(index.internalPointer());
		return createIndex( m_project->webServices().indexOf( services ), 0 );	
	} else
		return QModelIndex();

}
	
int WebServicesModel::rowCount(const QModelIndex &parent) const {
	if( parent.isValid() ) {
		if( ! parent.internalPointer() )
			return m_project->webServices().at( parent.row() )->operations().count();
		else
			return 0;		
	} else
		return m_project->webServices().count();
}

int WebServicesModel::columnCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	
	return 1;
}

void WebServicesModel::reset() {
	for( int i = 0; i < m_project->webServices().count(); i++ ) {
		connect( m_project->webServices().at( i ), SIGNAL(updated()), this, SIGNAL(layoutChanged()) );
	}
	
	QAbstractItemModel::reset();
}
