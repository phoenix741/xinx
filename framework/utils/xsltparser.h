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

#pragma once
#ifndef XSLTPARSER_H
#define XSLTPARSER_H

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QCoreApplication>
#include <QByteArray>
#include <QString>
#include <QSharedPointer>

namespace XinxProject
{
	class Project;
	typedef QSharedPointer<Project> ProjectPtr;
	typedef QWeakPointer<Project> ProjectPtrWeak;
}

class PrivateXsltParser;

class LIBEXPORT XsltParser
{
	Q_DECLARE_TR_FUNCTIONS(XsltParser)
public:
	class ErrorMessage
	{
	public:
		bool isWarning;
		QString message;
		int line;
	};

	XsltParser();
	virtual ~XsltParser();

	void setProject(XinxProject::ProjectPtr project);
	XinxProject::ProjectPtr project();

	bool loadStylesheet(const QString & filename);

	bool loadXmlFile(const QByteArray & data);
	bool loadXmlFile(const QString & filename);

	QString getOutput() const;

	bool process();

	const QList<XsltParser::ErrorMessage> & errors() const;
protected:
	virtual void registerPlugin(void* ctxt);
private:
	QScopedPointer<PrivateXsltParser> d;
	friend class PrivateXsltParser;
};

#endif // XSLTPARSER_H
