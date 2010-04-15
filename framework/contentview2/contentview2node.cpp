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

// Xinx header
#include "contentview2node.h"
#include "contentview2file.h"
#include "contentview2manager.h"

// Qt header
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSqlRecord>
#include <QHash>

namespace ContentView2
{

/* NodeException */
/*!
 * \ingroup ContentView2
 * \class NodeException
 * \since 0.9.0.0
 *
 * \brief Exception throw when a SQL error occur
 */

/*!
 * \brief Create the exception with a message and a line.
 *
 * \param assertion The condition who failed
 * \param locationFile The file wich failed (this file)
 * \param locationLine The line where the exception is throw
 * \param locationMethod The method where the exception is throw.
 * \param message Error of the exception.
 */
NodeException::NodeException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, QString message)
		: DatabaseException(assertion, locationFile, locationLine, locationMethod, message)
{
}

/* PrivateNode */

class PrivateNode : public QSharedData
{
public:
	int m_line;
	int m_fileId;
	int m_parentId;
	QHash<int,QVariant> m_datas;

	uint m_hash;
	long m_id;
	bool m_isLoaded;
	QSqlDatabase m_db;

	void load();

	PrivateNode();
	PrivateNode(const PrivateNode &other);
	~PrivateNode();
};

PrivateNode::PrivateNode() : m_line(-1), m_fileId(-1), m_parentId(-1), m_id(-1), m_isLoaded(false)
{
}

PrivateNode::PrivateNode(const PrivateNode &other) : QSharedData(other), m_line(other.m_line), m_fileId(other.m_fileId), m_datas(other.m_datas), m_id(other.m_id), m_isLoaded(other.m_isLoaded), m_db(other.m_db)
{
}

PrivateNode::~PrivateNode()
{

}

void PrivateNode::load()
{
	if ((m_id == -1) || (m_isLoaded)) return;

	QSqlQuery selectQuery = Manager::self()->getSqlQuery("SELECT parent_id, name, type, icon, display_name, tips, completion_value, line, file_id, hash, "
	                      "property1, property2, property3, property4, property5, property6, "
	                      "property7, property8, property9, property10 "
	                      "FROM cv_node WHERE id=:id", m_db);
	selectQuery.bindValue(":id", QVariant::fromValue(m_id));
	bool result = selectQuery.exec();
	EXCEPT_ELSE(result, NodeException, "PrivateNode::load", qPrintable(selectQuery.lastError().text()));
	EXCEPT_ELSE(selectQuery.first(), NodeException, "PrivateNode::load", Node::tr("Can't find the node %1").arg(m_id));

	m_datas.clear();

	m_parentId = selectQuery.value(0).toInt();
	m_datas.insert(Node::NODE_NAME, selectQuery.value(1).toString());
	m_datas.insert(Node::NODE_TYPE, selectQuery.value(2).toString());
	m_datas.insert(Node::NODE_ICON, selectQuery.value(3).toString());
	m_datas.insert(Node::NODE_DISPLAY_NAME, selectQuery.value(4).toString());
	m_datas.insert(Node::NODE_DISPLAY_TIPS, selectQuery.value(5).toString());
	m_datas.insert(Node::NODE_COMPLETE_FORM, selectQuery.value(6).toString());
	m_line   = selectQuery.value(7).toInt();
	m_fileId = selectQuery.value(8).toInt();
	m_hash   = selectQuery.value(9).toUInt();

	for (int i = 0; i < 10 ; i++)
	{
		QVariant value = selectQuery.value(10 + i);
		if (value.isValid())
		{
			m_datas.insert(Node::NODE_USER_VALUE + i, value.toString());
		}
	}

	QSqlQuery selectPropertiesQuery = Manager::self()->getSqlQuery("SELECT ord, value FROM cv_node_property WHERE node_id=:node_id", m_db);
	selectPropertiesQuery.bindValue(":node_id", QVariant::fromValue(m_id));
	result = selectPropertiesQuery.exec();
	EXCEPT_ELSE(result, NodeException, "PrivateNode::load", qPrintable(selectPropertiesQuery.lastError().text()));

	while (selectPropertiesQuery.next())
	{
		m_datas.insert(selectPropertiesQuery.value(0).toInt(), selectPropertiesQuery.value(1).toString());
	}

	selectQuery.finish();
	selectPropertiesQuery.finish();

	m_isLoaded = true;
	m_db       = QSqlDatabase();
}

