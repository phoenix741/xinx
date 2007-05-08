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
#include <QStringList>
#include <QDomElement>
#include <QAbstractItemModel>
#include <QHash>

class WSDLType {
	
};

class WSDLPart {
public:
	WSDLPart( const QDomElement & );

	QString name() const { return m_name; };
	QString type() const { return m_type; };
	QString element() const { return m_element; };
private:
	QString m_name;
	QString m_type;
	QString m_element;
};

class WSDLMessage {
public:
	WSDLMessage() {};
	WSDLMessage( const QDomElement & );

	QString name() const { return m_name; };
	const QList<WSDLPart> & parts() const { return m_parts; };
private:
	QString m_name;
	QList<WSDLPart> m_parts;
};

class WSDLOperation {
public:
	WSDLOperation( const QDomElement & );
	
	QString name() const { return m_name; };
	QString parameterOrder() const { return m_parameterOrder; };
	QString inputMessage() const { return m_inputMessage; };
	QString outputMessage() const { return m_outputMessage; };
private:
	QString m_name;
	QString m_parameterOrder;
	
	QString m_inputMessage;
	QString m_outputMessage;
};

class WSDLBinding {
public:
	WSDLBinding() {};
	WSDLBinding( const QDomElement & );
	
	QString name() const { return m_name; };
	QString type() const { return m_type; };
	
	const QList<WSDLOperation> & operations() const { return m_operations; };
private:
	QString m_name;
	QString m_type;
	
	QList<WSDLOperation> m_operations;
};

class WSDLPortType {
public:
	WSDLPortType() {};
	WSDLPortType( const QDomElement & );

	QString name() const { return m_name; };
	
	const QList<WSDLOperation> & operations() const { return m_operations; };
private:
	QString m_name;

	QList<WSDLOperation> m_operations;
};

class WSDLPort {
public:
	WSDLPort() {};
	WSDLPort( const QDomElement & );
	
	QString name() const { return m_name; };
	QString binding() const { return m_binding; };

	QString addressLocation() const { return m_addressLocation; };
private:
	QString m_name;
	QString m_binding;
	
	QString m_addressLocation;
};

class WSDLService {
public:
	WSDLService( const QDomElement & );

	QString name() const { return m_name; };

	const WSDLPort & port() const { return m_port; };
private:
	QString m_name;
	WSDLPort m_port;
};

class WSDL {
public:
	WSDL() {};
	WSDL( const QDomElement & );
	
	QString name() { return m_name; };
	
	const QHash<QString,WSDLType> & types() const { return m_types; };
	const QHash<QString,WSDLMessage> & messages() const { return m_messages; };
	const QHash<QString,WSDLBinding> & bindings() const { return m_bindings; };
	const QHash<QString,WSDLPortType> & portTypes() const { return m_portTypes; };
	const QList<WSDLService> & services() const { return m_services; };
private:	
	QString m_name;

	QHash<QString,WSDLType> m_types;
	QHash<QString,WSDLMessage> m_messages;
	QHash<QString,WSDLBinding> m_bindings;
	QHash<QString,WSDLPortType> m_portTypes;
	QList<WSDLService> m_services;
};

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
	
	void askWSDL( const QString & link );
	void loadFromElement( const QDomElement & element );
	
	void call( Operation operation, const QStringList & param );
signals:
	void updated();	
	void soapResponse( QString query, QString response, QString errorCode, QString errorString );
	
private slots:
	void httpRequestFinished ( int id, bool error );
	void httpSoapRequestFinished ( int id, bool error );
private:
	WSDL m_wsdl;

	QList<Operation> m_list;

	int m_requestId;
	QHttp * m_http;
	QBuffer * m_response;
	QString m_query;
};

class WebServicesModel : public QAbstractItemModel {
	Q_OBJECT
public:
	WebServicesModel( QObject *parent = 0, XSLProject * project = 0 );
	virtual ~WebServicesModel();
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;	
protected:
	void reset();
private:
	XSLProject* m_project;

friend class XSLProject;
};

#endif // __WEBSERVICES_H__
