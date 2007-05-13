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

class ConnectionWebServicesDialogImpl : public QDialog, public Ui::ConnectionWebServicesDialog {
	Q_OBJECT
public:
	enum QueryType { qtGet, qtPost };

	ConnectionWebServicesDialogImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	virtual ~ConnectionWebServicesDialogImpl();

	void setHost( const QString & path, quint16 port = 80 );
	bool get( const QString & path, QIODevice * to );
	bool post( const QString & path, QByteArray * data, QIODevice * to );
	bool request( QHttpRequestHeader * header, QByteArray * data, QIODevice * to );
private slots:
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





