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

#include "xmlrpcresponse.h"
#include <QXmlStreamReader>
#include <QDateTime>

XmlRpcResponse::XmlRpcResponse()
{

}

XmlRpcResponse::XmlRpcResponse(const QByteArray& datas)
{
	setResponse(datas);
}


void XmlRpcResponse::setResponse(const QByteArray& data)
{
	_fault_code   = -1;
	_fault_string = QString();

	QXmlStreamReader reader(data);

	while(!reader.atEnd())
	{
		reader.readNext();

		if (reader.isEndElement())
			break;

		if (reader.isStartElement())
		{
			const QString name = reader.name().toString();
			if (name == "methodResponse")
			{
				readResponse(&reader);
			}
		}
	}
}

void XmlRpcResponse::readResponse(QXmlStreamReader* reader)
{
	while(! reader->atEnd())
	{
		reader->readNext();

		if (reader->isEndElement())
			break;

		if (reader->isStartElement())
		{
			const QString name = reader->name().toString();
			if (name == "fault")
			{
				readFault(reader);
			}
			else if (name == "params")
			{
				readParams(reader);
			}
		}
	}
}

void XmlRpcResponse::readFault(QXmlStreamReader* reader)
{
	while(! reader->atEnd())
	{
		reader->readNext();

		if (reader->isEndElement())
			break;

		if (reader->isStartElement())
		{
			const QString name = reader->name().toString();
			if (name == "value")
			{
				QVariant var = readVariable(reader);
				if (var.type() == QVariant::Map)
				{
					QVariantMap map = var.toMap();
					_fault_code   = map.value("faultCode").toInt();
					_fault_string = map.value("faultString").toString();
				}
			}
		}
	}
}

void XmlRpcResponse::readParams(QXmlStreamReader* reader)
{
	while(! reader->atEnd())
	{
		reader->readNext();

		if (reader->isEndElement())
			break;

		if (reader->isStartElement())
		{
			const QString name = reader->name().toString();
			if (name == "param")
			{
				readParam(reader);
			}
		}
	}

}

void XmlRpcResponse::readParam(QXmlStreamReader* reader)
{
	while(! reader->atEnd())
	{
		reader->readNext();

		if (reader->isEndElement())
			break;

		if (reader->isStartElement())
		{
			const QString name = reader->name().toString();
			if (name == "value")
			{
				QVariant var = readVariable(reader);
				_parameters.append(var);
			}
		}
	}
}

QVariant XmlRpcResponse::readVariable(QXmlStreamReader* reader)
{
	QVariant value;

	while(! reader->atEnd())
	{
		reader->readNext();

		if (reader->isEndElement())
			break;

		if (reader->isStartElement())
		{
			const QString name = reader->name().toString();
			if (name == "int")
			{
				QString text = reader->readElementText();
				value = QVariant(text.toInt());
			}
			else if (name == "boolean")
			{
				QString text = reader->readElementText();
				value = QVariant((bool)text.toInt());
			}
			else if (name == "string")
			{
				QString text = reader->readElementText();
				value = QVariant(text);
			}
			else if (name == "double")
			{
				QString text = reader->readElementText();
				value = QVariant(text.toDouble());
			}
			else if (name == "dateTime.iso8601")
			{
				QString text = reader->readElementText();
				value = QVariant(QDateTime::fromString(text, "YYYYMMDDThhmmZ"));
			}
			else if (name == "base64")
			{
				QString text = reader->readElementText();
				value = QVariant(QByteArray::fromBase64(text.toAscii()));
			}
			else if (name == "array")
			{
				QVariantList list;
				readListData(reader, list);
				value = QVariant(list);
			}
			else if (name == "struct")
			{
				QVariantMap list;
				readMapMembers(reader, list);
				value = QVariant(list);
			}
			else if (name == "nil")
			{
				value = QVariant();
			}
		}
	}

	return value;
}

void XmlRpcResponse::readListData(QXmlStreamReader* reader, QVariantList& list)
{
	while(! reader->atEnd())
	{
		reader->readNext();

		if (reader->isEndElement())
			break;

		if (reader->isStartElement())
		{
			const QString name = reader->name().toString();
			if (name == "data")
			{
				readListValue(reader, list);
			}
		}
	}
}

void XmlRpcResponse::readListValue(QXmlStreamReader* reader, QVariantList& list)
{
	while(! reader->atEnd())
	{
		reader->readNext();

		if (reader->isEndElement())
			break;

		if (reader->isStartElement())
		{
			const QString name = reader->name().toString();
			if (name == "value")
			{
				QVariant var = readVariable(reader);
				list.append(var);
			}
		}
	}
}

void XmlRpcResponse::readMapMembers(QXmlStreamReader* reader, QVariantMap& list)
{
	while(! reader->atEnd())
	{
		reader->readNext();

		if (reader->isEndElement())
			break;

		if (reader->isStartElement())
		{
			const QString name = reader->name().toString();
			if (name == "member")
			{
				readMapMember(reader, list);
			}
		}
	}
}

void XmlRpcResponse::readMapMember(QXmlStreamReader* reader, QVariantMap& list)
{
	QString member_name;
	QVariant member_value;

	while(! reader->atEnd())
	{
		reader->readNext();

		if (reader->isEndElement())
			break;

		if (reader->isStartElement())
		{
			const QString name = reader->name().toString();
			if (name == "name")
			{
				member_name = readMapName(reader);
			}
			else if (name == "value")
			{
				member_value = readVariable(reader);
			}
		}
	}

	list.insert(member_name, member_value);
}

QString XmlRpcResponse::readMapName(QXmlStreamReader* reader)
{
	QString member_name;
	while(! reader->atEnd())
	{
		reader->readNext();

		if (reader->isEndElement())
			break;

		if (reader->isStartElement())
		{
			const QString name = reader->name().toString();
			if (name == "name")
			{
				member_name = reader->readElementText();
			}
		}
	}
	return member_name;
}

const QVariantList& XmlRpcResponse::parameters() const
{
	return _parameters;
}

int XmlRpcResponse::faultCode() const
{
	return _fault_code;
}

const QString& XmlRpcResponse::faultString() const
{
	return _fault_string;
}
