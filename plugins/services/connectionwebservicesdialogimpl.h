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

#ifndef CONNECTIONWEBSERVICESDIALOGIMPL_H
#define CONNECTIONWEBSERVICESDIALOGIMPL_H
#pragma once

// Xinx header
#include "ui_servicesconnection.h"

// Qt header
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
	 * \param parent Parent of the dialog
	 * \param f Flags to use on Windows. By default, the dialog have a fixed size.
	 */
	ConnectionWebServicesDialogImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	/*!
	 * Destructor of the WebServices Connection dialog.
	 */
	virtual ~ConnectionWebServicesDialogImpl();

	/*!
	 * Define the host and the port of the connection. This param is used to configure the internal QHttp.
	 * \param path The serveur who wants to connect.
	 * \param port Port to connect ( the default is 80 )
	 */
	void setHost( const QString & path, quint16 port = 80 );
	/*!
	 * Call the get request of the QHttp, with a path. The dialog show the progression
	 * of the connection (downloading, and uploading).
	 * \param path Path on the server where we want to send data.
	 * \param to Device where the content will be returned.
	 */
	bool get( const QString & path, QIODevice * to );
	/*!
	 * Call the post request of the QHttp, with a path. The data to post at the QHttp is stored in a
	 * QByteArray. The dialog show the progression of the connection (downloading, and uploading).
	 * \param path Path on the server to connect
	 * \param data Data to post to the server.
	 * \param to Device where the content will be returned.
	 */
	bool post( const QString & path, QByteArray * data, QIODevice * to );
	/*!
	 * Call the request method of the QHttp. We used this method to defines user request.
	 * \param header The header contains path, and other header information.
	 * \param data Data to be send to the server.
	 * \param to Device where the content will be returned.
	 */
	bool request( QHttpRequestHeader * header, QByteArray * data, QIODevice * to );

private slots:
	void on_m_buttonBox_clicked( QAbstractButton * button );
	void requestFinished( int id, bool error );
	void stateChanged( int state );
	void setSendProgress( int value, int max );
	void setReadProgress( int value, int max );
private:
	int m_requestId;
	QHttp * m_http;
	bool m_hasResult;
};
#endif





