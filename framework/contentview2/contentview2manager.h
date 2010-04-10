/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

#ifndef CONTENTVIEW2MANAGER_H
#define CONTENTVIEW2MANAGER_H
#pragma once

// Qt header
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMutex>

// Xiinx header
#include <core/xinxcore.h>
#include <core/exceptions.h>

class XinxProject;

namespace ContentView2
{

class Cache;
class Parser;

class LIBEXPORT DatabaseException : public XinxException
{
public:
	DatabaseException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, QString message);
private:
};

class LIBEXPORT Manager : public QObject
{
	Q_OBJECT
public:
	virtual ~Manager();

	void addInitializationParser(bool isGlobal, const QString & path, const QString & type, const QString & selection);
	void initializeDatabase();

	QSqlDatabase database() const;
	Cache * cache();

	QMutex & executeStatementMutex();
	QSqlQuery executeStatement(const QString & query, QStringList parameters);

	static Manager * self();
private:
	void openDatabase();
	void createDatabase(QSqlDatabase db);
	void closeDatabase();
	Manager();

	struct ParserTuple
	{
		bool isGlobal;
		QString path, type, selection;
	};

	QList<ParserTuple> m_persistentParser;
	Cache * m_cache;
	QMutex m_executeStatementMutex;
	static Manager * s_self;
};

} // namespace ContentView2

#endif // CONTENTVIEW2MANAGER_H
