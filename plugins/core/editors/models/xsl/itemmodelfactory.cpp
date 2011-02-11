/*
 XINX
 Copyright (C) 2007-2010 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "itemmodelfactory.h"
#include <editors/models/html_xsl_base/xmlcontexttype.h>
#include <codecompletion/model.h>
#include <contentview3/definitions.h>
#include <contentview3/file.h>
#include <contentview3/filenode.h>
#include <contentview3/cache.h>
#include <project/xinxprojectproject.h>
#include "stylesheet_templatenode.h"
#include "stylesheet_paramnode.h"
#include "stylesheet_variablenode.h"
#include "editors/models/html_xsl_base/xmlcontexttype.h"
#include <qdocumentcursor.h>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>

namespace Core
{

namespace Stylesheet
{

/* AttributeNodeItem */

XslNodeItem::XslNodeItem(const ContentView3::NodePtr & node) : CodeCompletion::ContentViewNodeItem(node)
{

}

XslNodeItem::~XslNodeItem()
{

}

void XslNodeItem::insertCompletionStart(CodeCompletion::Context context, TextFileEditor* editor, QDocumentCursor & cursor)
{
	Core::BaliseDefinition::XmlContextType * c = dynamic_cast<Core::BaliseDefinition::XmlContextType*> (context.context(XML_CONTEXT_TYPE));

	if (c->xmlnsList ().value (c->balise().nameSpacePrefix()) != "http://www.w3.org/1999/XSL/Transform" )
	{
		QDocumentCursor tc = cursor;
		tc.movePosition (1, QDocumentCursor::PreviousCharacter, QDocumentCursor::KeepAnchor);

		if (tc.selectedText () != "{")
		{
			cursor.insertText("{");
		}
	}
}

void XslNodeItem::insertCompletionEnd(CodeCompletion::Context context, TextFileEditor* editor, QDocumentCursor & cursor)
{
	Core::BaliseDefinition::XmlContextType * c = dynamic_cast<Core::BaliseDefinition::XmlContextType*> (context.context(XML_CONTEXT_TYPE));

	if (c->xmlnsList ().value (c->balise().nameSpacePrefix()) != "http://www.w3.org/1999/XSL/Transform" )
	{
		QDocumentCursor tc = cursor;
		tc.movePosition (1, QDocumentCursor::NextCharacter, QDocumentCursor::KeepAnchor);

		if (tc.selectedText () != "}")
		{
			cursor.insertText("}");
		}
		else
		{
			cursor.movePosition (1, QDocumentCursor::NextCharacter);
		}
	}
}

/* XslTemplateItem */

XslTemplateItem::XslTemplateItem(const ContentView3::NodePtr& node): XslNodeItem(node)
{

}

XslTemplateItem::~XslTemplateItem()
{

}

void XslTemplateItem::execute(CodeCompletion::Context context, TextFileEditor* editor)
{
    CodeCompletion::Item::execute(context, editor);

	QSharedPointer<TemplateNode> templateNode = node().staticCast<TemplateNode>();
	if (! templateNode->mode().isEmpty())
	{
		Core::BaliseDefinition::XmlContextType * c = dynamic_cast<Core::BaliseDefinition::XmlContextType*> (context.context(XML_CONTEXT_TYPE));
		if (!c->balise().attributes().contains("mode"))
		{
			// Go after quote
			QDocumentCursor tc = editor->textEdit()->find("\"", editor->textEdit()->textCursor());
			tc.moveTo(tc.selectionEnd());

			// Add mode
			tc.insertText(" mode=\"" + templateNode->mode() + "\"");
			tc.insertText("/>");

			// Move cursor
			editor->textEdit()->setTextCursor(tc);
		}
	}
}

/* ItemModelFactory */

ItemModelFactory::ItemModelFactory()
{

}

ItemModelFactory::~ItemModelFactory()
{

}

void ItemModelFactory::addApplyTemplatesMode(Core::BaliseDefinition::XmlContextType * c, const QString& contextType, ContentView3::NodePtr node)
{
	QString selectAttr = c->balise ().attributes ().value ("select");
	if (selectAttr.contains ("/"))
	{
		selectAttr = selectAttr.section ("/", -1);
	}

	QSharedPointer<TemplateNode> templateNode;
	if (! (templateNode = node.dynamicCast<TemplateNode>()).isNull () && !templateNode->isNamedTemplate () && (templateNode->name () == selectAttr) && !templateNode->mode ().isEmpty ())
	{
		if (! templateNode->mode ().startsWith(context().prefix (), Qt::CaseInsensitive)) return;

		CodeCompletion::Item * item = new CodeCompletion::Item;
		item->setIcon (QIcon(":/images/html_value.png"));
		item->setText (templateNode->mode ());
		item->setCompletionText (templateNode->mode ());
		item->setCompletionType (tr("Template Mode"));
		item->setKeyString (templateNode->mode ());
		item->setContextType (contextType);

		itemInterface ()->addItem (item);
	}
}

void ItemModelFactory::addApplyTemplatesSelect(Core::BaliseDefinition::XmlContextType * c, const QString& contextType, ContentView3::NodePtr node)
{
	if (! node->name().startsWith(context().prefix (), Qt::CaseInsensitive)) return;

	QSharedPointer<TemplateNode> templateNode;
	if (! (templateNode = node.dynamicCast<TemplateNode>()).isNull () && !templateNode->isNamedTemplate ())
	{
		XslTemplateItem * item = new XslTemplateItem(node);
		item->setContextType(contextType);
		itemInterface()->addItem(item);
	}
}

