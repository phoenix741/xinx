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
#include "xmltexteditor.h"
#include "config/selfwebpluginsettings.h"
#include "editors/models/xsl/stylesheet_parser.h"

#include <plugins/xinxpluginsloader.h>
#include <core/xinxconfig.h>
#include <editors/xinxlanguagefactory.h>
#include <editors/models/html_xsl_base/definition_attributenode.h>
#include <editors/models/html_xsl_base/definition_valuenode.h>
#include <editors/models/html_xsl_base/xmlcontexttype.h>
#include <editors/models/html_xsl_base/xmldefinitionmanager.h>

#include <codecompletion/completer.h>

// QCodeEdit header
#include <qlanguagedefinition.h>
#include <qdocumentcursor.h>
#include <qdocumentline.h>

// Qt header
#include <QKeyEvent>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextCursor>
#include <QStack>
#include <QModelIndex>
#include <QDebug>

// Define
#define EOWREGEXP   "[~!@\\$#%\\^&\\*\\(\\)\\+\\{\\}|\"<>,/;'\\[\\]\\\\=\\s]"
#define isEditBalise(value) ((value == cpEditNodeName) || (value == cpEditParamName) || (value == cpEditParamValue))

/* XmlTextEditor */

XmlTextEditor::XmlTextEditor(QWidget * parent) : XinxCodeEdit(parent)
{
	setHighlighter("XML");
}

XmlTextEditor::~XmlTextEditor()
{

}

QDocumentCursor XmlTextEditor::insertCompletionParam(Core::BaliseDefinition::XmlContextType * context, QDocumentCursor & tc, QSharedPointer<Core::BaliseDefinition::BaliseNode> balise, bool movePosition)
{
	QDocumentCursor result = QDocumentCursor();

	if (SelfWebPluginSettings::self()->config().xml.addDefaultAttribute && balise)
	{
		QStringList paramList;
		if (context)
		{
			paramList = context->balise().attributes().keys();
		}

		foreach(QSharedPointer<Core::BaliseDefinition::AttributeNode> attr, balise->defaultAttributes())
		{
			if (! paramList.contains(attr->name()))
			{
				QString defaultValueString;
				QList< QSharedPointer<Core::BaliseDefinition::ValueNode> > defaultValues = attr->defaultValues();

				if (defaultValues.size())
				{
					defaultValueString = defaultValues.at(0)->name();
				}

				tc.insertText(QString(" %1=\"%2\"").arg(attr->name()).arg(defaultValueString));

				if (result.isNull() && defaultValueString.isEmpty())
				{
					result = tc;
					result.movePosition(1, QDocumentCursor::PreviousCharacter);
				}
			}
		}
	}
	if (result.isValid() && movePosition)
		tc.moveTo(result);

	return result;
}


QDocumentCursor XmlTextEditor::insertCompletionValue(Core::BaliseDefinition::XmlContextType * context, QDocumentCursor & tc, QSharedPointer<Core::BaliseDefinition::AttributeNode> attribute)
{
	if (attribute && SelfWebPluginSettings::self()->config().xml.addDefaultAttribute)
	{
		QList< QSharedPointer<Core::BaliseDefinition::ValueNode> > defaultValues = attribute->defaultValues();
		if (defaultValues.size())
		{
			tc.insertText(QString("=\"%1\"").arg(defaultValues.at(0)->name()));
			return tc;
		}
	}
	tc.insertText("=\"\"");
	tc.movePosition(1, QDocumentCursor::PreviousCharacter);

	return tc;
}

QDocumentCursor XmlTextEditor::insertCompletionBalises(Core::BaliseDefinition::XmlContextType * context, QDocumentCursor & tc, QSharedPointer<Core::BaliseDefinition::BaliseNode> balise)
{
	QString indent = tc.line().text();
	indent = indent.left(indent.indexOf(QRegExp("\\S")));

	QDocumentCursor result = QDocumentCursor();
	int cnt = 0;

	QString ns = context->balise().nameSpacePrefix();
	if (!ns.isEmpty())
	{
		ns += ":";
	}

	if (SelfWebPluginSettings::self()->config().xml.addDefaultSubBalise && balise)
	{
		foreach(QSharedPointer<Core::BaliseDefinition::BaliseNode> b, balise->defaultBalises())
		{

			tc.insertText("\n" + indent + "\t");
			tc.insertText("<" + ns + b->name());
			QDocumentCursor insertPosition = insertCompletionParam(context, tc, b, false);
			if (result.isNull()) result = insertPosition;
			tc.insertText(">");
			insertCompletionBalises(context, tc, b);
			cnt++;
		}
	}

	if (SelfWebPluginSettings::self()->config().xml.addClosedBalise)
	{
		if (cnt > 0)
			tc.insertText("\n" + indent);
		tc.insertText(QString("</%1>").arg(ns + balise->name()));
		if (result.isValid())
			tc.moveTo(result);
	}

	return result;
}

