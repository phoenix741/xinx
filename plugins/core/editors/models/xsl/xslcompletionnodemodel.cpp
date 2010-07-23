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
#include "xslcompletionnodemodel.h"
#include "xslcontentviewparser.h"

// Qt header
#include <QSqlQuery>
#include <QSqlError>

/* XslCompletionNodeModel */

XslCompletionNodeModel::XslCompletionNodeModel(QSqlDatabase db, ContentView2::FileContainer file, QObject *parent) : ContentView2::CompletionModel(db, file, parent), m_completionMode(COMPLETION_NONE_MODE)
{

}

XslCompletionNodeModel::~XslCompletionNodeModel()
{

}

QStringList XslCompletionNodeModel::params(QString templateName) const
{
	QStringList list;

	QList<QVariant> parameters;

	QString paramsQueryStr
	= "SELECT child.name "
	  "FROM cv_file, cv_node, cv_node child " + ContentView2::CompletionModel::whereClause(parameters) + " AND cv_node.id = child.parent_id "
	  "AND cv_node.type=? AND child.type=? AND cv_node.name=?";

	QSqlQuery paramsQuery(paramsQueryStr, database());
	foreach(const QVariant & p, parameters)
		paramsQuery.addBindValue(p);
	paramsQuery.addBindValue("XslTemplate");
	paramsQuery.addBindValue("XslParam");
	paramsQuery.addBindValue(templateName);

	bool result = paramsQuery.exec();
	Q_ASSERT_X(result, "XslCompletionNodeModel::params", qPrintable(paramsQuery.lastError().text()));

	while (paramsQuery.next())
	{
		list << paramsQuery.value(0).toString();
	}

	return list;
}

void XslCompletionNodeModel::setCompleteTags(CompletionTags value)
{
	m_completeTags = value;
}

void XslCompletionNodeModel::setCurrentTemplateName(const QString & name, const QString & mode)
{
	m_currentTemplateName = name;
	m_currentTemplateMode = mode;
}

void XslCompletionNodeModel::setCompleteAll()
{
	m_completionMode = COMPLETION_VALUE_ALL;
}

void XslCompletionNodeModel::setCompleteNone()
{
	m_completionMode = COMPLETION_NONE_MODE;
}

void XslCompletionNodeModel::setCompleteNode()
{
	m_completionMode = COMPLETION_NODE_MODE;
}

void XslCompletionNodeModel::setCompleteAttribute(const QString & baliseName, const QStringList & hiddenAttributeList)
{
	if ((m_completionMode != COMPLETION_ATTRIBUTE_MODE) || (m_baliseName != baliseName) || (m_hiddenAttributeList != hiddenAttributeList))
	{
		m_completionMode      = COMPLETION_ATTRIBUTE_MODE;
		m_baliseName          = baliseName;
		m_baliseNode          = XmlCompletionParser::self()->balise(baliseName);
		m_hiddenAttributeList = hiddenAttributeList;
	}
}

void XslCompletionNodeModel::setCompleteValue(const QString & baliseName, const QString & attributeName, const QString & applyTemplateMatch)
{
	QString tempApplyTemplateMatch = applyTemplateMatch;

	if (tempApplyTemplateMatch.contains("/"))
		tempApplyTemplateMatch.remove(0, tempApplyTemplateMatch.lastIndexOf("/") + 1);

	if ((m_completionMode != COMPLETION_VALUE_MODE) || (m_baliseName != baliseName) || (m_attributeName != attributeName) || (m_applyTemplateMatch != tempApplyTemplateMatch))
	{
		m_completionMode     = COMPLETION_VALUE_MODE;
		m_baliseName         = baliseName;
		m_baliseNode         = XmlCompletionParser::self()->balise(baliseName);
		m_attributeName      = attributeName;
		m_attributeNode      = XmlCompletionParser::self()->baliseAttribute(baliseName, attributeName);
		m_applyTemplateMatch = tempApplyTemplateMatch;
	}
}

QString XslCompletionNodeModel::completionSelectClause() const
{
	return "cv_node.display_name as display_name, cv_node.name as name, cv_node.icon as icon, cv_node.id as id, cv_node.type as type, cv_node.completion_value as value";
}

QString XslCompletionNodeModel::completionSnipetSelectQuery(QList<QVariant> & parameters) const
{
	QString query;

	query =	"SELECT " + completionSelectClause() +
			"  FROM " + completionTableClause() +
			" WHERE " + completionWhereClause(parameters) +
			"   AND ( cv_node.type = 'Snipet' )";

	return query;
}

