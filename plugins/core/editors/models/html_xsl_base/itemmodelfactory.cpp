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
#include <editors/models/html_xsl_base/xmlcompletionparser.h>
#include <project/xinxprojectmanager.h>
#include <project/xinxprojectproject.h>
#include <codecompletion/model.h>
#include "definition_rootnode.h"
#include "definition_balisenode.h"
#include "definition_attributenode.h"
#include "definition_valuenode.h"
#include "config/selfwebpluginsettings.h"
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>
#include <editors/widgeteditor/xml/xmltexteditor.h>
#include "xmldefinitionmanager.h"

namespace Core
{

namespace BaliseDefinition
{

/* BaliseNodeItem */

BaliseNodeItem::BaliseNodeItem(const ContentView3::NodePtr & node) : CodeCompletion::ContentViewNodeItem(node)
{

}

BaliseNodeItem::~BaliseNodeItem()
{

}

void BaliseNodeItem::execute(CodeCompletion::Context context, TextFileEditor* editor)
{
	XmlContextType * c = dynamic_cast<XmlContextType*>(context.context(XML_CONTEXT_TYPE));

	QSharedPointer<BaliseNode> baliseNode = node().dynamicCast<BaliseNode>();
	if (baliseNode)
	{
		editor->textEdit()->insertText(completionText());
		QDocumentCursor tc = editor->textEdit()->textCursor();
		tc = qobject_cast<XmlTextEditor*>(editor->textEdit())->insertCompletionParam(c, tc, baliseNode, true);
		if (tc.isValid())
		{
			editor->textEdit()->setTextCursor(tc);
		}
	}
}

/* AttributeNodeItem */

AttributeNodeItem::AttributeNodeItem(const ContentView3::NodePtr & node) : CodeCompletion::ContentViewNodeItem(node)
{

}

AttributeNodeItem::~AttributeNodeItem()
{

}

void AttributeNodeItem::execute(CodeCompletion::Context context, TextFileEditor *editor)
{
	XmlContextType * c = dynamic_cast<XmlContextType*>(context.context(XML_CONTEXT_TYPE));

	QSharedPointer<AttributeNode> attributeNode = node().dynamicCast<AttributeNode>();
	if (attributeNode)
	{
		editor->textEdit()->insertText(completionText());
		QDocumentCursor tc = editor->textEdit()->textCursor();
		tc = qobject_cast<XmlTextEditor*>(editor->textEdit())->insertCompletionValue(c, tc, attributeNode);
		if (tc.isValid())
		{
			editor->textEdit()->setTextCursor(tc);
		}
	}
}

/* ItemModelFactory */

ItemModelFactory::ItemModelFactory() : _xsl_context(tr("XML Style Sheet Nodes")), _html_context(tr("HTML Nodes")), _ns_context(tr("XML Namespace"))
{

}

ItemModelFactory::~ItemModelFactory()
{

}

void ItemModelFactory::addBalises(const QString & ctxt, QSharedPointer<Core::BaliseDefinition::RootNode> node)
{
	QList< QSharedPointer<BaliseNode> > balises = node->balises();

	foreach(QSharedPointer<BaliseNode> balise, balises)
	{
		if (balise->name().startsWith(context().prefix(), Qt::CaseInsensitive))
		{
			BaliseNodeItem * item = new BaliseNodeItem(balise);
			item->setContextType(ctxt);
			itemInterface()->addItem(item);
		}
	}
}

void ItemModelFactory::addAttributes(XmlContextType * c, const QString & ctxt, QSharedPointer<Core::BaliseDefinition::RootNode> root)
{
	const QString name = c->balise().baliseName();
	const QStringList attributes = c->balise().attributes().keys();

	QSharedPointer<Core::BaliseDefinition::BaliseNode> balise = root->balise(name);
	if (balise)
	{
		QList< QSharedPointer<AttributeNode> > attributesNode = balise->attributes();
		foreach(QSharedPointer<AttributeNode> attributeNode, attributesNode)
		{
			if (!attributes.contains(attributeNode->name()) && attributeNode->name().startsWith(context().prefix(), Qt::CaseInsensitive))
			{
				AttributeNodeItem * item = new AttributeNodeItem(attributeNode);
				item->setContextType(ctxt);
				itemInterface()->addItem(item);
			}
		}
	}
}

void ItemModelFactory::addValues(XmlContextType * c, const QString & ctxt, QSharedPointer<Core::BaliseDefinition::RootNode> root)
{
	const QString name = c->balise().baliseName();
	const QString attribute = c->attributeName();

	QSharedPointer<Core::BaliseDefinition::BaliseNode> balise = root->balise(name);
	if (balise)
	{
		QSharedPointer<Core::BaliseDefinition::AttributeNode> attributeNode = balise->attribute(attribute);

		if (attributeNode)
		{
			QList< QSharedPointer<ValueNode> > valuesNode = attributeNode->values();
			foreach(QSharedPointer<ValueNode> valueNode, valuesNode)
			{
				if (valueNode->name().startsWith(context().prefix(), Qt::CaseInsensitive))
				{
					CodeCompletion::ContentViewNodeItem * item = new CodeCompletion::ContentViewNodeItem(valueNode);
					item->setContextType(ctxt);
					itemInterface()->addItem(item);
				}
			}
		}
	}
}

void ItemModelFactory::addCompletion(XmlContextType * c, const QString & context, QSharedPointer<Core::BaliseDefinition::RootNode> root)
{
	if (c->position() == XmlContextType::BALISE_NAME)
	{
		addBalises(context, root);
	}
	else if (c->position() == XmlContextType::ATTRIBUTE_NAME)
	{
		addAttributes(c, context, root);
	}
	else if (c->position() == XmlContextType::ATTRIBUTE_CONTENT)
	{
		addValues(c, context, root);
	}
}


void ItemModelFactory::generate()
{
	XmlContextType * c = dynamic_cast<XmlContextType*>(context().context(XML_CONTEXT_TYPE));
	if (c)
	{
		// if definition balise et context non déjà écris
		if ((c->position() == XmlContextType::BALISE_NAME) && c->balise().nameSpacePrefix().isEmpty())
		{
			QHashIterator<QString,QString> i(c->xmlnsList());
			while (i.hasNext())
			{
				i.next();
				const QString key = i.key();
				const QString value = i.value();

				if (key.startsWith(context().prefix(), Qt::CaseInsensitive))
				{
					CodeCompletion::Item * item = new CodeCompletion::Item;
					item->setText(QString("%1 - %2").arg(key, value));
					item->setCompletionText(key);
					item->setCompletionType(tr("Namespace"));
					item->setKeyString(key);
					item->setIcon(QIcon(":/images/template.png"));
					item->setContextType(_ns_context);
					itemInterface()->addItem(item);
				}
			}
		}

		if (c->isHtml() && (c->balise().nameSpacePrefix().isEmpty()))
		{
			addCompletion(c, _html_context, XmlDefinitionManager::self()->getHtmlRootNode());
		}

		if (c->isXsl() && (c->xmlnsList().value(c->balise().nameSpacePrefix()) == "http://www.w3.org/1999/XSL/Transform"))
		{
			addCompletion(c, _xsl_context, XmlDefinitionManager::self()->getXslRootNode());
		}
	}
}

}

}
