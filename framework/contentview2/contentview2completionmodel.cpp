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

/*!
 * \ingroup ContentView2
 * \class CompletionModel
 * \since 0.9.0.0
 *
 * \brief This class is used as model to complete on the content of the ContentView database.
 *
 * This class is used to select the content of the database to present information for the completion.
 * Informations presented are choose by sub-classe.
 */

/*!
 * \brief Create an instance for database \p db and the file \p file.
 *
 * The file container is used to know the id to use for the completion. If the \p file is not already loaded
 * the completion doesn't work.
 */
CompletionModel::CompletionModel(QSqlDatabase db, FileContainer file, QObject * parent) : QSqlQueryModel(parent), m_db(db), m_file(file)
{

}

//! Destroy this instance
CompletionModel::~CompletionModel()
{

}

/*!
 * \brief Return information about the index for the role \p role.
 *
 * In case of a role upper then Node::NODE_USER_VALUE, a Node object
 * is created to get the value.
 */
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

//! This method do nothing
bool CompletionModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role)
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(value);
	Q_UNUSED(role);
	return false;
}

/*!
 * \brief Set the prefix to \e prefix to increase the speed
 *
 * The selection is limited to 5 element to increase the speed. To
 * update the selection, the method setPrefix() update the used prefix and
 * refresh the query.
 */
void CompletionModel::setPrefix(const QString & prefix)
{
	if (m_prefix != prefix)
	{
		m_prefix = prefix;
		select();
	}
}

/*!
 * \brief Return the where clause part of the query used to complete
 *
 * This method is used to select the element to complete. This method
 * can be sub-classing to add restriction on the element used to complete.
 *
 * The current restriction is :
 * \li Select all element of the current project or from the general project
 * \li Select all element where the file is marked as '*' or where the id file
 * is the equals to the file().
 * \li Select all element of the file in the cv_import table.
 */
QString CompletionModel::whereClause() const
{
	return  "WHERE (cv_file.project_id=0 OR cv_file.project_id=:project_id) "
	        "AND cv_file.id=cv_node.file_id "
	        "AND (cv_file.selection='*' OR cv_node.file_id=:id1 OR "
	        "EXISTS (	 SELECT 1 FROM cv_import import "
	        "WHERE import.child_id = cv_node.file_id "
	        "AND import.parent_id = :id2)) ";
}

//! Select the 5 first element return by the where clause of whereClause()
void CompletionModel::select()
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
	if (result)
	{
		setQuery(query);
	}
	else
	{
		ErrorManager::self()->addMessage(m_file.path(), -1, ErrorManager::MessageInformation, tr("Completion not ready"));
		Q_ASSERT_X(result, "CompletionModel::select", qPrintable(query.lastError().text()));

	}
}

//! Return the FileContainer used for the completion
FileContainer CompletionModel::file() const
{
	return m_file;
}

//! Return the database used for the completion (provide for convinience)
QSqlDatabase CompletionModel::database() const
{
	return m_db;
}

//! Return the database used for the completion (provide for convinience)
QSqlDatabase CompletionModel::database()
{
	return m_db;
}

//! Return the first finding node for the given \p name from parameters.
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
