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

class WebServices;
class QHttp;
class QIODevice;
class XSLProject;

class Operation {
public:
	Operation( QString name, QString soapAction ) : m_name( name ), m_action( soapAction ) {};

	const QString & name() const { return m_name; };
	const QString & soapAction() const { return m_action; };
	
	const QStringList & inputParam() { return m_inputParam; };
	const QStringList & outputParam() { return m_outputParam; };
private:
	QString m_name;
	QString m_action;
	
	QStringList m_inputParam;
	QStringList m_outputParam;
	
friend class WebServices;
};

class WebServices : public QObject {
	Q_OBJECT
public:
	WebServices( const QString & link, QObject * parent = 0 );

	const QString & name() const { return m_name; };
	const QList<Operation> & operations() const { return m_list; };
	
	void askWSDL( const QString & link );
	void loadFromElement( const QDomElement & element );
signals:
	void updated();	
private slots:
	void httpRequestFinished ( int id, bool error );
private:
	QString m_name;
	QList<Operation> m_list;

	int m_requestId;
	QHttp * m_http;
	QIODevice * m_response;
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
