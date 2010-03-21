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

// Xinx header
#include "contentview2node.h"
#include "contentview2file.h"

// Qt header
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSqlRecord>
#include <QHash>

namespace ContentView2
{

/* NodeException */

NodeException::NodeException(QString message, QString nodeName) : XinxException(QString(message).arg(nodeName)), m_nodeName(nodeName)
{
}

const QString & NodeException::getNodeName() const
{
	return m_nodeName;
}

/* PrivateNode */

class PrivateNode : public QSharedData
{
public:
	int m_line;
	int m_fileId;
	QHash<int,QVariant> m_datas;

	long m_id;
	bool m_isLoaded;
	QSqlDatabase m_db;

	void load();

	PrivateNode();
	PrivateNode(const PrivateNode &other);
	~PrivateNode();
};

PrivateNode::PrivateNode() : m_line(-1), m_id(-1), m_isLoaded(false)
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

	QSqlQuery selectQuery("SELECT name, type, icon, display_name, tips, completion_value, line, file_id, hash, "
	                      "property1, property2, property3, property4, property5, property6, "
	                      "property7, property8, property9, property10 "
	                      "FROM cv_node WHERE id=:id", m_db);
	selectQuery.bindValue(":id", QVariant::fromValue(m_id));
	bool result = selectQuery.exec();
	Q_ASSERT_X(result, "PrivateNode::load", qPrintable(selectQuery.lastError().text()));
	if (! selectQuery.first())
	{
		throw NodeException(Node::tr("Can't find the node %1"), QString("%1").arg(m_id));
	}

	m_datas.clear();

	m_datas.insert(Node::NODE_NAME, selectQuery.value(0).toString());
	m_datas.insert(Node::NODE_TYPE, selectQuery.value(1).toString());
	m_datas.insert(Node::NODE_ICON, selectQuery.value(2).toString());
	m_datas.insert(Node::NODE_DISPLAY_NAME, selectQuery.value(3).toString());
	m_datas.insert(Node::NODE_DISPLAY_TIPS, selectQuery.value(4).toString());
	m_datas.insert(Node::NODE_COMPLETE_FORM, selectQuery.value(5).toString());
	m_line = selectQuery.value(6).toInt();

	for (int i = 0; i < 10 ; i++)
	{
		QVariant value = selectQuery.value(9 + i);
		if (value.isValid())
		{
			m_datas.insert(Node::NODE_USER_VALUE + i, value.toString());
		}
	}

	QSqlQuery selectPropertiesQuery("SELECT ord, value FROM cv_node_property WHERE node_id=:node_id", m_db);
	selectPropertiesQuery.bindValue(":node_id", QVariant::fromValue(m_id));
	result = selectPropertiesQuery.exec();
	Q_ASSERT_X(result, "PrivateNode::load", qPrintable(selectPropertiesQuery.lastError().text()));

	while (selectPropertiesQuery.next())
	{
		m_datas.insert(selectPropertiesQuery.value(0).toInt(), selectPropertiesQuery.value(1).toString());
	}

	m_fileId   = selectQuery.value(7).toInt();
	m_isLoaded = true;
	m_db       = QSqlDatabase();
}

/* Node */

Node::Node()
{
	d = new PrivateNode;
}

Node::Node(const Node & other) : d(other.d)
{
}

Node::Node(QSqlDatabase db, uint id)
{
	d = new PrivateNode;
	load(db, id);
}

Node::~Node()
{
}

void Node::load(QSqlDatabase db, uint id)
{
	d->m_isLoaded = false;
	d->m_id = id;
	d->m_db = db;
}

void Node::reload(QSqlDatabase db)
{
	if (d->m_id >= 0)
		load(db, d->m_id);
}

uint Node::create(QSqlDatabase db, int forcedId)
{
	QSqlQuery selectQuery("SELECT 1 FROM cv_file WHERE id=:file_id", db);
	selectQuery.bindValue(":file_id", d->m_fileId);

	bool result = selectQuery.exec();
	Q_ASSERT_X(result, "Node::create", qPrintable(selectQuery.lastError().text()));

	if (! selectQuery.first())
	{
		throw NodeException(tr("Can't find the file node %1"), d->m_datas.value(Node::NODE_NAME).toString());
	}

	QSqlQuery insertQuery(db);
	if (forcedId == -1)
	{
		insertQuery.prepare("INSERT INTO cv_node(name, type, icon, display_name, tips, completion_value, line, file_id, hash, property1, property2, property3, property4, property5, property6, property7, property8, property9, property10) "
		                    "VALUES(:name, :type, :icon, :display_name, :tips, :completion_value, :line, :file_id, :hash, :property1, :property2, :property3, :property4, :property5, :property6, :property7, :property8, :property9, :property10)");
	}
	else
	{
		insertQuery.prepare("INSERT INTO cv_node(id, name, type, icon, display_name, tips, completion_value, line, file_id, hash, property1, property2, property3, property4, property5, property6, property7, property8, property9, property10) "
		                    "VALUES(:forced_id, :name, :type, :icon, :display_name, :tips, :completion_value, :line, :file_id, :hash, :property1, :property2, :property3, :property4, :property5, :property6, :property7, :property8, :property9, :property10)");
		insertQuery.bindValue(":forced_id", forcedId);
	}

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

	result = insertQuery.exec();
	Q_ASSERT_X(result, "Node::create", qPrintable(insertQuery.lastError().text()));

	uint newId = insertQuery.lastInsertId().toInt();

	QSqlQuery insertPropertyQuery("INSERT INTO cv_node_property(node_id, ord, value) "
	                              "VALUES(:node_id, :ord, :value)", db);

	insertPropertyQuery.bindValue(":node_id", newId);
	foreach(const int ord, d->m_datas.keys())
	{
		if (ord < Node::NODE_USER_VALUE + 10) continue;

		insertPropertyQuery.bindValue(":ord", ord);
		insertPropertyQuery.bindValue(":value", d->m_datas.value(ord));

		result = insertPropertyQuery.exec();
		Q_ASSERT_X(result, "Node::create", qPrintable(insertPropertyQuery.lastError().text()));
	}

	d->m_id       = newId;
	d->m_isLoaded = true;
	return newId;
}