/* Node */

/*!
 * \ingroup ContentView2
 * \class Node
 * \since 0.9.0.0
 *
 * \brief This class represent a node element for the content view
 *
 * This class is a node element for the content view. It can be added to another
 * content view element, and detached.
 *
 * This class is based on SQL on the table cv_node.
 *
 * This class limit the number of load. The method load is really launch when a set or
 * get method is made.
 */

/*!
 * \enum Node::RoleIndex
 * \brief List of role that can be used with method data() and setData()
 *
 * To simplify, the content view node will be not subclassed. Else data will
 * be stored in this structure as role. Other value (as mode, ...) can be stored
 * as user value, if needed.
 */
/*!
 * \var Node::RoleIndex Node::NODE_ID
 * \brief The id of the node (a number)
 */
/*!
 * \var Node::RoleIndex Node::NODE_NAME
 * \brief The waiting content is a QString.
 */
/*!
 * \var Node::RoleIndex Node::NODE_TYPE
 * \brief The waiting content is a QString.
 */
/*!
 * \var Node::RoleIndex Node::NODE_ICON
 * \brief The waiting content is a QString.
 */
/*!
 * \var Node::RoleIndex Node::NODE_DISPLAY_NAME
 * \brief The waiting content is a QString.
 */
/*!
 * \var Node::RoleIndex Node::NODE_DISPLAY_TIPS
 * \brief The waiting content is a QString.
 */
/*!
 * \var Node::RoleIndex Node::NODE_COMPLETE_FORM
 * \brief The waiting content is a QString.
 */
/*!
 * \var Node::RoleIndex Node::NODE_USER_VALUE
 * \brief No waiting content.
 */

/*!
 * \brief Create an empty content view node
 */
Node::Node()
{
	d = new PrivateNode;
}

//! Copy the node
Node::Node(const Node & other) : d(other.d)
{
}

/*!
 * \brief Create an empty content view node and load the id
 */
Node::Node(QSqlDatabase db, uint id)
{
	d = new PrivateNode;
	load(db, id);
}

/*!
 * \brief Destroy the instance of ContentViewNode
 */
Node::~Node()
{
}

//! Load the object from the base
void Node::load(QSqlDatabase db, uint id)
{
	d->m_isLoaded = false;
	d->m_id = id;
	d->m_db = db;
}

//! Reload the object
void Node::reload(QSqlDatabase db)
{
	if (d->m_id >= 0)
		load(db, d->m_id);
}

