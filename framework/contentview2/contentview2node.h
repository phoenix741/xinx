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

/*!
 * \file contentviewnode.h
 * \brief Contains the element for manage a node used in content view.
 */

#ifndef CONTENTVIEWNODE_H
#define CONTENTVIEWNODE_H
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>

// Qt header
#include <QList>
#include <QHash>
#include <QSqlDatabase>
#include <QApplication>
#include <QExplicitlySharedDataPointer>

namespace ContentView2
{

/*!
 * \class NodeException
 * \brief Exception throw when a SQL error occur
 */
class LIBEXPORT NodeException : public XinxException
{
public:
	/*!
	 * Create the exception with a message and a line.
	 * \param message Error of the exception.
	 * \param line Line where the error is.
	 */
	NodeException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, QString message, QString nodeName);

	/*!
	 * Return the line where the error is.
	 * \return The line of the error.
	 */
	const QString & getNodeName() const;
private:
	QString m_nodeName;
};

class PrivateNode;
class File;

/*!
 * \class Node
 * \brief This class represent a node element for the content view
 *
 * This class is a node element for the content view. It can be added to another
 * content view element, and detached.
 *
 * This class is based on SQL
 */
class LIBEXPORT Node
{
	Q_DECLARE_TR_FUNCTIONS(Node)
public:
	/*!
	 * To simplify, the content view node will be not subclassed. Else data will
	 * be stored in this structure as role. Other value (as mode, ...) can be stored
	 * as user value, if needed.
	 */
	enum RoleIndex
	{
		NODE_ID            = 32,  //!< The id of the node (a number)
		NODE_NAME          = 2,   //!< The waiting content is a QString.
		NODE_TYPE          = 33,  //!< The waiting content is a QString.
		NODE_ICON          = 1,   //!< The waiting content is a QString.
		NODE_DISPLAY_NAME  = 0,   //!< The waiting content is a QString.
		NODE_DISPLAY_TIPS  = 3,   //!< The waiting content is a QString.
		NODE_COMPLETE_FORM = 34,  //!< The waiting content is a QString.
		NODE_USER_VALUE    = 35,  //!< No waiting content.
	};

	/*!
	 * Create an empty content view node
	 */
	Node();
	//! Copy the node
	Node(const Node & other);
	/*!
	 * Create an empty content view node and load the id
	 */
	Node(QSqlDatabase db, uint id);
	/*!
	 * Destroy the instance of ContentViewNode
	 */
	~Node();

	//! Load the object from the base
	void load(QSqlDatabase db, uint id);
	//! Create the node with the content of the object
	uint create(QSqlDatabase db, int forcedId = -1);
	//! Update the object in the base
	void update(QSqlDatabase db);
	//! Reload the object
	void reload(QSqlDatabase db);

	/*!
	 * Delete the node. After deleted the node, the content of the object is
	 * obsolete.
	 * \see removeAll()
	 */
	void destroy(QSqlDatabase db);
	//! Remove all childs of the node.
	void destroyChilds(QSqlDatabase db);

	//! Return the node id
	uint nodeId() const;

	/*!
	 * Attach this node and all it's child to the parent node below.
	 * \param parent The parent node id where this node is attached.
	 * \see detach()
	 */
	bool attach(QSqlDatabase db, uint parentId);

	/*!
	 * Detach the node from the given parent id
	 * \see attach()
	 */
	void detach(QSqlDatabase db, uint parentId);

	//! Return the register line for the node
	int line() const;
	//! Change the line of the node
	void setLine(int value);

	QString filename(QSqlDatabase db) const;
	//! Return the current file name of the node. If not set, this is the current file name.
	int fileId() const;
	//! Set the file name of the node with \e value.
	void setFileId(int value);
	//! Set the file to use with the node.
	void setFile(const File & file);
	//! Return the file the use the node
	File file(QSqlDatabase db);

	//! Return the data stored in the node for the given \e index
	QVariant data(int index = Node::NODE_NAME) const;
	//! Set the data for the givent \e index
	void setData(const QVariant & value, int index = Node::NODE_NAME);

	/*!
	 * List of id of the childs node of this node.
	 * \see attach(), detach()
	 */
	QList<int> childs(QSqlDatabase db) const;
	/*!
	 * List of id of the childs node of this node.
	 * \see attach(), detach()
	 */
	QList<int> parents(QSqlDatabase db) const;

	//! Make a hash of the node and search the node having the same hash.
	int indexOfChild(QSqlDatabase db, const Node & node) const;

	//! Return true is the node has a valid content.
	bool isValid() const;
	//! Clear the data of the node and make it not valid
	void clear();

	//! Copy the node
	Node & operator=(const Node & node);
private:
	QExplicitlySharedDataPointer<PrivateNode> d;
	friend uint qHash(const Node & node);
};

extern uint qHash(const Node & node);

} // namespace ContentView2

#endif // CONTENTVIEWNODE_H