QString XslCompletionNodeModel::completionNodeSelectQuery(QList<QVariant> & parameters) const
{
	QString query;

	query =	"SELECT " + completionSelectClause() +
			"  FROM " + completionTableClause() +
			" WHERE " + completionWhereClause(parameters) +
			"   AND ( cv_node.type='XmlBalise' ";

	if (m_completeTags.testFlag(XslCompletionNodeModel::Xsl))
	{
		query += " AND property1 = ?"; // XmlCompletionParser::NODE_XML_TYPE
		parameters << "stylesheet";
	}
	if (m_completeTags.testFlag(XslCompletionNodeModel::Html))
	{
		query += " AND property1 = ?"; // XmlCompletionParser::NODE_XML_TYPE
		parameters << "html";
	}

	query += ")";

	return query;
}

QString XslCompletionNodeModel::completionAttributeSelectQuery(QList<QVariant> & parameters) const
{
	QString query;

	query =	"SELECT " + completionSelectClause() +
			"  FROM " + completionTableClause() +
			" WHERE " + completionWhereClause(parameters) +
			"   AND( cv_node.type='XmlAttribute' ";

	if (! m_hiddenAttributeList.isEmpty())
	{
		query += " AND cv_node.name NOT IN ( ";
		for(int i = 1; i <= m_hiddenAttributeList.size(); i++)
		{
			if( i == m_hiddenAttributeList.size() )
			{
				query += "?";
			}
			else
			{
				query += "?, ";
			}

			parameters << m_hiddenAttributeList.at(i - 1);
		}
		query += " ) ";
	}
	query += " AND cv_node.parent_id=? ";
	query += ")";

	parameters << m_baliseNode.nodeId();

	return query;
}

QString XslCompletionNodeModel::completionValueVariableSelectQuery(QList<QVariant> & parameters) const
{
	QString query;

	query =	"SELECT " + completionSelectClause() +
			"  FROM " + completionTableClause() + ", cv_node nodeParent"
			" WHERE " + completionWhereClause(parameters) +
			"   AND  nodeParent.id=cv_node.parent_id "
			"   AND ( cv_node.type in ('XslParam', 'XslVariable')) AND (nodeParent.type <> 'XslTemplate' OR ( nodeParent.name=? AND nodeParent.property1=? )) ";

	parameters << m_currentTemplateName;
	parameters << m_currentTemplateMode;

	return query;
}

QString XslCompletionNodeModel::completionValueTemplateSelectQuery(QList<QVariant> & parameters) const
{
	QString query;

	query =	"SELECT " + completionSelectClause() +
			"  FROM " + completionTableClause() +
			" WHERE " + completionWhereClause(parameters) +
			"   AND ( cv_node.type = 'XslTemplate' )";

	return query;
}

QString XslCompletionNodeModel::completionValueHtmlSelectQuery(QList<QVariant> & parameters) const
{
	QString query;

	query =	"SELECT " + completionSelectClause() +
			"  FROM " + completionTableClause() +
			" WHERE " + completionWhereClause(parameters) +
			"   AND ( cv_node.type = 'XmlValue' AND cv_node.parent_id=? )";

	parameters << m_attributeNode.nodeId();

	return query;
}

QString XslCompletionNodeModel::completionSelectQuery(QList<QVariant> & parameters) const
{
	QString query;
	query = "SELECT * FROM (" +
			completionSnipetSelectQuery(parameters);

	if (m_completionMode == COMPLETION_NONE_MODE)
	{
		// Only snipet
	}
	else if ((m_completionMode == COMPLETION_NODE_MODE) || (m_completionMode == COMPLETION_VALUE_ALL))
	{
		query += " UNION ALL " + completionNodeSelectQuery(parameters);
	}
	else if ((m_completionMode == COMPLETION_ATTRIBUTE_MODE) || (m_completionMode == COMPLETION_VALUE_ALL))
	{
		query += " UNION ALL " + completionAttributeSelectQuery(parameters);
	}
	else if ((m_completionMode == COMPLETION_VALUE_MODE) || (m_completionMode == COMPLETION_VALUE_ALL))
	{
		query += " UNION ALL " + completionValueHtmlSelectQuery(parameters);
		query += " UNION ALL " + completionValueTemplateSelectQuery(parameters);
		query += " UNION ALL " + completionValueVariableSelectQuery(parameters);
	}

	query += " )";

	return query;
}