//! Create the node with the content of the object
uint Node::create(QSqlDatabase db, int forcedId)
{
	QSqlQuery insertQuery;
	if (forcedId == -1)
	{
		insertQuery = Manager::self()->getSqlQuery(
							"INSERT INTO cv_node(parent_id, name, type, icon, display_name, tips, completion_value, line, file_id, hash, property1, property2, property3, property4, property5, property6, property7, property8, property9, property10) "
							"VALUES(:parent_id, :name, :type, :icon, :display_name, :tips, :completion_value, :line, :file_id, :hash, :property1, :property2, :property3, :property4, :property5, :property6, :property7, :property8, :property9, :property10)", db);
	}
	else
	{
		insertQuery = Manager::self()->getSqlQuery(
							"INSERT INTO cv_node(id, parent_id, name, type, icon, display_name, tips, completion_value, line, file_id, hash, property1, property2, property3, property4, property5, property6, property7, property8, property9, property10) "
							"VALUES(:forced_id, :parent_id, :name, :type, :icon, :display_name, :tips, :completion_value, :line, :file_id, :hash, :property1, :property2, :property3, :property4, :property5, :property6, :property7, :property8, :property9, :property10)", db);
		insertQuery.bindValue(":forced_id", forcedId);
	}

	insertQuery.bindValue(":parent_id", d->m_parentId);
	insertQuery.bindValue(":name", d->m_datas.value(Node::NODE_NAME));
	insertQuery.bindValue(":type", d->m_datas.value(Node::NODE_TYPE));
	insertQuery.bindValue(":icon", d->m_datas.value(Node::NODE_ICON));
	insertQuery.bindValue(":display_name", d->m_datas.value(Node::NODE_DISPLAY_NAME));
	insertQuery.bindValue(":tips", d->m_datas.value(Node::NODE_DISPLAY_TIPS));
	insertQuery.bindValue(":completion_value", d->m_datas.value(Node::NODE_COMPLETE_FORM).toString());
	insertQuery.bindValue(":line", d->m_line);
	insertQuery.bindValue(":file_id", d->m_fileId);
	insertQuery.bindValue(":hash", qHash(*this));
	insertQuery.bindValue(":property1", d->m_datas.value(Node::NODE_USER_VALUE).toString());
	insertQuery.bindValue(":property2", d->m_datas.value(Node::NODE_USER_VALUE + 1).toString());
	insertQuery.bindValue(":property3", d->m_datas.value(Node::NODE_USER_VALUE + 2).toString());
	insertQuery.bindValue(":property4", d->m_datas.value(Node::NODE_USER_VALUE + 3).toString());
	insertQuery.bindValue(":property5", d->m_datas.value(Node::NODE_USER_VALUE + 4).toString());
	insertQuery.bindValue(":property6", d->m_datas.value(Node::NODE_USER_VALUE + 5).toString());
	insertQuery.bindValue(":property7", d->m_datas.value(Node::NODE_USER_VALUE + 6).toString());
	insertQuery.bindValue(":property8", d->m_datas.value(Node::NODE_USER_VALUE + 7).toString());
	insertQuery.bindValue(":property9", d->m_datas.value(Node::NODE_USER_VALUE + 8).toString());
	insertQuery.bindValue(":property10", d->m_datas.value(Node::NODE_USER_VALUE + 9).toString());

	bool result = insertQuery.exec();
	EXCEPT_ELSE(result, NodeException, "Node::create", qPrintable(insertQuery.lastError().text()));

	uint newId = insertQuery.lastInsertId().toInt();

	QSqlQuery insertPropertyQuery;

	foreach(const int ord, d->m_datas.keys())
	{
		if (ord < Node::NODE_USER_VALUE + 10) continue;
		if(insertPropertyQuery.lastQuery().isEmpty())
		{
			insertPropertyQuery = Manager::self()->getSqlQuery(
				"INSERT INTO cv_node_property(node_id, ord, value) "
				"VALUES(:node_id, :ord, :value)", db);
		}

		insertPropertyQuery.bindValue(":node_id", newId);
		insertPropertyQuery.bindValue(":ord", ord);
		insertPropertyQuery.bindValue(":value", d->m_datas.value(ord));

		result = insertPropertyQuery.exec();
		EXCEPT_ELSE(result, NodeException, "Node::create", qPrintable(insertPropertyQuery.lastError().text()));
	}

	d->m_id       = newId;
	d->m_isLoaded = true;
	return newId;
}

