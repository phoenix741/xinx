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
#pragma once

// Qt header
#include <QString>
#include <QDomElement>
#include <QAbstractItemModel>
#include <QHash>
#include <QProgressDialog>
#include <QPointer>
#include <QHttp>

// Xinx header
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
	QString m_paramString;
	QString m_paramType;
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
	QString m_name;
	QString m_encodingStyle;
	QString m_namespaceString;

	QList<Parameter*> m_inputParam;
	QList<Parameter*> m_outputParam;

	friend class WebServices;
};

class PrivateWebServices;

class WebServices : public QObject {
	Q_OBJECT
public:
	WebServices( const QString & wsdlLink, const QString & wsdlContent, QObject * parent = 0 );
	virtual ~WebServices();

	QString name();
	const QList<Operation*> & operations();

	void loadFromContent( const QString & wsdlContent );
	void loadFromElement( const QDomElement & element );

	const WSDL & wsdl() const;
signals:
	void updated();

private:
	WSDL m_wsdl;
	QString m_wsdlLink, m_wsdlContent;
	QList<Operation*> m_list;
};

typedef QList<WebServices*> WebServicesList;

class XinxProject;

class WebServicesManager : public QObject, public WebServicesList {
	Q_OBJECT
public:
	WebServicesManager( const WebServicesManager & manager );
	WebServicesManager();
	virtual ~WebServicesManager();

	void setProject( XinxProject * project );

	static WebServicesManager * self();

public slots:
	void updateWebServicesList();

signals:
	void changed();

private slots:
	void responseReady();

private:
	QPointer<QHttp> m_http;
	QPointer<QProgressDialog> m_httpDialog;
	QString m_httpString;
	bool m_hasFinished, m_isUpdate;

	static WebServicesManager * s_self;
};

#endif // __WEBSERVICES_H__
