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

#include "webservicespropertydialogimpl.h"

#include "xslproject.h"
#include "webservices.h"
#include "serviceresultdialogimpl.h"
//
CrudWebServicesPropertyDialogImpl::CrudWebServicesPropertyDialogImpl( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f), m_project(NULL) {
	setupUi(this);
}
//

void CrudWebServicesPropertyDialogImpl::setProject( XSLProject * project ) {
	m_project = project;
}

void CrudWebServicesPropertyDialogImpl::on_m_addParamToolButton_clicked() {
	m_paramTableWidget->setRowCount( m_paramTableWidget->rowCount() + 1 );
	m_paramTableWidget->setCurrentCell( m_paramTableWidget->rowCount() - 1, 0 );
	m_paramTableWidget->editItem( m_paramTableWidget->item( m_paramTableWidget->rowCount() - 1, 0 ) );
}

void CrudWebServicesPropertyDialogImpl::on_m_addFieldToolButton_clicked() {
	m_setTableWidget->setRowCount( m_setTableWidget->rowCount() + 1 );
	m_setTableWidget->setCurrentCell( m_setTableWidget->rowCount() - 1, 0 );
	m_setTableWidget->editItem( m_setTableWidget->item( m_setTableWidget->rowCount() - 1, 0 ) );
}


void CrudWebServicesPropertyDialogImpl::on_m_delParamToolButton_clicked() {
	m_paramTableWidget->removeRow( m_paramTableWidget->currentRow() );
}

void CrudWebServicesPropertyDialogImpl::on_m_delFieldToolButton_clicked() {
	m_setTableWidget->removeRow( m_setTableWidget->currentRow() );
}


void CrudWebServicesPropertyDialogImpl::on_m_previousParamToolButton_clicked() {
	int currentRow = m_paramTableWidget->currentRow(), precedentRow = currentRow - 1;
	
	if( currentRow > 0 ) {
		QString precedentParamValue = m_paramTableWidget->item( precedentRow, 0 )->text();
		QString paramValue = m_paramTableWidget->item( currentRow, 0 )->text();
		
		m_paramTableWidget->setItem( precedentRow, 0, new QTableWidgetItem( paramValue ) );
		m_paramTableWidget->setItem( currentRow, 0, new QTableWidgetItem( precedentParamValue ) );

		m_paramTableWidget->setCurrentCell( precedentRow, 0 );
	}
}

void CrudWebServicesPropertyDialogImpl::on_m_previousFieldToolButton_clicked() {
	int currentRow = m_setTableWidget->currentRow(), precedentRow = currentRow - 1;
	
	if( currentRow > 0 ) {
		QString precedentFieldName = m_setTableWidget->item( precedentRow, 0 )->text();
		QString fieldName = m_setTableWidget->item( currentRow, 0 )->text();

		QString precedentFieldValue = m_setTableWidget->item( precedentRow, 1 )->text();
		QString fieldValue = m_setTableWidget->item( currentRow, 1 )->text();
		
		m_setTableWidget->setItem( precedentRow, 0, new QTableWidgetItem( fieldName ) );
		m_setTableWidget->setItem( precedentRow, 1, new QTableWidgetItem( fieldValue ) );
		m_setTableWidget->setItem( currentRow, 0, new QTableWidgetItem( precedentFieldName ) );
		m_setTableWidget->setItem( currentRow, 1, new QTableWidgetItem( precedentFieldValue ) );

		m_setTableWidget->setCurrentCell( precedentRow, 0 );
	}
}


void CrudWebServicesPropertyDialogImpl::on_m_nextParamToolButton_clicked() {
	int currentRow = m_paramTableWidget->currentRow(), nextRow = currentRow + 1;
	
	if( nextRow < m_paramTableWidget->rowCount() ) {
		QString nextParamValue = m_paramTableWidget->item( nextRow, 0 )->text();
		QString paramValue = m_paramTableWidget->item( currentRow, 0 )->text();
		
		m_paramTableWidget->setItem( nextRow, 0, new QTableWidgetItem( paramValue ) );
		m_paramTableWidget->setItem( currentRow, 0, new QTableWidgetItem( nextParamValue ) );

		m_paramTableWidget->setCurrentCell( nextRow, 0 );
	}
}

