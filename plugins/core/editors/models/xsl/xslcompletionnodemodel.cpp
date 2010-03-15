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

QStringList XslCompletionNodeModel::modes(QString templateName) const
{
	QStringList list;

	QString modeQueryStr
	= "SELECT distinct cv_node_property.value "
	  "FROM cv_node, cv_node_property " + ContentView2::CompletionModel::whereClause() + " AND cv_node.id = cv_node_property.node_id "
	  "AND cv_node_property.ord=:ord AND cv_node.type=:type AND cv_node.name=:name";

	QSqlQuery modeQuery(modeQueryStr, database());
	modeQuery.bindValue(":ord", ContentView2::Node::NODE_USER_VALUE);
	modeQuery.bindValue(":type", "XslTemplate");
	modeQuery.bindValue(":name", templateName);

	bool result = modeQuery.exec();
	Q_ASSERT_X(result, "XslCompletionNodeModel::modes", qPrintable(modeQuery.lastError().text()));

	while (modeQuery.next())
	{
		list << modeQuery.value(0).toString();
	}

	return list;
}

QStringList XslCompletionNodeModel::params(QString templateName) const
{
	QStringList list;

	QString paramsQueryStr
	= "SELECT child.name "
	  "FROM cv_file, cv_node, cv_node child, cv_link " + ContentView2::CompletionModel::whereClause() + " AND cv_node.id = cv_link.parent_id "
	  "AND child.id = cv_link.child_id AND cv_node.type=:type AND child.type=:param AND cv_node.name=:name";

	QSqlQuery paramsQuery(paramsQueryStr, database());
	paramsQuery.bindValue(":project_id", file().file(database()).projectId());
	paramsQuery.bindValue(":id1", file().file(database()).fileId());
	paramsQuery.bindValue(":id2", file().file(database()).fileId());
	paramsQuery.bindValue(":type", "XslTemplate");
	paramsQuery.bindValue(":param", "XslParam");
	paramsQuery.bindValue(":name", templateName);

	bool result = paramsQuery.exec();
	Q_ASSERT_X(result, "XslCompletionNodeModel::params", qPrintable(paramsQuery.lastError().text()));

	while (paramsQuery.next())
	{
		list << paramsQuery.value(0).toString();
	}

	return list;
}

ContentView2::Node XslCompletionNodeModel::nodeOfWord(const QString & name) const
{
	QSqlQuery q(QString("SELECT cv_node.id FROM cv_node %1 AND cv_node.name = :name").arg(whereClause()), database());
	q.bindValue(":name", name);
	if (q.first())
	{
		uint id = q.value(0).toUInt();
		return ContentView2::Node(database(), id);
	}

	return ContentView2::Node();
}

void XslCompletionNodeModel::setCompleteTags(CompletionTags value)
{
	if (value != m_completeTags)
	{
		m_completeTags = value;

		if (m_completionMode == COMPLETION_NODE_MODE)
			select();
	}
}

void XslCompletionNodeModel::setCurrentTemplateName(const QString & name, const QString & mode)
{
	if ((m_currentTemplateName != name) || (m_currentTemplateMode != mode))
	{
		m_currentTemplateName = name;
		m_currentTemplateMode = mode;

		if (m_completionMode == COMPLETION_VALUE_MODE)
			select();
	}
}

void XslCompletionNodeModel::setCompleteNone()
{
	if (m_completionMode != COMPLETION_NONE_MODE)
	{
		m_completionMode = COMPLETION_NONE_MODE;

		select();
	}
}

void XslCompletionNodeModel::setCompleteNode()
{
	if (m_completionMode != COMPLETION_NODE_MODE)
	{
		m_completionMode = COMPLETION_NODE_MODE;

		select();
	}
}

void XslCompletionNodeModel::setCompleteAttribute(const QString & baliseName, const QStringList & hiddenAttributeList)
{
	if ((m_completionMode != COMPLETION_ATTRIBUTE_MODE) || (m_baliseName != baliseName) || (m_hiddenAttributeList != hiddenAttributeList))
	{
		m_completionMode      = COMPLETION_ATTRIBUTE_MODE;
		m_baliseName          = baliseName;
		m_baliseNode          = XmlCompletionParser::self()->balise(baliseName);
		m_hiddenAttributeList = hiddenAttributeList;

		select();
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

		select();
	}
}

QString XslCompletionNodeModel::whereClause() const
{
	QString clause = ContentView2::CompletionModel::whereClause();

	clause += " AND ( ( cv_node.type = 'Snipet' ) ";

	if (m_completionMode == COMPLETION_NONE_MODE)
	{
		// Only snipet
	}
	else if (m_completionMode == COMPLETION_NODE_MODE)
	{
		clause += " OR (cv_node.type='XmlBalise' ";
		if (m_completeTags.testFlag(XslCompletionNodeModel::Xsl))
		{
			clause += QString(" AND EXISTS (SELECT 1 FROM cv_node_property WHERE cv_node_property.node_id=cv_node.id AND cv_node_property.ord=%1 AND cv_node_property.value=%2) ")
			          .arg((int)XmlCompletionParser::NODE_XML_TYPE).arg("stylesheet");
		}
		if (m_completeTags.testFlag(XslCompletionNodeModel::Html))
		{
			clause += QString(" AND EXISTS (SELECT 1 FROM cv_node_property WHERE cv_node_property.node_id=cv_node.id AND cv_node_property.ord=%1 AND cv_node_property.value=%2) ")
			          .arg((int)XmlCompletionParser::NODE_XML_TYPE).arg("html");
		}
		clause += ")";
	}
	else if (m_completionMode == COMPLETION_ATTRIBUTE_MODE)
	{
		clause += " OR ( cv_node.type='XmlAttribute' ";
		if (! m_hiddenAttributeList.isEmpty())
			clause += QString(" and cv_node.name not in ( '%1' ) ").arg(m_hiddenAttributeList.join("', '"));
		clause += QString(" AND EXISTS ( "
		                  "	SELECT 1 "
		                  "	FROM cv_link "
		                  "	WHERE cv_link.parent_id=%1 "
		                  "	  AND cv_link.child_id=cv_node.id) ").arg(m_baliseNode.nodeId());
		clause += ")";
	}
	else if (m_completionMode == COMPLETION_VALUE_MODE)
	{
		clause += QString(" OR ( cv_node.type = 'XmlValue' AND EXISTS ( "
		                  "	SELECT 1 "
		                  "	FROM cv_link linkAttribute "
		                  "	WHERE linkAttribute.parent_id=%1 "
		                  "	  AND linkAttribute.child_id=cv_node.id)) ").arg(m_attributeNode.nodeId());
		clause += QString(" OR ( cv_node.type in ('XslParam', 'XslVariable') AND EXISTS ( "
		                  "	SELECT 1 "
		                  "	FROM cv_link linkParent, cv_node nodeParent LEFT JOIN cv_node_property ON nodeParent.id=cv_node_property.node_id AND cv_node_property.ord=%1"
		                  "	WHERE linkParent.parent_id=nodeParent.id "
		                  "	  AND linkParent.child_id=cv_node.id"
		                  "	  AND (nodeParent.type <> 'XslTemplate' OR ( nodeParent.name='%2' AND cv_node_property.value='%3' )))) ")
		          .arg(ContentView2::Node::NODE_USER_VALUE).arg(m_currentTemplateName).arg(m_currentTemplateMode) ;
		clause += QString(" OR ( cv_node.type = 'XslTemplate' ) ");
	}

	clause += ")";

	return clause;
}
