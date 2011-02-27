/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef XMLRPCREQUEST_H
#define XMLRPCREQUEST_H

#include <QByteArray>
#include <QVariant>
#include <QVariantList>

class QXmlStreamWriter;

class XmlRpcRequest : public QObject
{
	Q_OBJECT
public:
	XmlRpcRequest();
	explicit XmlRpcRequest(const QString & methodName);
	~XmlRpcRequest();

	void setMethodName(const QString & name);
	const QString & getMethodName() const;

	const QVariantList & parameters() const;
	void setParameters(const QVariantList & list);
	void addParameters(const QVariant & variant);
	void clearParameters();

	QByteArray getRequest() const;
private:
	void createParam(QXmlStreamWriter* writer, const QVariant& param) const;

	QString _methode_name;
	QVariantList _parameters;
};

#endif // XMLRPCREQUEST_H
