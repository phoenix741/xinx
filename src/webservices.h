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
 
#ifndef __WEBSERVICES_H__
#define __WEBSERVICES_H__

#include <QString>
#include <QDomElement>
#include <QAbstractItemModel>

#include "wsdl.h"

class WebServices;
class QHttp;
class QIODevice;
class QBuffer;
class XSLProject;

class Parameter {
public:
	Parameter( QString ParamString, QString ParamType ) : m_paramString( ParamString ), m_paramType( ParamType ) { };
	const QString & paramString() const { return m_paramString; };
	const QString & paramType() const { return m_paramType; };
private:
	QString m_paramString;
	QString m_paramType;
};

class Operation {
public:
	Operation( QString name ) : m_name( name ) {};

	QString name() { return m_name; };
	
	const QList<Parameter> & inputParam() { return m_inputParam; };
	const QList<Parameter> & outputParam() { return m_outputParam; };
private:
	QString m_name;
	
	QList<Parameter> m_inputParam;
	QList<Parameter> m_outputParam;
	
friend class WebServices;
};

class WebServices : public QObject {
	Q_OBJECT
public:
	WebServices( const QString & link, QObject * parent = 0 );

	QString name() { return m_wsdl.name(); };
	const QList<Operation> & operations() { return m_list; };
	
	void askWSDL( QWidget * parent = 0 );
	void loadFromElement( const QDomElement & element );
	
	void call( Operation operation, const QStringList & param );
signals:
	void updated();	
	void soapResponse( QString query, QString response, QString errorCode, QString errorString );
	
private slots:
	void httpSoapRequestFinished ( int id, bool error );
private:
	WSDL m_wsdl;
	QString m_link;
	QList<Operation> m_list;

	int m_requestId;
	QHttp * m_http;
	QBuffer * m_response;
	QString m_query;
};

typedef QList<WebServices*> WebServicesList;

class WebServicesModel : public QAbstractItemModel {
	Q_OBJECT
public:
	WebServicesModel( QObject *parent = 0, WebServicesList * list = 0 );
	virtual ~WebServicesModel();
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;	

	void reset( WebServicesList * list = 0 );
private:
	WebServicesList * m_list;
};

#endif // __WEBSERVICES_H__
