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
#include "xsltexteditor.h"
#include <config/selfwebpluginsettings.h>
#include <editors/models/html_xsl_base/xmlcontexttype.h>
#include <contentview3/file.h>
#include <contentview3/filenode.h>
#include <qdocumentline.h>
#include <project/xinxprojectproject.h>
#include <codecompletion/completer.h>
#include <editors/models/xsl/stylesheet_paramnode.h>

/* XslTextEditor */

XslTextEditor::XslTextEditor(QWidget * parent) : XmlTextEditor(parent)
{

}

XslTextEditor::~XslTextEditor()
{

}

QSharedPointer<Core::Stylesheet::TemplateNode> XslTextEditor::localNodeOfTemplate(const ContentView3::NodePtr& node, const QString& templateName, bool isNamed)
{
	foreach(ContentView3::NodePtr child, node->childs())
	{
		QSharedPointer<Core::Stylesheet::TemplateNode> templateNode = child.dynamicCast<Core::Stylesheet::TemplateNode>();
		if (templateNode && (templateNode->name() == templateName) &&  templateNode->isNamedTemplate() == isNamed)
		{
			return templateNode;
		}
	}

	return QSharedPointer<Core::Stylesheet::TemplateNode> ();
}


QSharedPointer<Core::Stylesheet::TemplateNode> XslTextEditor::globalNodeOfTemplate(const QString& templateName, bool isNamed)
{
	ContentView3::FilePtr filePtr = file().toStrongRef();
	if (filePtr.isNull()) return QSharedPointer<Core::Stylesheet::TemplateNode>();

	QSharedPointer<Core::Stylesheet::TemplateNode> node = localNodeOfTemplate(filePtr->rootNode().dynamicCast<ContentView3::Node> (), templateName, isNamed);
	if (node.isNull() && filePtr->project())
	{
		foreach(const ContentView3::FilePtr & import, filePtr->project()->cache()->importOf(filePtr))
		{
			node = localNodeOfTemplate(import->rootNode().dynamicCast<ContentView3::Node> (), templateName, isNamed);
			if (node)
			{
				break;
			}
		}
	}
	return node;
}

void XslTextEditor::insertDragAndDropText(const QString& text)
{
	CodeCompletion::Context context = completer()->context();
	Core::BaliseDefinition::XmlContextType * context_type = dynamic_cast<Core::BaliseDefinition::XmlContextType*>(context.context(XML_CONTEXT_TYPE));
	if (! context_type)
	{
		qWarning() << tr("No XML context type found. It's a problem");
		XinxCodeEdit::insertDragAndDropText(text);
		return;
	}

	if (context_type->isXsl() && (context_type->xmlnsList().value(context_type->balise().nameSpacePrefix()) == "http://www.w3.org/1999/XSL/Transform"))
	{
		// If we are in a XSL input
		XinxCodeEdit::insertDragAndDropText(text);
	}
	else
	{
		// If we are in a HTML input
		XinxCodeEdit::insertDragAndDropText("{" + text + "}");
	}
}

QDocumentCursor XslTextEditor::insertCompletionBalises(Core::BaliseDefinition::XmlContextType* context, QDocumentCursor& tc, QSharedPointer< Core::BaliseDefinition::BaliseNode > balise)
{
	QString indent = tc.line().text();
	indent = indent.left(indent.indexOf(QRegExp("\\S")));

	QDocumentCursor position = QDocumentCursor();
	int cnt = 0;

	if (SelfWebPluginSettings::self()->config().xml.addDefaultSubBalise)
	{
		bool isNamed = false;
		QString templateName;
		if (context->balise().baliseName() == "call-template")
		{
			templateName = context->balise().attributes().value("name");
			isNamed = true;
		}
		else if (context->balise().baliseName() == "apply-templates")
		{
			templateName = context->balise().attributes().value("match");
		}

		if (! templateName.isEmpty())
		{
			QSharedPointer<Core::Stylesheet::TemplateNode> templateNode = globalNodeOfTemplate(templateName, isNamed);

			if (templateNode)
			{
				foreach(ContentView3::NodePtr child, templateNode->childs())
				{
					QSharedPointer<Core::Stylesheet::ParamNode> paramNode = child.dynamicCast<Core::Stylesheet::ParamNode>();

					if (paramNode && paramNode->value().isEmpty())
					{
						tc.insertText("\n" + indent + "\t");
						tc.insertText("<xsl:with-param name=\"" + paramNode->name() + "\" select=\"\"/>");
						if (position.isNull())
						{
							position = tc;
							position.movePosition(3, QDocumentCursor::Left);
						}
						cnt++;
					}
				}
			}
		}
	}

	if (cnt > 0)
		tc.insertText("\n" + indent);

	if (position.isNull())
	{
		position = XmlTextEditor::insertCompletionBalises(context, tc, balise);
	}
	else
	{
		XmlTextEditor::insertCompletionBalises(context, tc, balise);
		tc.moveTo(position);
	}

	return position;
}
