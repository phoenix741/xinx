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
#include "connectionwebservicesdialogimpl.h"
#include "private/p_connectionwebservicesdialogimpl.h"

/* PrivateConnectionWebServicesDialogImpl */

PrivateConnectionWebServicesDialogImpl::PrivateConnectionWebServicesDialogImpl( ConnectionWebServicesDialogImpl * parent ) {
	m_parent = parent;
	m_http = new QHttp( this );
	
	connect( m_http, SIGNAL(dataSendProgress(int,int)), this, SLOT(setSendProgress(int,int)) );
	connect( m_http, SIGNAL(dataReadProgress(int,int)), this, SLOT(setReadProgress(int,int)) );
	connect( m_http, SIGNAL(requestFinished(int,bool)), this, SLOT(requestFinished(int,bool)) );
	connect( m_http, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)) );
}

PrivateConnectionWebServicesDialogImpl::~PrivateConnectionWebServicesDialogImpl() {
	delete m_http;
}


void PrivateConnectionWebServicesDialogImpl::requestFinished( int id, bool error ) {
	if( m_requestId == id ) {
		m_hasResult = ! error;
		if( error ) {
			m_parent->m_abortPushButton->setText( tr("Close") );
			m_parent->m_abortPushButton->disconnect();
			connect( m_parent->m_abortPushButton, SIGNAL(clicked()), m_parent, SLOT(close()) );
			
			switch( m_http->error() ) {
			case QHttp::HostNotFound:
				m_parent->m_labelState->setText( tr("The host name lookup failed.") );
				break;
			case QHttp::ConnectionRefused:
				m_parent->m_labelState->setText( tr("The server refused the connection.") );
				break;
			case QHttp::UnexpectedClose:
				m_parent->m_labelState->setText( tr("The server closed the connection unexpectedly.") );
				break;
			case QHttp::InvalidResponseHeader:
				m_parent->m_labelState->setText( tr("The server sent an invalid response header.") );
				break;
			case QHttp::WrongContentLength:
				m_parent->m_labelState->setText( tr("The client could not read the content correctly.") );
				break;
			case QHttp::Aborted:
				m_parent->m_labelState->setText( tr("The request was aborted.") );
				break;
			default:
				m_parent->m_labelState->setText( tr("An error occure.") );
			}
		} else {
			m_parent->close();
		}
	}
}

void PrivateConnectionWebServicesDialogImpl::stateChanged( int state ) {
	switch( state ) {
	case QHttp::Unconnected:
		m_parent->m_labelState->setText( tr("There is no connection to the host.") );
		m_parent->m_iconState->setPixmap( QPixmap( ":/images/network_disconnected_lan.png" ) );
		break;
	case QHttp::HostLookup:
		m_parent->m_labelState->setText( tr("A host name lookup is in progress.") );
		m_parent->m_iconState->setPixmap( QPixmap( ":/images/network_connected_lan.png" ) );
		break;
	case QHttp::Connecting:
		m_parent->m_labelState->setText( tr("An attempt to connect to the host is in progress.") );
		m_parent->m_iconState->setPixmap( QPixmap( ":/images/network_connected_lan.png" ) );
		break;
	case QHttp::Sending:
		m_parent->m_labelState->setText( tr("The client is sending its request to the server.") );
		m_parent->m_iconState->setPixmap( QPixmap( ":/images/network_outgoing_lan.png" ) );
		break;
	case QHttp::Reading:
		m_parent->m_labelState->setText( tr("The client's request has been sent and the client is reading the server's response.") );
		m_parent->m_iconState->setPixmap( QPixmap( ":/images/network_incoming_lan.png" ) );
		break;
	case QHttp::Connected:
		m_parent->m_labelState->setText( tr("The connection to the host is open.") );
		m_parent->m_iconState->setPixmap( QPixmap( ":/images/network_connected_lan.png" ) );
		break;
	case QHttp::Closing:
		m_parent->m_labelState->setText( tr("The connection is closing down.") );
		m_parent->m_iconState->setPixmap( QPixmap( ":/images/network_disconnected_lan.png" ) );
		break;
	}
	qApp->processEvents();
}

void PrivateConnectionWebServicesDialogImpl::setSendProgress( int value, int max ) {
	m_parent->m_sendProgressBar->setValue( value );
	m_parent->m_sendProgressBar->setMaximum( max );
}

void PrivateConnectionWebServicesDialogImpl::setReadProgress( int value, int max ) {
	m_parent->m_recieveProgressBar->setValue( value );
	m_parent->m_recieveProgressBar->setMaximum( max );
}


/* ConnectionWebServicesDialogImpl */

ConnectionWebServicesDialogImpl::ConnectionWebServicesDialogImpl( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f) {
	setupUi( this );
	m_sendProgressBar->setValue( 0 );
	m_recieveProgressBar->setValue( 0 );

	d = new PrivateConnectionWebServicesDialogImpl( this );	

	connect( m_abortPushButton, SIGNAL(clicked()), d->m_http, SLOT(abort()) );
}

ConnectionWebServicesDialogImpl::~ConnectionWebServicesDialogImpl() {
	delete d;
}

void ConnectionWebServicesDialogImpl::setHost( const QString & path, quint16 port ) {
	d->m_http->setHost( path, port );
}

bool ConnectionWebServicesDialogImpl::get( const QString & path, QIODevice * to ) {
	d->m_requestId = d->m_http->get( path, to );
	this->exec();
	return d->m_hasResult;
}

bool ConnectionWebServicesDialogImpl::post( const QString & path, QByteArray * data, QIODevice * to ) {
	d->m_requestId = d->m_http->post( path, *data, to );
	this->exec();
	return d->m_hasResult;
}

bool ConnectionWebServicesDialogImpl::request( QHttpRequestHeader * header, QByteArray * data, QIODevice * to ) {
	d->m_requestId = d->m_http->request( *header, *data, to );
	this->exec();
	return d->m_hasResult;
}

