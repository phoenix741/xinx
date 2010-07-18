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

#ifndef _XSLCOMPLETIONNODEMODEL_H_
#define _XSLCOMPLETIONNODEMODEL_H_
#pragma once

// Xinx header
#include "xslcontentviewparser.h"
#include "xmlcompletionparser.h"
#include <contentview2/contentview2completionmodel.h>
#include <contentview2/contentview2node.h>

// Qt header
#include <QStringList>
#include <QMultiHash>

/*!
 * \class XslCompletionNodeModel
 * \brief Model used to complete XSL Stylesheet.
 *
 * This class is used to complete stylesheet.
 */
class XslCompletionNodeModel : public ContentView2::CompletionModel
{
	Q_OBJECT
public:
	enum CompletionTag { NoTags = 0x0, Html = 0x1, Xsl = 0x2 };
	Q_DECLARE_FLAGS(CompletionTags, CompletionTag)

	XslCompletionNodeModel(QSqlDatabase db, ContentView2::FileContainer file, QObject *parent = 0);
	virtual ~XslCompletionNodeModel();

	void setCurrentTemplateName(const QString & name, const QString & mode);

	void setCompleteTags(CompletionTags value);
	void setCompleteAll();
	void setCompleteNone();
	void setCompleteNode();
	void setCompleteAttribute(const QString & baliseName, const QStringList & hiddenAttributeList);
	void setCompleteValue(const QString & baliseName, const QString & attributeName, const QString & applyTemplateMatch);

	QStringList modes(QString templateName) const;
	QStringList params(QString templateName) const;
protected:
	virtual QString whereClause() const;
private:
	enum CompletionMode { COMPLETION_NONE_MODE, COMPLETION_NODE_MODE, COMPLETION_ATTRIBUTE_MODE, COMPLETION_VALUE_MODE, COMPLETION_VALUE_ALL };

	CompletionTags m_completeTags;
	enum CompletionMode m_completionMode;
	QString m_baliseName, m_attributeName, m_applyTemplateMatch, m_currentTemplateName, m_currentTemplateMode;
	ContentView2::Node m_baliseNode, m_attributeNode;
	QStringList m_hiddenAttributeList;
};

#endif /* _XSLCOMPLETIONNODEMODEL_H_ */