bool XmlTextEditor::isCommentAvailable()
{
	return true;
}

void XmlTextEditor::commentSelectedText(bool uncomment)
{
	QString nodeName, paramName;

	QDocumentCursor cursor(textCursor());

	QDocumentCursor cursorStart(document());
	cursorStart.moveTo(cursor.selectionStart());
	bool isStartCommented = false; // FIXME: editPosition(cursorStart, nodeName, paramName) == cpEditComment;

	QDocumentCursor cursorEnd(textCursor());
	cursorEnd.moveTo(cursor.selectionEnd());
	bool isEndCommented =  false; // FIXME: editPosition(cursorEnd, nodeName, paramName) == cpEditComment;

	QString text = cursor.selectedText();
	text = text.replace("<!--", "");
	text = text.replace("-->", "");

	cursor.beginEditBlock();

	cursor.removeSelectedText();
	if (!(isStartCommented ^ uncomment))
	{
		// Comment
		if (! uncomment)
			cursor.insertText("<!--");
		else
			cursor.insertText("-->");
	}
	cursor.insertText(text);
	if (!(isEndCommented ^ uncomment))
	{
		// End the comment
		if (! uncomment)
			cursor.insertText("-->");
		else
			cursor.insertText("<!--");
	}

	cursor.endEditBlock();
}

bool XmlTextEditor::localKeyPressExecute(QKeyEvent * e)
{
	if ((e->key() == Qt::Key_Return) && ((e->modifiers() == Qt::ControlModifier) || (e->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier))))
	{
		key_shenter(e->modifiers() & Qt::ShiftModifier);
		e->ignore();
		return false;
	}
	else
		return XinxCodeEdit::localKeyPressExecute(e);
}

void XmlTextEditor::key_shenter(bool back)
{
	XinxCodeEdit::FindFlags flags = 0;
	if (back) flags |= XinxCodeEdit::FindBackward;

	QDocumentCursor cursor = textCursor();
	if (back)
	{
		cursor.moveTo(cursor.selectionStart());
		cursor.movePosition(2, QDocumentCursor::Left, QDocumentCursor::MoveAnchor);
	}
	cursor = find(QRegExp("=\"[^\"]*\""), cursor, flags);

	if (! cursor.isNull())
	{
		QDocumentCursor newCursor = textCursor();
		newCursor.moveTo(cursor.selectionStart());
		newCursor.movePosition(2, QDocumentCursor::Right);
		newCursor.movePosition(cursor.selectionEnd().position() - cursor.selectionStart().position() - 3, QDocumentCursor::Right, QDocumentCursor::KeepAnchor);
		setTextCursor(newCursor);
	}
}

