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

#pragma once
#ifndef CONTENTVIEWNODE_H
#define CONTENTVIEWNODE_H

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>
#include <contentview2/contentview2manager.h>

// Qt header
#include <QList>
#include <QHash>
#include <QSqlDatabase>
#include <QApplication>
#include <QExplicitlySharedDataPointer>

namespace ContentView2
{

class LIBEXPORT NodeException : public DatabaseException
{
public:
	NodeException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, QString message);
private:
};

class PrivateNode;
class File;

class LIBEXPORT Node
{
	Q_DECLARE_TR_FUNCTIONS(Node)
public:
	enum RoleIndex
	{
		NODE_ID            = 32,
		NODE_NAME          = 2,
		NODE_TYPE          = 33,
		NODE_ICON          = 1,
		NODE_DISPLAY_NAME  = 0,
		NODE_DISPLAY_TIPS  = 3,
		NODE_COMPLETE_FORM = 34,
		NODE_USER_VALUE    = 35,
	};

	Node();
	Node(const Node & other);
	Node(QSqlDatabase db, uint id);
	~Node();

	void load(QSqlDatabase db, uint id);
	uint create(QSqlDatabase db, int forcedId = -1);
	void update(QSqlDatabase db);
	void reload(QSqlDatabase db);

	void destroy(QSqlDatabase db);
	void destroyChilds(QSqlDatabase db);
	void destroyChildsAsRoot(QSqlDatabase db);

	uint nodeId() const;

	bool attach(uint parentId);
	void detach();

	int line() const;
	void setLine(int value);

	QString filename(QSqlDatabase db) const;
	int fileId() const;
	void setFileId(int value);
	void setFile(const File & file);
	File file(QSqlDatabase db);

	QVariant data(int index = Node::NODE_NAME) const;
	void setData(const QVariant & value, int index = Node::NODE_NAME);

	QList<int> childs(QSqlDatabase db) const;
	int parent(QSqlDatabase db) const;

	int indexOfChild(QSqlDatabase db, const Node & node) const;

	bool isValid() const;
	void clear();

	Node & operator=(const Node & node);
private:
	QExplicitlySharedDataPointer<PrivateNode> d;
	friend uint qHash(const Node & node);
};

extern uint qHash(const Node & node);

} // namespace ContentView2

#endif // CONTENTVIEWNODE_H
