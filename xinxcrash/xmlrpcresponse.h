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

#ifndef XMLRPCRESPONSE_H
#define XMLRPCRESPONSE_H

#include <QObject>
#include <QVariant>

class QXmlStreamReader;

class XmlRpcResponse : public QObject
{
	Q_OBJECT
public:
	XmlRpcResponse();
    XmlRpcResponse(const QByteArray & datas);

	void setResponse(const QByteArray & data);

	int faultCode() const;
	const QString & faultString() const;
	const QVariantList & parameters() const;
private:
	void readResponse(QXmlStreamReader * reader);
	void readFault(QXmlStreamReader * reader);
	void readParams(QXmlStreamReader * reader);
	void readParam(QXmlStreamReader * reader);
	QVariant readVariable(QXmlStreamReader * reader);
	void readListData(QXmlStreamReader* reader, QVariantList & list);
	void readListValue(QXmlStreamReader * reader, QVariantList & list);
	void readMapMembers(QXmlStreamReader* reader, QVariantMap & list);
	void readMapMember(QXmlStreamReader* reader, QVariantMap & list);
	QString readMapName(QXmlStreamReader * reader);

	int _fault_code;
	QString _fault_string;
	QVariantList _parameters;
};

#endif // XMLRPCRESPONSE_H