//! Update the object in the base
void Node::update(QSqlDatabase db)
{
	Q_ASSERT_X(d->m_id >= 0, "Node::update", "The node must be initialized");
	d->load();

	uint newHash = qHash(*this);
	if (d->m_hash == newHash) return;
	d->m_hash = newHash;

	QSqlQuery updateQuery = Manager::self()->getSqlQuery(
							"UPDATE cv_node "
							"SET	parent_id=:parent_id , "
							"		name=:name , "
							"		type=:type , "
							"		icon=:icon , "
							"		display_name=:display_name , "
							"		tips=:tips , "
							"		completion_value=:completion_value , "
							"		line=:line , "
							"		hash=:hash , "
							"		property1=:property1, "
							"		property2=:property2, "
							"		property3=:property3, "
							"		property4=:property4, "
							"		property5=:property5, "
							"		property6=:property6, "
							"		property7=:property7, "
							"		property8=:property8, "
							"		property9=:property9, "
							"		property10=:property10 "
							"WHERE id=:id", db);
	updateQuery.bindValue(":parent_id", d->m_parentId);
	updateQuery.bindValue(":name", d->m_datas.value(Node::NODE_NAME));
	updateQuery.bindValue(":type", d->m_datas.value(Node::NODE_TYPE));
	updateQuery.bindValue(":icon", d->m_datas.value(Node::NODE_ICON));
	updateQuery.bindValue(":display_name", d->m_datas.value(Node::NODE_DISPLAY_NAME));
	updateQuery.bindValue(":tips", d->m_datas.value(Node::NODE_DISPLAY_TIPS));
	updateQuery.bindValue(":completion_value", d->m_datas.value(Node::NODE_COMPLETE_FORM));
	updateQuery.bindValue(":line", d->m_line);
	updateQuery.bindValue(":hash", newHash);
	updateQuery.bindValue(":id", QVariant::fromValue(d->m_id));
	updateQuery.bindValue(":property1", d->m_datas.value(Node::NODE_USER_VALUE).toString());
	updateQuery.bindValue(":property2", d->m_datas.value(Node::NODE_USER_VALUE + 1).toString());
	updateQuery.bindValue(":property3", d->m_datas.value(Node::NODE_USER_VALUE + 2).toString());
	updateQuery.bindValue(":property4", d->m_datas.value(Node::NODE_USER_VALUE + 3).toString());
	updateQuery.bindValue(":property5", d->m_datas.value(Node::NODE_USER_VALUE + 4).toString());
	updateQuery.bindValue(":property6", d->m_datas.value(Node::NODE_USER_VALUE + 5).toString());
	updateQuery.bindValue(":property7", d->m_datas.value(Node::NODE_USER_VALUE + 6).toString());
	updateQuery.bindValue(":property8", d->m_datas.value(Node::NODE_USER_VALUE + 7).toString());
	updateQuery.bindValue(":property9", d->m_datas.value(Node::NODE_USER_VALUE + 8).toString());
	updateQuery.bindValue(":property10", d->m_datas.value(Node::NODE_USER_VALUE + 9).toString());

	bool result = updateQuery.exec();
	EXCEPT_ELSE(result, NodeException, "Node::update", qPrintable(updateQuery.lastError().text()));

	QSqlQuery deleteQuery = Manager::self()->getSqlQuery("DELETE FROM cv_node_property WHERE node_id=:id", db);
	deleteQuery.bindValue(":id", QVariant::fromValue(d->m_id));
	result = deleteQuery.exec();
	EXCEPT_ELSE(result, NodeException, "Node::update", qPrintable(deleteQuery.lastError().text()));

	QSqlQuery insertPropertyQuery = Manager::self()->getSqlQuery("INSERT INTO cv_node_property(node_id, ord, value) "
	                              "VALUES(:node_id, :ord, :value)", db);

	insertPropertyQuery.bindValue(":node_id", QVariant::fromValue(d->m_id));
	foreach(const int ord, d->m_datas.keys())
	{
		if (ord < Node::NODE_USER_VALUE + 10) continue;

		insertPropertyQuery.bindValue(":ord", ord);
		insertPropertyQuery.bindValue(":value", d->m_datas.value(ord));

		result = insertPropertyQuery.exec();
		EXCEPT_ELSE(result, NodeException, "Node::update", qPrintable(insertPropertyQuery.lastError().text()));
	}

}

/*!
 * \brief Delete the node.
 *
 * After deleted the node, the content of the object is obsolete.
 * \see destroyChilds()
 */
