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

// Xinx header
#include "servicesprojectwizard.h"

/* ServicesPageImpl */

ServicesPageImpl::ServicesPageImpl( int nextId, QWidget * parent ) : QWizardPage( parent ), m_nextId( nextId ) {
	setupUi( this );
	setTitle( windowTitle() );
	setSubTitle( tr("Define if the project contains WebServices. WebServices can be used to "
					"query database.") );

	registerField( "project.services", m_addWebServicesButton );
}

int ServicesPageImpl::nextId() const {
	if( m_addWebServicesButton->isChecked() )
		return m_nextId + 1;
	else
		return m_nextId + 2;
}

/* ServicesListPageImpl */

ServicesListPageImpl::ServicesListPageImpl( int nextId, QWidget * parent ) : QWizardPage( parent ), m_nextId( nextId ) {
	setupUi( this );
	setTitle( windowTitle() );
	setSubTitle( tr("Define the list of WSDL. WSDL is used to describe the web services. This"
					"list contains link to WSDL.") );

	registerField( "services.list", m_webServicesWidget );

	m_webServicesWidget->setDefaultProposedValue( "http://localhost:8888/gce/services/?WSDL" );
	m_webServicesWidget->setDefaultVisible( false );
}

QVariant ServicesListPageImpl::field( const QString &name ) const {
	return QWizardPage::field( name );
}

int ServicesListPageImpl::nextId() const {
	return m_nextId + 2;
}