bool XmlTextEditor::processKeyPress(QKeyEvent * e)
{
	Q_ASSERT_X(completer(), "XmlTextEditor::processKeyPress", "No completer defined, it's an error");
	CodeCompletion::Context context = completer()->context();
	Core::BaliseDefinition::XmlContextType * context_type = dynamic_cast<Core::BaliseDefinition::XmlContextType*>(context.context(XML_CONTEXT_TYPE));
	if (! context_type)
	{
		qWarning() << tr("No XML context type found. It's a problem");
		return false;
	}

	if (! SelfWebPluginSettings::self()->config().xml.activeCompletion) return false;

	if (!e->text().isEmpty())
	{
		if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
		{
			QDocumentCursor tc(textCursor());
			bool hasOpenBalise = false, hasCloseBalise = false;
			QDocumentLine line = tc.line(), previousLine = line.previous();

			foreach(QParenthesis p, line.parentheses())
			{
				if (p.role & QParenthesis::Close)
					hasCloseBalise = true;
			}
			foreach(QParenthesis p, previousLine.parentheses())
			{
				if (p.role & QParenthesis::Open)
					hasOpenBalise = true;
			}

			if (hasOpenBalise && hasCloseBalise)
				tc.deletePreviousChar();

			return true;
		}
		else if ((e->text().right(2) == ">") && (SelfWebPluginSettings::self()->config().xml.addClosedBalise))
		{
			if ((context_type->position() == Core::BaliseDefinition::XmlContextType::BALISE_NAME) ||(context_type->position() == Core::BaliseDefinition::XmlContextType::ATTRIBUTE_NAME))
			{
				QDocumentCursor tc(textCursor());
				QDocumentCursor tc2(textCursor());
				tc2.movePosition(2, QDocumentCursor::PreviousCharacter, QDocumentCursor::KeepAnchor);
				if (tc2.selectedText() != "/>")
				{
					QSharedPointer<Core::BaliseDefinition::BaliseNode> balise;
					if (context_type->isXsl() && (context_type->xmlnsList().value(context_type->balise().nameSpacePrefix()) == "http://www.w3.org/1999/XSL/Transform"))
					{
						balise = Core::BaliseDefinition::XmlDefinitionManager::self()->getXslRootNode()->balise(context_type->balise().baliseName());
					}
					else if (balise.isNull() && context_type->isHtml())
					{
						balise = Core::BaliseDefinition::XmlDefinitionManager::self()->getHtmlRootNode()->balise(context_type->balise().baliseName());
					}

					if (! balise.isNull())
					{
						QDocumentCursor position = textCursor();
						position.setAutoUpdated(false);
						QDocumentCursor newPosition = insertCompletionBalises(context_type, tc, balise);
						if (newPosition.isNull())
							tc.moveTo(position);
						setTextCursor(tc);
					}
				}
				return true;
			}
		}
		else if (e->text().right(1) == "=" && SelfWebPluginSettings::self()->config().xml.addDefaultAttribute)
		{
			if (context_type->position() == Core::BaliseDefinition::XmlContextType::ATTRIBUTE_NAME)
			{
				QDocumentCursor tc(textCursor());
				QDocumentCursor tc2(textCursor());
				tc2.movePosition(1, QDocumentCursor::PreviousCharacter);

				QSharedPointer<Core::BaliseDefinition::BaliseNode> balise;
				if (context_type->isXsl() && (context_type->xmlnsList().value(context_type->balise().nameSpacePrefix()) == "http://www.w3.org/1999/XSL/Transform"))
				{
					balise = Core::BaliseDefinition::XmlDefinitionManager::self()->getXslRootNode()->balise(context_type->balise().baliseName());
				}
				else if (balise.isNull() && context_type->isHtml())
				{
					balise = Core::BaliseDefinition::XmlDefinitionManager::self()->getHtmlRootNode()->balise(context_type->balise().baliseName());
				}

				if (balise)
				{
					QSharedPointer<Core::BaliseDefinition::AttributeNode> attribute = balise->attribute(context_type->attributeName());
					if (attribute)
					{
						tc.deletePreviousChar();
						tc = insertCompletionValue(context_type, tc, attribute);
						setTextCursor(tc);
					}
				}
				return true;
			}
		}
		else if (e->text().right(1) == " " && SelfWebPluginSettings::self()->config().xml.addDefaultAttribute)
		{
			QDocumentCursor tc(textCursor());
			QDocumentCursor tc2(textCursor());
			tc2.movePosition(1, QDocumentCursor::PreviousCharacter);
			if (context_type->position() == Core::BaliseDefinition::XmlContextType::BALISE_NAME)
			{
				QSharedPointer<Core::BaliseDefinition::BaliseNode> balise;
				if (context_type->isXsl() && (context_type->xmlnsList().value(context_type->balise().nameSpacePrefix()) == "http://www.w3.org/1999/XSL/Transform"))
				{
					balise = Core::BaliseDefinition::XmlDefinitionManager::self()->getXslRootNode()->balise(context_type->balise().baliseName());
				}
				else if (balise.isNull() && context_type->isHtml())
				{
					balise = Core::BaliseDefinition::XmlDefinitionManager::self()->getHtmlRootNode()->balise(context_type->balise().baliseName());
				}

				if (! balise.isNull())
				{
					tc.deletePreviousChar();
					if (insertCompletionParam(context_type, tc, balise, true).isNull())
						tc.insertText(" ");
					setTextCursor(tc);
				}
				return true;
			}
		}
		else if (e->text().right(1) == "/")
		{
			QDocumentCursor tc(textCursor());
			tc.movePosition(2, QDocumentCursor::PreviousCharacter, QDocumentCursor::KeepAnchor);

			QString selected = tc.selectedText();
			if (selected == "</")
			{
				QDocumentCursor balise = find(QRegExp("[<>]"), textCursor());
				if (balise.isNull() || (balise.selectedText() == "<"))
				{
					Core::BaliseDefinition::XmlBalise balise = context_type->xpathLocation().top();

					QDocumentCursor c(textCursor());
					c.insertText((balise.nameSpacePrefix().isEmpty() ? "" : balise.nameSpacePrefix() + ":") + balise.baliseName() + ">");
					setTextCursor(c);
				}
			}
		}
	}
	return false;
}