void Node::destroy(QSqlDatabase db)
{
	Q_ASSERT_X(d->m_id >= 0, "Node::destroy", "The node must be initialized");
	destroyChilds(db);

	QSqlQuery deleteQuery1 = Manager::self()->getSqlQuery("DELETE FROM cv_node WHERE id=:id", db);
	deleteQuery1.bindValue(":id", QVariant::fromValue(d->m_id));
	bool result = deleteQuery1.exec();
	EXCEPT_ELSE(result, NodeException, "Node::destroy", qPrintable(deleteQuery1.lastError().text()));

	QSqlQuery deleteQuery2 = Manager::self()->getSqlQuery("DELETE FROM cv_node_property WHERE node_id=:id", db);
	deleteQuery2.bindValue(":id", QVariant::fromValue(d->m_id));
	result = deleteQuery2.exec();
	EXCEPT_ELSE(result, NodeException, "Node::destroy", qPrintable(deleteQuery2.lastError().text()));

	d->m_id = -1;
}

//! Return the node id
uint Node::nodeId() const
{
	return d->m_id;
}

/*!
 * \brief Attach this node and all it's child to the parent node below. A node can have only one parent at a time.
 * \param parentId The parent node id where this node is attached.
 * \see detach()
 */
bool Node::attach(uint parentId)
{
	Q_ASSERT_X(d->m_parentId == -1, "Node::attach", "Node must be detached first");

	d->load();
	d->m_parentId = parentId;
	return true;
}

/*!
 * \brief Detach the node from the given parent id
 * \see attach()
 */
void Node::detach()
{
	Q_ASSERT_X(d->m_parentId, "Node::detach", "The node must be attached first");
	d->load();
	d->m_parentId = -1;
}

//! Remove all childs of the node.
void Node::destroyChilds(QSqlDatabase db)
{
	if (d->m_id == -1) return ;

	foreach(int id, childs(db))
	{
		Node child(db, id);
		child.destroy(db);
	}
}

/*!
 * \brief Remove all childs of the him (considered as root node)
 *
 * This method is provide for convinience and optimisation and delete
 * all node from the current files but not himself (the root node)
 */
void Node::destroyChildsAsRoot(QSqlDatabase db)
{
	Q_ASSERT_X(d->m_id >= 0, "Node::destroyChildsAsRoot", "The node must be initialized");

	d->load();
	Q_ASSERT_X(d->m_fileId >= 0, "Node::destroyChildsAsRoot", "The node must be attached to a file");

	QSqlQuery deleteQuery = Manager::self()->getSqlQuery("DELETE FROM cv_node WHERE id <> :id AND file_id=:file_id", db);
	deleteQuery.bindValue(":id", QVariant::fromValue(d->m_id));
	deleteQuery.bindValue(":file_id", QVariant::fromValue(d->m_fileId));
	bool result = deleteQuery.exec();
	EXCEPT_ELSE(result, NodeException, "Node::destroyChildsAsRoot", qPrintable(deleteQuery.lastError().text()));
}

//! Return the register line for the node
int Node::line() const
{
	d->load();
	return d->m_line;
}

//! Change the line of the node
void Node::setLine(int value)
{
	d->load();
	d->m_line = value;
}

//! Return the filename of the File of the current node
QString Node::filename(QSqlDatabase db) const
{
	QSqlQuery selectQuery = Manager::self()->getSqlQuery("SELECT path FROM cv_file WHERE id=:file_id", db);
	selectQuery.bindValue(":file_id", d->m_fileId);

	bool result = selectQuery.exec();
	EXCEPT_ELSE(result, NodeException, "Node::filename", qPrintable(selectQuery.lastError().text()));

	if (selectQuery.first())
	{
		const QString path = selectQuery.value(0).toString();
		selectQuery.finish();
		return path;
	}
	else
	{
		selectQuery.finish();
		return QString();
	}
}

//! Return the file id of the File of the current node.
int Node::fileId() const
{
	d->load();
	return d->m_fileId;
}

/*!
 * \brief Set the file ID of the File of the node with \e value.
 *
 * The file ID can't be modified after the file creation or if the file is loaded.
 */
void Node::setFileId(int value)
{
	Q_ASSERT_X(d->m_id == -1, "Node::setFileId", "The filename can't be modified after node creation");
	d->m_fileId = value;
}

/*!
 * \brief Set the file to use with the node.
 *
 * The File can't be modified after the file creation or if the file is loaded.
 */
