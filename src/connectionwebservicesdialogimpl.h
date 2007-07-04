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

#ifndef CONNECTIONWEBSERVICESDIALOGIMPL_H
#define CONNECTIONWEBSERVICESDIALOGIMPL_H
//
#include "ui_servicesconnection.h"
//
class QHttp;
class QIODevice;
class QHttpRequestHeader;

class PrivateConnectionWebServicesDialogImpl;

/*!
 * This is the implementation of a dialog who show the progress of the connection to the WebServices.
 * This dialog initiate the connection and make the request. The data is returned in a QIODevice.
 */
class ConnectionWebServicesDialogImpl : public QDialog, public Ui::ConnectionWebServicesDialog {
	Q_OBJECT
public:
	/*! 
	 * Constructor of the WebServices Connection dialog. 
	 * The dialog as a fixed size.
	 */
	ConnectionWebServicesDialogImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	/*!
	 * Destructor of the WebServices Connection dialog.
	 */
	virtual ~ConnectionWebServicesDialogImpl();

	/*!
	 * Define the host and the port of the connection.
	 * \param path The serveur who wants to connect.
	 * \param port Port to connect ( the default is 80 )
	 */
	void setHost( const QString & path, quint16 port = 80 );
	/*!
	 * Make a HTTP Get request.
	 * \param path Path on the server to connect
	 * \param to Contents returned when the request is terminated.
	 */
	bool get( const QString & path, QIODevice * to );
	/*!
	 * Make a HTTP Post request.
	 * \param path Path on the server to connect
	 * \param data Data to be send to the server.
	 * \param to Contents returned when the request is terminated.
	 */
	bool post( const QString & path, QByteArray * data, QIODevice * to );
	/*!
	 * Make a HTTP Defined Request.
	 * \param header The header contains path, and other header information.
	 * \param data Data to be send to the server.
	 * \param to Contents returned when the request is terminated.
	 */
	bool request( QHttpRequestHeader * header, QByteArray * data, QIODevice * to );
private:
	PrivateConnectionWebServicesDialogImpl * d;
	friend class PrivateConnectionWebServicesDialogImpl;
};
#endif





