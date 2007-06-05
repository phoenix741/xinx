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
#include <QHash>

#include "wsdl.h"

class WebServices;
class QHttp;
class QIODevice;
class QBuffer;

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
	~Operation();

	QString name() { return m_name; };
	
	const QList<Parameter*> & inputParam() { return m_inputParam; };
	const QList<Parameter*> & outputParam() { return m_outputParam; };
	
	QString encodingStyle() { return m_encodingStyle; };
	QString namespaceString() { return m_namespaceString; };
private:
	QString m_name;
	QString m_encodingStyle;
	QString m_namespaceString;
	
	QList<Parameter*> m_inputParam;
	QList<Parameter*> m_outputParam;
	
friend class WebServices;
};

class WebServices : public QObject {
	Q_OBJECT
public:
	WebServices( const QString & link, QObject * parent = 0 );
	~WebServices();

	QString name() { return m_wsdl.name(); };
	const QList<Operation*> & operations() { return m_list; };
	
	void askWSDL( QWidget * parent = 0 );
	void loadFromElement( const QDomElement & element );
	
	void call( Operation * operation, const QHash<QString,QString> & param );
signals:
	void updated();	
	void soapResponse( QString query, QString response, QString errorCode, QString errorString );
private:
	WSDL m_wsdl;
	QString m_link;
	QList<Operation*> m_list;
};

typedef QList<WebServices*> WebServicesList;

#endif // __WEBSERVICES_H__
