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

#ifndef XSLTEXTEDITOR_H_
#define XSLTEXTEDITOR_H_
#pragma once

// Xinx header
#include <editors/xinxcodeedit.h>
#include "editors/widgeteditor/xml/xmltexteditor.h"
#include "editors/models/xsl/stylesheet_templatenode.h"

// Qt header
#include <QStringList>
#include <QHash>

// QCodeEdit header
#include <document/qdocumentcursor.h>

/* XslTextEditor */

class XslCompletionNodeModel;

namespace Core
{
namespace Stylesheet
{
class TemplateNode;
}
}

class XslTextEditor : public XmlTextEditor
{
	Q_OBJECT
public:
	XslTextEditor(QWidget * parent = 0);
	virtual ~XslTextEditor();

	virtual QDocumentCursor insertCompletionBalises(Core::BaliseDefinition::XmlContextType* context, QDocumentCursor& tc, QSharedPointer< Core::BaliseDefinition::BaliseNode > balise);

	QSharedPointer<Core::Stylesheet::TemplateNode> localNodeOfTemplate(const ContentView3::NodePtr & node, const QString & templateName, bool isNamed);
	QSharedPointer<Core::Stylesheet::TemplateNode> globalNodeOfTemplate(const QString & templateName, bool isNamed);

};

#endif /*XSLTEXTEDITOR_H_*/