void CrudWebServicesPropertyDialogImpl::on_m_nextFieldToolButton_clicked() {
	int currentRow = m_setTableWidget->currentRow(), nextRow = currentRow + 1;
	
	if( nextRow < m_setTableWidget->rowCount() ) {
		QString nextFieldName = m_setTableWidget->item( nextRow, 0 )->text();
		QString nextFieldValue = m_setTableWidget->item( nextRow, 1 )->text();
		QString fieldName = m_setTableWidget->item( currentRow, 0 )->text();
		QString fieldValue = m_setTableWidget->item( currentRow, 1 )->text();
		
		m_setTableWidget->setItem( nextRow, 0, new QTableWidgetItem( fieldName ) );
		m_setTableWidget->setItem( nextRow, 1, new QTableWidgetItem( fieldValue ) );
		m_setTableWidget->setItem( currentRow, 0, new QTableWidgetItem( nextFieldName ) );
		m_setTableWidget->setItem( currentRow, 1, new QTableWidgetItem( nextFieldValue ) );

		m_setTableWidget->setCurrentCell( nextRow, 0 );
	}
}

QString CrudWebServicesPropertyDialogImpl::generateXMLFile() {
	QString webServicesName, serviceName, apiName;
//	webServicesName = m_webServicesNameComboBox->currentText();
//	serviceName     = m_serviceNameComboBox->currentText();
	
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
	
	QDomElement ctx = document.createElement( "ctx" );
	egx_ws.appendChild( ctx );
	
	if( ! m_entityLineEdit->text().isEmpty() )
		ctx.setAttribute( "entity", m_entityLineEdit->text() );
	if( ! m_userLineEdit->text().isEmpty() )
		ctx.setAttribute( "user", m_userLineEdit->text() );
	if( ! m_passwordLineEdit->text().isEmpty() )
		ctx.setAttribute( "password", m_passwordLineEdit->text() );
	if( ! m_targetLineEdit->text().isEmpty() )
		ctx.setAttribute( "target", m_targetLineEdit->text() );
	
	QDomElement bean_in = document.createElement( "bean_in" );
	egx_ws.appendChild( bean_in );
	
	QDomElement api = document.createElement( apiName );
	bean_in.appendChild( api );
	
	api.setAttribute( "xmlns", "http://www.generix.fr/technicalframework/businesscomponent/applicationmodule/common" );
	api.setAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
	api.setAttribute( "xsi:schemaLocation", QString("http://www.generix.fr/technicalframework/businesscomponent/applicationmodule/common %1.xsd").arg( apiName ) );
	
	api.setAttribute( "viewObjectName", m_viewObjectLineEdit->text() );
		
	if( ( serviceName == "retrieve" ) || ( serviceName == "update" ) || ( serviceName == "delete" ) || ( serviceName == "updateCreate" ) ) {
		QDomElement retrieve = document.createElement( "retrieve" );
		api.appendChild( retrieve );
		
		retrieve.setAttribute( "key", m_retrieveKeyLineEdit->text() );
		for( int i = 0 ; i < m_paramTableWidget->rowCount() ; i++ ) {
			QDomElement param = document.createElement( "param" );
			retrieve.appendChild( param );
					
			param.setAttribute( "num", QString( "%1" ).arg( i + 1 ) );
			if( ! m_paramTableWidget->item( i, 0 )->text().isEmpty() ) 
				param.setAttribute( "value", m_paramTableWidget->item( i, 0 )->text() );
			
		}
	} 
	if( ( serviceName == "create" ) || ( serviceName == "update" ) || ( serviceName == "updateCreate" ) ) {
		QDomElement row = document.createElement( "row" );
		api.appendChild( row );
		
		row.setAttribute( "num", "1" );
		
		for( int i = 0 ; i < m_setTableWidget->rowCount() ; i++ ) {
			QDomElement field = document.createElement( "field" );
			row.appendChild( field );
				
			if( ! m_setTableWidget->item( i, 0 )->text().isEmpty() )
				field.setAttribute( "name", m_setTableWidget->item( i, 0 )->text() );
			if( ! m_setTableWidget->item( i, 1 )->text().isEmpty() )
				field.setAttribute( "value", m_setTableWidget->item( i, 1 )->text() );
		}
	}
	
	return document.toString( 2 );
}


void CrudWebServicesPropertyDialogImpl::on_m_testButton_clicked() {
	ServiceResultDialogImpl * dlg = new ServiceResultDialogImpl( this );
	dlg->show();
	
	dlg->setInputStreamText( generateXMLFile() );
	
	dlg->exec();
	delete dlg;
}

