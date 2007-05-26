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

#include "newwebservicesdialogimpl.h"

#include <assert.h>

#include "globals.h"
#include "xslproject.h"
#include "webservices.h"
//
NewWebServicesDialogImpl::NewWebServicesDialogImpl( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f) {
	setupUi(this);

	m_webServicesNameComboBox->clear();
	foreach( WebServices* service, *(global.m_webServices) ) 
		m_webServicesNameComboBox->addItem( QIcon(":/services.png"), service->name() );

	on_m_webServicesNameComboBox_currentIndexChanged( m_webServicesNameComboBox->currentIndex() );
}
//

void NewWebServicesDialogImpl::on_m_webServicesNameComboBox_currentIndexChanged( int index ) {
	assert( global.m_webServices != NULL );
	m_serviceNameComboBox->clear();

	foreach( Operation operation, (*(global.m_webServices))[ index ]->operations() ) {
		m_serviceNameComboBox->addItem( QIcon(":/action.png"), operation.name() );
	}
}

WebServices * NewWebServicesDialogImpl::calledWebServices() {
	QString webServicesName = m_webServicesNameComboBox->currentText();
	
	for( int i = 0; i < global.m_webServices->count(); i++ ) {
		if( (*(global.m_webServices))[ i ]->name() == webServicesName ) {
			return (*(global.m_webServices))[ i ];
		}
	}
	
	return NULL;
}

Operation NewWebServicesDialogImpl::calledOperation(){
	QString serviceName = m_serviceNameComboBox->currentText();
	WebServices * s = calledWebServices();
	
	for( int j = 0; j < s->operations().count(); j++ ) {
		Operation op = (s->operations())[ j ];
		if( op.name() == serviceName ) {
			return op;
		}
	}
	
	return Operation("");
}


QString NewWebServicesDialogImpl::generateXMLFile() {
	QString webServicesName, serviceName, apiName;
	webServicesName = m_webServicesNameComboBox->currentText();
	serviceName     = m_serviceNameComboBox->currentText();
	
	if( webServicesName == "crudManager" )
		apiName = "CRUDManager" + serviceName + "In";
	else
	if( webServicesName == "gCEService" )
		apiName = "GCEService" + serviceName + "In";
	else
		apiName = webServicesName + serviceName + "In";
	
	QDomDocument document;
	QDomElement egx_ws = document.createElement( "egx_ws" );
	document.appendChild( egx_ws );
	
	QDomElement bean_in = document.createElement( "bean_in" );
	egx_ws.appendChild( bean_in );
	
	QDomElement api = document.createElement( apiName );
	bean_in.appendChild( api );
	
	api.setAttribute( "xmlns", "http://www.generix.fr/technicalframework/businesscomponent/applicationmodule/common" );
	api.setAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
	api.setAttribute( "xsi:schemaLocation", QString("http://www.generix.fr/technicalframework/businesscomponent/applicationmodule/common %1.xsd").arg( apiName ) );
	
	return document.toString( 2 );
}

