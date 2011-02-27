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

#include "xmlrpcrequest.h"
#include <QXmlStreamWriter>
#include <QDateTime>

XmlRpcRequest::XmlRpcRequest()
{

}

XmlRpcRequest::XmlRpcRequest(const QString& methodName): _methode_name(methodName)
{

}

XmlRpcRequest::~XmlRpcRequest()
{

}

void XmlRpcRequest::setMethodName(const QString& name)
{
	_methode_name = name;
}

const QString& XmlRpcRequest::getMethodName() const
{
	return _methode_name;
}

void XmlRpcRequest::setParameters(const QVariantList& list)
{
	_parameters = list;
}

const QVariantList& XmlRpcRequest::parameters() const
{
	return _parameters;
}

void XmlRpcRequest::addParameters(const QVariant& variant)
{
	_parameters.append(variant);
}

void XmlRpcRequest::clearParameters()
{
	_parameters.clear();
}

void XmlRpcRequest::createParam(QXmlStreamWriter* writer, const QVariant & param) const
{
	switch(param.type())
	{
	case QVariant::Int:
		writer->writeTextElement("int", QString::number(param.toInt()));
		break;
	case QVariant::Bool:
		writer->writeTextElement("boolean", QString::number((int)param.toBool()));
		break;
	case QVariant::String:
		writer->writeTextElement("string", param.toString());
		break;
	case QVariant::Double:
		writer->writeTextElement("double", QString::number(param.toDouble()));
		break;
	case QVariant::DateTime:
		writer->writeTextElement("dateTime.iso8601", param.toDateTime().toString("YYYYMMDDThhmmZ"));
		break;
	case QVariant::ByteArray:
		writer->writeTextElement("base64", param.toByteArray().toBase64());
		break;
	case QVariant::List:
		{
			writer->writeStartElement("array");
			writer->writeStartElement("data");
			QVariantList list = param.toList();
			foreach(const QVariant & var, list)
			{
				writer->writeStartElement("value");
				createParam(writer, var);
				writer->writeEndElement();
			}
			writer->writeEndElement();
			writer->writeEndElement();
		}
		break;
	case QVariant::Map:
		{
			writer->writeStartElement("struct");
			QVariantMap map = param.toMap();
			QMapIterator<QString,QVariant> it(map);
			while(it.hasNext())
			{
				it.next();
				writer->writeStartElement("member");
				writer->writeTextElement("name", it.key());
				writer->writeStartElement("value");
				createParam(writer, it.value());
				writer->writeEndElement();
				writer->writeEndElement();
			}
			writer->writeEndElement();
		}
		break;
	case QVariant::Invalid:
		writer->writeStartElement("nil");
		writer->writeEndElement();
		break;
	default:
		break;
	}
}

QByteArray XmlRpcRequest::getRequest() const
{
	QByteArray result;
	QXmlStreamWriter writer(&result);

	writer.writeStartDocument();
	writer.writeStartElement("methodCall");
	writer.writeTextElement("methodName", _methode_name);
	writer.writeStartElement("params");
	foreach(const QVariant & param, _parameters)
	{
		writer.writeStartElement("param");
		createParam(&writer, param);
		writer.writeEndElement();
	}
	writer.writeEndElement();
	writer.writeEndElement();
	writer.writeEndDocument();

	return result;
}
