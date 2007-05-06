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
//
WebServicesPropertyDialogImpl::WebServicesPropertyDialogImpl( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f), m_project(NULL) {
	setupUi(this);
}
//

void WebServicesPropertyDialogImpl::setProject( XSLProject * project ) {
	m_project = project;
	
	m_webServicesNameComboBox->clear();
	
	if( m_project ) {
		foreach( WebServices* service, m_project->webServices() ) {
			m_webServicesNameComboBox->addItem( QIcon(":/CVstruct.png"), service->name() );
		}
	}
	on_m_webServicesNameComboBox_currentIndexChanged( m_webServicesNameComboBox->currentIndex() );
}

void WebServicesPropertyDialogImpl::on_m_webServicesNameComboBox_currentIndexChanged( int index ) {
	m_serviceNameComboBox->clear();
	if( m_project ) {
		if( m_project->webServices()[ index ]->name() == "crudManager" ) 
			m_serviceTypeComboBox->setCurrentIndex( 0 );		
		else 
			m_serviceTypeComboBox->setCurrentIndex( 1 );		
		
		foreach( Operation operation, m_project->webServices()[ index ]->operations() ) {
			m_serviceNameComboBox->addItem( QIcon(":/CVpublic_slot.png"), operation.name() );
		}
	}
}


void WebServicesPropertyDialogImpl::on_m_addParamToolButton_clicked() {
	m_paramTableWidget->setRowCount( m_paramTableWidget->rowCount() + 1 );
	m_paramTableWidget->setCurrentCell( m_paramTableWidget->rowCount() - 1, 0 );
	m_paramTableWidget->editItem( m_paramTableWidget->item( m_paramTableWidget->rowCount() - 1, 0 ) );
}

void WebServicesPropertyDialogImpl::on_m_addFieldToolButton_clicked() {
	m_setTableWidget->setRowCount( m_setTableWidget->rowCount() + 1 );
	m_setTableWidget->setCurrentCell( m_setTableWidget->rowCount() - 1, 0 );
	m_setTableWidget->editItem( m_setTableWidget->item( m_setTableWidget->rowCount() - 1, 0 ) );
}


void WebServicesPropertyDialogImpl::on_m_delParamToolButton_clicked() {
	m_paramTableWidget->removeRow( m_paramTableWidget->currentRow() );
}

void WebServicesPropertyDialogImpl::on_m_delFieldToolButton_clicked() {
	m_setTableWidget->removeRow( m_setTableWidget->currentRow() );
}


void WebServicesPropertyDialogImpl::on_m_previousParamToolButton_clicked() {
	int currentRow = m_paramTableWidget->currentRow(), precedentRow = currentRow - 1;
	
	if( currentRow > 0 ) {
		QString precedentParamValue = m_paramTableWidget->item( precedentRow, 0 )->text();
		QString paramValue = m_paramTableWidget->item( currentRow, 0 )->text();
		
		m_paramTableWidget->setItem( precedentRow, 0, new QTableWidgetItem( paramValue ) );
		m_paramTableWidget->setItem( currentRow, 0, new QTableWidgetItem( precedentParamValue ) );

		m_paramTableWidget->setCurrentCell( precedentRow, 0 );
	}
}

void WebServicesPropertyDialogImpl::on_m_previousFieldToolButton_clicked() {
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


void WebServicesPropertyDialogImpl::on_m_nextParamToolButton_clicked() {
	int currentRow = m_paramTableWidget->currentRow(), nextRow = currentRow + 1;
	
	if( nextRow < m_paramTableWidget->rowCount() ) {
		QString nextParamValue = m_paramTableWidget->item( nextRow, 0 )->text();
		QString paramValue = m_paramTableWidget->item( currentRow, 0 )->text();
		
		m_paramTableWidget->setItem( nextRow, 0, new QTableWidgetItem( paramValue ) );
		m_paramTableWidget->setItem( currentRow, 0, new QTableWidgetItem( nextParamValue ) );

		m_paramTableWidget->setCurrentCell( nextRow, 0 );
	}
}

void WebServicesPropertyDialogImpl::on_m_nextFieldToolButton_clicked() {
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