void Node::update(QSqlDatabase db)
{
	Q_ASSERT_X(d->m_id >= 0, "Node::update", "The node must be initialized");
	d->load();

	QSqlQuery select("SELECT hash FROM cv_node WHERE id=:id", db);
	select.bindValue(":id", QVariant::fromValue(d->m_id));
	bool result = select.exec();
	Q_ASSERT_X(result, "Node::update", qPrintable(select.lastError().text()));

	if (! select.next())
	{
		throw NodeException(tr("Can't find the node %1 (%2)").arg(d->m_id), d->m_datas.value(Node::NODE_NAME).toString());
	}

	uint hash    = select.value(0).toUInt();
	uint newHash = qHash(*this);

	if (hash == newHash) return;

	QSqlQuery updateQuery("UPDATE cv_node "
	                      "SET	name=:name , "
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

	result = updateQuery.exec();
	Q_ASSERT_X(result, "Node::update", qPrintable(updateQuery.lastError().text()));

	QSqlQuery deleteQuery("DELETE FROM cv_node_property WHERE node_id=:id", db);
	deleteQuery.bindValue(":id", QVariant::fromValue(d->m_id));
	result = deleteQuery.exec();
	Q_ASSERT_X(result, "Node::update", qPrintable(deleteQuery.lastError().text()));

	QSqlQuery insertPropertyQuery("INSERT INTO cv_node_property(node_id, ord, value) "
	                              "VALUES(:node_id, :ord, :value)", db);

	insertPropertyQuery.bindValue(":node_id", QVariant::fromValue(d->m_id));
	foreach(const int ord, d->m_datas.keys())
	{
		if (ord < Node::NODE_USER_VALUE + 10) continue;

		insertPropertyQuery.bindValue(":ord", ord);
		insertPropertyQuery.bindValue(":value", d->m_datas.value(ord));

		result = insertPropertyQuery.exec();
		Q_ASSERT_X(result, "Node::update", qPrintable(insertPropertyQuery.lastError().text()));
	}
}

void Node::destroy(QSqlDatabase db)
{
	Q_ASSERT_X(d->m_id >= 0, "Node::destroy", "The node must be initialized");
	destroyChilds(db);

	QSqlQuery deleteQuery1("DELETE FROM cv_node WHERE id=:id", db);
	deleteQuery1.bindValue(":id", QVariant::fromValue(d->m_id));
	bool result = deleteQuery1.exec();
	Q_ASSERT_X(result, "Node::destroy", qPrintable(deleteQuery1.lastError().text()));

	QSqlQuery deleteQuery2("DELETE FROM cv_node_property WHERE node_id=:id", db);
	deleteQuery2.bindValue(":id", QVariant::fromValue(d->m_id));
	result = deleteQuery2.exec();
	Q_ASSERT_X(result, "Node::destroy", qPrintable(deleteQuery2.lastError().text()));

	QSqlQuery deleteQuery3("DELETE FROM cv_link WHERE parent_id=:id1 or child_id=:id2", db);
	deleteQuery3.bindValue(":id1", QVariant::fromValue(d->m_id));
	deleteQuery3.bindValue(":id2", QVariant::fromValue(d->m_id));
	result = deleteQuery3.exec();
	Q_ASSERT_X(result, "Node::destroy", qPrintable(deleteQuery3.lastError().text()));

	d->m_id = -1;
}

uint Node::nodeId() const
{
	return d->m_id;
}

bool Node::attach(QSqlDatabase db, uint parentId)
{
	Q_ASSERT_X(d->m_id >= 0, "Node::attach", "The node must be initialized");

	QSqlQuery checkId("SELECT 1 FROM cv_link WHERE parent_id=:parent AND child_id=:child", db);
	checkId.bindValue(":parent", QVariant::fromValue(parentId));
	checkId.bindValue(":child", QVariant::fromValue(d->m_id));
	bool result = checkId.exec();
	Q_ASSERT_X(result, "Node::attach", qPrintable(checkId.lastError().text()));

	if (checkId.next())
		return false;

	QSqlQuery insert("INSERT INTO cv_link(parent_id, child_id) VALUES( :parent, :child )", db);
	insert.bindValue(":parent", QVariant::fromValue(parentId));
	insert.bindValue(":child", QVariant::fromValue(d->m_id));
	result = insert.exec();
	Q_ASSERT_X(result, "Node::attach", qPrintable(insert.lastError().text()));

	return true;
}

void Node::detach(QSqlDatabase db, uint parentId)
{
	Q_ASSERT_X(d->m_id >= 0, "Node::detach", "The node must be initialized");

	QSqlQuery remove("DELETE FROM cv_link WHERE parent_id=:parent AND child_id=:child", db);
	remove.bindValue(":parent", QVariant::fromValue(parentId));
	remove.bindValue(":child", QVariant::fromValue(d->m_id));
	bool result = remove.exec();
	Q_ASSERT_X(result, "Node::detach", qPrintable(remove.lastError().text()));
}

void Node::destroyChilds(QSqlDatabase db)
{
	if (d->m_id == -1) return ;

	foreach(int id, childs(db))
	{
		Node child(db, id);
		child.destroy(db);
	}
}

int Node::line() const
{
	d->load();
	return d->m_line;
}

void Node::setLine(int value)
{
	d->load();
	d->m_line = value;
}

QString Node::filename(QSqlDatabase db) const
{
	QSqlQuery selectQuery("SELECT path FROM cv_file WHERE id=:file_id", db);
	selectQuery.bindValue(":file_id", d->m_fileId);

	bool result = selectQuery.exec();
	Q_ASSERT_X(result, "Node::filename", qPrintable(selectQuery.lastError().text()));

	if (selectQuery.first())
	{
		return selectQuery.value(0).toString();
	}
	else
	{
		return QString();
	}
}

int Node::fileId() const
{
	d->load();
	return d->m_fileId;
}

void Node::setFileId(int value)
{
	Q_ASSERT_X(d->m_id == -1, "Node::setFileId", "The filename can't be modified after node creation");
	d->m_fileId = value;
}

void Node::setFile(const File & file)
{
	Q_ASSERT_X(d->m_id == -1, "Node::setFile", "The filename can't be modified after node creation");
	d->m_fileId = file.fileId();
}

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

QVariant Node::data(int index) const
{
	d->load();
	return d->m_datas.value(index);
}

void Node::setData(const QVariant & value, int index)
{
	d->load();
	if (d->m_datas.value(index) != value)
	{
		d->m_datas.insert(index, value);
	}
}

QList<int> Node::childs(QSqlDatabase db) const
{
	QList<int> result;
	if (d->m_id == -1) return result;

	QSqlQuery select("SELECT child_id FROM cv_link WHERE parent_id=:parent_id", db);
	select.bindValue(":parent_id", QVariant::fromValue(d->m_id));
	bool r = select.exec();
	Q_ASSERT_X(r, "Node::childs", qPrintable(select.lastError().text()));

	while (select.next())
	{
		result += select.value(0).toInt();
	}

	return result;
}

QList<int> Node::parents(QSqlDatabase db) const
{
	QList<int> result;
	if (d->m_id == -1) return result;

	QSqlQuery select("SELECT parent_id FROM cv_link WHERE child_id=:child_id", db);
	select.bindValue(":child_id", QVariant::fromValue(d->m_id));
	bool r = select.exec();
	Q_ASSERT_X(r, "Node::parents", qPrintable(select.lastError().text()));

	while (select.next())
	{
		result += select.value(0).toInt();
	}

	return result;
}

int Node::indexOfChild(QSqlDatabase db, const Node & node) const
{
	if (d->m_id == -1) return -1;

	QSqlQuery select("SELECT child_id FROM cv_link, cv_node WHERE cv_link.parent_id=:parent_id and cv_node.id=cv_link.child_id and cv_node.hash=:hash", db);
	select.bindValue(":parent_id", QVariant::fromValue(d->m_id));
	select.bindValue(":hash", QVariant::fromValue(qHash(node)));
	bool result = select.exec();
	Q_ASSERT_X(result, "Node::attach", qPrintable(select.lastError().text()));

	if (select.next())
	{
		return select.value(0).toUInt();
	}
	else
	{
		return -1;
	}
}

bool Node::isValid() const
{
	return d->m_id >= 0;
}

void Node::clear()
{
	d->m_datas.clear();

	d->m_line     = -1;
	d->m_fileId   = -1;
	d->m_id       = -1;
	d->m_isLoaded = false;
	d->m_db       = QSqlDatabase();
}

Node & Node::operator=(const Node & node)
{
	this->d = node.d;

	return *this;
}

/* qHash */

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
