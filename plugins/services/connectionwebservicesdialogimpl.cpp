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

// Xinx header
#include "connectionwebservicesdialogimpl.h"

// Qt header
#include <QHttp>

/* ConnectionWebServicesDialogImpl */

ConnectionWebServicesDialogImpl::ConnectionWebServicesDialogImpl( QWidget * parent, Qt::WFlags f)
	: QDialog(parent, f) {
	setupUi( this );
	m_sendProgressBar->setValue( 0 );
	m_recieveProgressBar->setValue( 0 );

	m_http = new QHttp( this );

	connect( m_http, SIGNAL(dataSendProgress(int,int)), this, SLOT(setSendProgress(int,int)) );
	connect( m_http, SIGNAL(dataReadProgress(int,int)), this, SLOT(setReadProgress(int,int)) );
	connect( m_http, SIGNAL(requestFinished(int,bool)), this, SLOT(requestFinished(int,bool)) );
	connect( m_http, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)) );
}

ConnectionWebServicesDialogImpl::~ConnectionWebServicesDialogImpl() {
	delete m_http;
}

void ConnectionWebServicesDialogImpl::on_m_buttonBox_clicked( QAbstractButton * button ) {
	if( m_buttonBox->standardButton( button ) == QDialogButtonBox::Abort ) {
		m_http->abort();
	} else if( m_buttonBox->standardButton( button ) == QDialogButtonBox::Close ) {
		close();
	}
}

void ConnectionWebServicesDialogImpl::setHost( const QString & path, quint16 port ) {
	m_http->setHost( path, port );
}

bool ConnectionWebServicesDialogImpl::get( const QString & path, QIODevice * to ) {
	m_requestId = m_http->get( path, to );
	this->exec();
	return m_hasResult;
}

bool ConnectionWebServicesDialogImpl::post( const QString & path, QByteArray * data, QIODevice * to ) {
	m_requestId = m_http->post( path, *data, to );
	this->exec();
	return m_hasResult;
}

bool ConnectionWebServicesDialogImpl::request( QHttpRequestHeader * header, QByteArray * data, QIODevice * to ) {
	m_requestId = m_http->request( *header, *data, to );
	this->exec();
	return m_hasResult;
}

void ConnectionWebServicesDialogImpl::requestFinished( int id, bool error ) {
	if( m_requestId == id ) {
		m_hasResult = ! error;
		if( error ) {
			m_buttonBox->setStandardButtons( QDialogButtonBox::Close );

			switch( m_http->error() ) {
			case QHttp::HostNotFound:
				m_labelState->setText( tr("The host name lookup failed.") );
				break;
			case QHttp::ConnectionRefused:
				m_labelState->setText( tr("The server refused the connection.") );
				break;
			case QHttp::UnexpectedClose:
				m_labelState->setText( tr("The server closed the connection unexpectedly.") );
				break;
			case QHttp::InvalidResponseHeader:
				m_labelState->setText( tr("The server sent an invalid response header.") );
				break;
			case QHttp::WrongContentLength:
				m_labelState->setText( tr("The client could not read the content correctly.") );
				break;
			case QHttp::Aborted:
				m_labelState->setText( tr("The request was aborted.") );
				break;
			default:
				m_labelState->setText( tr("An error occure.") );
			}
		} else {
			close();
		}
	}
}

void ConnectionWebServicesDialogImpl::stateChanged( int state ) {
	switch( state ) {
	case QHttp::Unconnected:
		m_labelState->setText( tr("There is no connection to the host.") );
		m_iconState->setPixmap( QPixmap( ":/images/network_disconnected_lan.png" ) );
		break;
	case QHttp::HostLookup:
		m_labelState->setText( tr("A host name lookup is in progress.") );
		m_iconState->setPixmap( QPixmap( ":/images/network_connected_lan.png" ) );
		break;
	case QHttp::Connecting:
		m_labelState->setText( tr("An attempt to connect to the host is in progress.") );
		m_iconState->setPixmap( QPixmap( ":/images/network_connected_lan.png" ) );
		break;
	case QHttp::Sending:
		m_labelState->setText( tr("The client is sending its request to the server.") );
		m_iconState->setPixmap( QPixmap( ":/images/network_outgoing_lan.png" ) );
		break;
	case QHttp::Reading:
		m_labelState->setText( tr("The client's request has been sent and the client is reading the server's response.") );
		m_iconState->setPixmap( QPixmap( ":/images/network_incoming_lan.png" ) );
		break;
	case QHttp::Connected:
		m_labelState->setText( tr("The connection to the host is open.") );
		m_iconState->setPixmap( QPixmap( ":/images/network_connected_lan.png" ) );
		break;
	case QHttp::Closing:
		m_labelState->setText( tr("The connection is closing down.") );
		m_iconState->setPixmap( QPixmap( ":/images/network_disconnected_lan.png" ) );
		break;
	}
	qApp->processEvents();
}

void ConnectionWebServicesDialogImpl::setSendProgress( int value, int max ) {
	m_sendProgressBar->setValue( value );
	m_sendProgressBar->setMaximum( max );
}

void ConnectionWebServicesDialogImpl::setReadProgress( int value, int max ) {
	m_recieveProgressBar->setValue( value );
	m_recieveProgressBar->setMaximum( max );
}

