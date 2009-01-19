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

#ifndef __WEBSERVICES_H__
#define __WEBSERVICES_H__

#include <QString>
#include <QDomElement>
#include <QAbstractItemModel>
#include <QHash>

#include "wsdl.h"

class WebServices;

class PrivateParameter;

class Parameter {
public:
	Parameter( QString paramString, QString paramType );
	virtual ~Parameter();
	const QString & paramString() const;
	const QString & paramType() const;
private:
	PrivateParameter * d;
	friend class PrivateParameter;
};

class PrivateOperation;

class Operation {
public:
	Operation( QString name );
	virtual ~Operation();

	QString name();

	const QList<Parameter*> & inputParam();
	const QList<Parameter*> & outputParam();

	QString encodingStyle();
	QString namespaceString();
private:
	PrivateOperation * d;
	friend class PrivateOperation;
	friend class WebServices;
};

class PrivateWebServices;

class WebServices : public QObject {
	Q_OBJECT
public:
	WebServices( const QString & link, QObject * parent = 0 );
	virtual ~WebServices();

	QString name();
	const QList<Operation*> & operations();

	void askWSDL( QWidget * parent = 0 );
	void loadFromElement( const QDomElement & element );

	void call( Operation * operation, const QHash<QString,QString> & param );
signals:
	void updated();
	void soapResponse( QHash<QString,QString> query, QHash<QString,QString> response, QString errorCode, QString errorString );
private:
	PrivateWebServices * d;
	friend class PrivateWebServices;
};

typedef QList<WebServices*> WebServicesList;

class XSLProject;

class WebServicesManager : public QObject, public WebServicesList {
	Q_OBJECT
public:
	WebServicesManager( const WebServicesManager & manager );
	WebServicesManager();
	virtual ~WebServicesManager();

	void setProject( XSLProject * project );

	static WebServicesManager * self();
public slots:
	void updateWebServicesList();
signals:
	void changed();
private slots:
	void webServicesReponse( QHash<QString,QString> query, QHash<QString,QString> response, QString errorCode, QString errorString );
private:
	static WebServicesManager * s_self;
};

#endif // __WEBSERVICES_H__
