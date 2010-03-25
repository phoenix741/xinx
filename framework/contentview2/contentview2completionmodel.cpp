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
#include "contentview2completionmodel.h"
#include "contentview2/contentview2cache.h"
#include "contentview2/contentview2manager.h"
#include "project/xinxproject.h"

// Qt header
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QIcon>

namespace ContentView2
{

/* CompletionModel */

CompletionModel::CompletionModel(QSqlDatabase db, FileContainer file, QObject * parent) : QSqlQueryModel(parent), m_db(db), m_file(file)
{

}

CompletionModel::~CompletionModel()
{

}

QVariant CompletionModel::data(const QModelIndex &idx, int role) const
{
	if (! idx.isValid()) return QVariant();

	if (role == Qt::DecorationRole)
	{
		const int row = idx.row();

		QString imageName = QSqlQueryModel::data(index(row, 2)).toString();
		QImage image = QImage(imageName);
		if (! image.isNull())
			return image.scaled(QSize(16,16));
		else
			return QVariant();
	}
	else if (role == ContentView2::Node::NODE_NAME)
	{
		const int row = idx.row();

		return QSqlQueryModel::data(index(row, 1)).toString();
	}
	else if (role == ContentView2::Node::NODE_ID)
	{
		const int row = idx.row();

		return QSqlQueryModel::data(index(row, 3)).toInt();
	}
	else if (role == ContentView2::Node::NODE_TYPE)
	{
		const int row = idx.row();
		QSqlRecord rec = record(row);

		return QSqlQueryModel::data(index(row, 4)).toString();
	}
	else if (role == ContentView2::Node::NODE_COMPLETE_FORM)
	{
		const int row = idx.row();
		QSqlRecord rec = record(row);

		return QSqlQueryModel::data(index(row, 5)).toString();
	}
	else if (role >= ContentView2::Node::NODE_USER_VALUE)
	{
		const int row = idx.row();
		QSqlRecord rec = record(row);

		ContentView2::Node node(m_db, QSqlQueryModel::data(index(row, 3)).toUInt());
		return node.data(role);
	}

	return QSqlQueryModel::data(idx, role);
}

bool CompletionModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role)
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(value);
	Q_UNUSED(role);
	return false;
}

void CompletionModel::setPrefix(const QString & prefix)
{
	if (m_prefix != prefix)
	{
		m_prefix = prefix;
		select();
	}
}

QString CompletionModel::whereClause() const
{
	return  "WHERE (cv_file.project_id=0 OR cv_file.project_id=:project_id) "
	        "AND cv_file.id=cv_node.file_id "
	        "AND (cv_file.selection='*' OR cv_node.file_id=:id1 OR "
	        "EXISTS (	 SELECT 1 FROM cv_import import "
	        "WHERE import.child_id = cv_node.file_id "
	        "AND import.parent_id = :id2)) ";
}

void CompletionModel::select()
{
	if (m_file.isValid(m_db))
	{
		m_file.reload(m_db);
		QSqlQuery query(m_db);

		// Order by clause
		QString queryStr =
		    "SELECT cv_node.display_name, cv_node.name, cv_node.icon, cv_node.id, cv_node.type, cv_node.completion_value "
		    "FROM cv_node, cv_file " + whereClause() + " AND cv_node.display_name like ifnull(:prefix,'')||'%' "
			"ORDER BY lower(cv_node.display_name) LIMIT 5";

		// Set the query used all snipet
		query.prepare(queryStr);
		query.bindValue(":project_id", m_file.file(m_db).projectId());
		query.bindValue(":id1", m_file.file(m_db).fileId());
		query.bindValue(":id2", m_file.file(m_db).fileId());
		query.bindValue(":prefix", m_prefix);

		bool result = query.exec();
		Q_ASSERT_X(result, "CompletionModel::select", qPrintable(query.lastError().text()));
		setQuery(query);
	}
}

FileContainer CompletionModel::file() const
{
	return m_file;
}


QSqlDatabase CompletionModel::database() const
{
	return m_db;
}

QSqlDatabase CompletionModel::database()
{
	return m_db;
}

ContentView2::Node CompletionModel::nodeOfWord(const QString & name) const
{
	QSqlQuery q(QString("SELECT cv_node.id FROM cv_file, cv_node %1 AND cv_node.name = :name").arg(whereClause()), database());
	q.bindValue(":project_id", file().file(database()).projectId());
	q.bindValue(":id1", file().file(database()).fileId());
	q.bindValue(":id2", file().file(database()).fileId());
	q.bindValue(":name", name);
	bool result = q.exec();
	Q_ASSERT_X(result, "CompletionModel::nodeOfWord", qPrintable(q.lastError().text()));

	if (q.first())
	{
		uint id = q.value(0).toUInt();
		return ContentView2::Node(database(), id);
	}

	return ContentView2::Node();
}

} // namespace ContentView2