void Node::setFile(const File & file)
{
	Q_ASSERT_X(d->m_id == -1, "Node::setFile", "The filename can't be modified after node creation");
	d->m_fileId = file.fileId();
}

//! Return the file the use the node
File Node::file(QSqlDatabase db)
{
	d->load();
	if (d->m_fileId >= 0)
	{
		return File(db, d->m_fileId);
	}
	else
	{
		return File();
	}
}

//! Return the data stored in the node for the given \e index
QVariant Node::data(int index) const
{
	d->load();
	return d->m_datas.value(index);
}

//! Set the data for the givent \e index
void Node::setData(const QVariant & value, int index)
{
	d->load();
	if (d->m_datas.value(index) != value)
	{
		d->m_datas.insert(index, value);
	}
}

/*!
 * \brief List of id of the child's node of this node.
 * \see attach(), detach()
 */
QList<int> Node::childs(QSqlDatabase db) const
{
	QList<int> result;
	if (d->m_id == -1) return result;

	QSqlQuery select = Manager::self()->getSqlQuery("SELECT id FROM cv_node WHERE parent_id=:parent_id", db);
	select.bindValue(":parent_id", QVariant::fromValue(d->m_id));
	EXCEPT_ELSE(select.exec(), NodeException, "Node::childs", qPrintable(select.lastError().text()));

	while (select.next())
	{
		result += select.value(0).toInt();
	}

	select.finish();

	return result;
}

/*!
 * \brief Return the parent of the node.
 *
 * A node can have only one parent.
 * \see attach(), detach()
 */
int Node::parent(QSqlDatabase db) const
{
	if (d->m_id == -1) return d->m_parentId;

	QSqlQuery select = Manager::self()->getSqlQuery("SELECT parent_id FROM cv_node WHERE id=:child_id", db);
	select.bindValue(":child_id", QVariant::fromValue(d->m_id));
	bool r = select.exec();
	Q_ASSERT_X(r, "Node::parent", qPrintable(select.lastError().text()));

	if (select.first())
	{
		int parent_id = select.value(0).toInt();
		select.finish();
		return parent_id;
	}
	else
	{
		select.finish();
		return d->m_id;
	}
}

//! Make a hash of the node and search the node having the same hash.
int Node::indexOfChild(QSqlDatabase db, const Node & node) const
{
	if (d->m_id == -1) return -1;

	QSqlQuery select = Manager::self()->getSqlQuery("SELECT id FROM cv_node WHERE parent_id=:parent_id and hash=:hash", db);
	select.bindValue(":parent_id", QVariant::fromValue(d->m_id));
	select.bindValue(":hash", QVariant::fromValue(qHash(node)));
	bool result = select.exec();
	EXCEPT_ELSE(result, NodeException, "Node::indexOfChild", qPrintable(select.lastError().text()));

	if (select.next())
	{
		quint64 childId = select.value(0).toUInt();
		select.finish();
		return childId;
	}
	else
	{
		select.finish();
		return -1;
	}
}

//! Return true is the node has a valid content.
bool Node::isValid() const
{
	return d->m_id >= 0;
}

//! Clear the data of the node and make it not valid
void Node::clear()
{
	d->m_datas.clear();

	d->m_line     = -1;
	d->m_parentId = -1;
	d->m_fileId   = -1;
	d->m_id       = -1;
	d->m_isLoaded = false;
	d->m_db       = QSqlDatabase();
}

//! Copy the node
Node & Node::operator=(const Node & node)
{
	this->d = node.d;

	return *this;
}

/* qHash */

/*!
 * \ingroup ContentView2
 * \brief Calculate the hash code for the node \p node.
 *
 * The calculate hash can not to be unique. It can be used for
 * the update to check that the node can't be modified. Or to
 * find quickly a node with it's index.
 */
uint qHash(const Node & node)
{
	node.d->load();

	QString textToHash = QString("%1%2").arg(node.d->m_fileId).arg(node.d->m_line);

	foreach(const QVariant & data, node.d->m_datas.values())
	{
		textToHash += data.toString();
	}

	textToHash.remove(QLatin1Char(' '));

	return qHash(textToHash);
}

} // namespace ContentView2
