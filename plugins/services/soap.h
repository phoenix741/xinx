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
 
#ifndef __SOAP_H__
#define __SOAP_H__
#pragma once

#include <QString>
#include <QDomDocument>
#include <QPair>
#include <QStringList>

class Envelop {
public: 
	Envelop( const QString & encodingStyle, const QString & namespaceString, const QString & operation );
	Envelop( const QString & element, const QString & response );
	~Envelop();
	void setParam( const QString & name, const QString & type, const QString & value );
	
	QStringList getParams();
	QPair<QString,QString> getParam( const QString & name );
	
	QString getErrorCode();
	QString getErrorString();
	
	QString toString();
	
private:
	QDomDocument m_envelop;
	QDomElement m_operation;
	QString m_errorCode, m_errorString;
};

#endif // __SOAP_H__