void ItemModelFactory::addCallTemplate(Core::BaliseDefinition::XmlContextType * c, const QString& contextType, ContentView3::NodePtr node)
{
	if (! node->name().startsWith(context().prefix (), Qt::CaseInsensitive)) return;

	QSharedPointer<TemplateNode> templateNode;
	if (! (templateNode = node.dynamicCast<TemplateNode>()).isNull () && templateNode->isNamedTemplate ())
	{
		XslNodeItem * item = new XslNodeItem(node);
		item->setContextType(contextType);
		itemInterface()->addItem(item);
	}
}

void ItemModelFactory::addOther(Core::BaliseDefinition::XmlContextType * c, const QString& contextType, ContentView3::NodePtr node, bool local, Core::BaliseDefinition::XmlBalise balise)
{
	if (node.dynamicCast<ParamNode>() || node.dynamicCast<VariableNode>())
	{
		if (! node->name().startsWith(context().prefix (), Qt::CaseInsensitive)) return;

		XslNodeItem * item = new XslNodeItem(node);
		item->setContextType(contextType);
		itemInterface()->addItem(item);
	}
	else if (node.dynamicCast<TemplateNode>() && local)
	{
		QSharedPointer<TemplateNode> templateNode = node.dynamicCast<TemplateNode>();
		if ((templateNode->isNamedTemplate() && balise.attributes().value("name") == templateNode->name()) || (!templateNode->isNamedTemplate() && balise.attributes().value("match").contains(templateNode->name())))
		{
			addNode(tr("Template %1").arg(templateNode->name()), node, QString(), !local);
		}
	}
}

void ItemModelFactory::addWithParamName(Core::BaliseDefinition::XmlContextType* c, QString contextType, ContentView3::NodePtr node)
{
	QStack<Core::BaliseDefinition::XmlBalise> xPath = c->xpathLocation();
	xPath.pop();
	Core::BaliseDefinition::XmlBalise balise = xPath.top();

	QString templateName;

	if (balise.baliseName() == "call-template")
	{
		templateName = balise.attributes().value("name");
	}
	else if (balise.baliseName() == "apply-templates")
	{
		templateName = balise.attributes().value("value");
	}

	if (templateName.isEmpty() || (templateName != node->name())) return;

	foreach(ContentView3::NodePtr child, node->childs())
	{
		if (! child->name().startsWith(context ().prefix (), Qt::CaseInsensitive)) continue;

		if (child.dynamicCast<ParamNode>())
		{
			CodeCompletion::ContentViewNodeItem * item = new CodeCompletion::ContentViewNodeItem(child);
			item->setContextType(contextType);
			item->setCompletionHelper(QString());
			itemInterface()->addItem(item);
		}
	}
}

void ItemModelFactory::addNode(const QString & contextType, ContentView3::NodePtr node, QString path, bool import)
{
	Core::BaliseDefinition::XmlContextType * c = dynamic_cast<Core::BaliseDefinition::XmlContextType*> (context().context(XML_CONTEXT_TYPE));

	Core::BaliseDefinition::XmlBalise balise;
	if (!import)
	{
		QStack<Core::BaliseDefinition::XmlBalise> xPath = c->xpathLocation();
		while (xPath.size())
		{
			balise = xPath.pop();
			if (balise.baliseName() == "template")
			{
				break;
			}
		}
	}

	foreach(ContentView3::NodePtr child, node->childs())
	{
		if ((c->balise ().baliseName () == "call-template") && (c->attributeName () == "name")) // FIXME: test namespace
		{
			addCallTemplate(c, contextType, child);
		}
		else if ((c->balise ().baliseName () == "apply-templates") && (c->attributeName () == "select"))
		{
			addApplyTemplatesSelect(c, contextType, child);
		}
		else if ((c->balise ().baliseName () == "apply-templates") && (c->attributeName () == "mode"))
		{
			addApplyTemplatesMode(c, contextType, child);
		}
		else if ((c->balise ().baliseName () == "with-param") && (c->attributeName () == "name"))
		{
			addWithParamName(c, contextType, child);
		}
		else
		{
			addOther(c, contextType, child, !import, balise);
		}
	}
}

void ItemModelFactory::generate()
{
	Core::BaliseDefinition::XmlContextType * c = dynamic_cast<Core::BaliseDefinition::XmlContextType*> (context().context(XML_CONTEXT_TYPE));
	if (c && c->position () == Core::BaliseDefinition::XmlContextType::ATTRIBUTE_CONTENT)
	{
		ContentView3::FilePtr file = context().fileStrongRef ();
		XinxProject::ProjectPtr project = file->project();

		ContentView3::FileNodePtr rootNode = file->rootNode();
		addNode (tr("Local"), rootNode, QString(), false);

		if (project)
		{
			foreach(const ContentView3::FilePtr & import, project->cache ()->importOf(file))
			{
				ContentView3::FileNodePtr rootNode = import->rootNode();
				addNode (QDir(project->projectPath ()).relativeFilePath (import->filename()), rootNode, QString(), true);
			}
		}
	}
}

}

}
