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

// Xinx header
#include "xmlcontextparser.h"
#include "xmlcontexttype.h"
#include <editors/xinxcodeedit.h>

// QCodeEdit header
#include <qdocumentcursor.h>
#include <editors/textfileeditor.h>
#include <editors/widgeteditor/html/htmlfileeditor.h>
#include <editors/widgeteditor/xsl/xslfileeditor.h>

namespace Core
{

namespace BaliseDefinition
{

QStack<XmlBalise> XmlContextParser::xpath(const QDocumentCursor & cursor, const QStringList & includeOnly, const QString & prefix, const QStringList & attributeName) const
{
	QStack<XmlBalise> xpath;
	xpath.reserve(100); // 100 level is big for an XML
	QStack<QString> baliseClose;

	QDocumentCursor c = cursor;
	do
	{
		c = editor()->textEdit()->find(QRegExp("<[^<>]*>"), c, XinxCodeEdit::FindBackward);
		if (c.isNull()) break;

		QString baliseText = c.selectedText(), baliseEpuree = baliseText;
		baliseText.remove(0, 1).chop(1);
		baliseText = baliseText.trimmed();
		baliseEpuree.remove(0, 2).chop(1);
		baliseEpuree = baliseEpuree.trimmed();
		baliseEpuree = baliseEpuree.section(' ', 0, 0);

		if (!(prefix.isEmpty() || baliseText.startsWith(prefix + ":") || baliseText.startsWith("/" + prefix + ":")))
		{
			// do nothing
		}
		else if (baliseText.startsWith('?') || baliseText.startsWith('!'))
		{
			// do nothing
		}
		else if (baliseText.startsWith('/') && (includeOnly.isEmpty() || includeOnly.contains(baliseEpuree)))
		{
			baliseClose.push(baliseEpuree);
		}
		else if (baliseText.endsWith('/'))
		{
			// do nothing : add and remove from baliseClose
		}
		else
		{
			const XmlBalise xpathBalise = createBalise(baliseText, attributeName);
			const QString xpathBaliseName = xpathBalise.nameSpacePrefix() + (! xpathBalise.nameSpacePrefix().isEmpty() ? ":"  : "" ) + xpathBalise.baliseName();

			if (!(includeOnly.isEmpty() || includeOnly.contains(xpathBalise.baliseName()))) continue; // do nothing

			if (!baliseClose.isEmpty() && (xpathBaliseName == baliseClose.top()))
			{
				baliseClose.pop();
			}
			else
			{
				xpath.push(xpathBalise);
			}
		}
	}
	while (true);

	// Stack is in the wrong sens, we invert it
	QStack<XmlBalise> invertedXPath;
	invertedXPath.reserve(xpath.size());
	while (xpath.size())
	{
		invertedXPath.push(xpath.pop());
	}

	return invertedXPath;
}

XmlBalise XmlContextParser::createBalise(const QString & text, const QStringList & attributeName) const
{
	QString baliseName = text.section(' ', 0, 0).trimmed();
	QHash<QString,QString> attributes;

	XmlBalise xpathBalise;
	QStringList params = text.split(' ', QString::SkipEmptyParts);
	if (params.isEmpty())
	{
		return xpathBalise;
	}
	params.removeFirst();

	foreach(const QString & param, params)
	{
		int indexOfEquals = param.indexOf('=');
		if (indexOfEquals < 0) continue;

		const QString name  = param.left(indexOfEquals).trimmed();
		if (attributeName.isEmpty() || attributeName.contains(name))
		{
			QString value  = param.mid(indexOfEquals + 1).remove('=').trimmed();
			if ((value.startsWith("\"") || value.startsWith("'")) && (value.at(0) == value.at(value.size() -1)))
			{
				value = value.mid(1, value.length() - 2);
			}
			attributes[ name ] = value;
		}
	}

	if (baliseName.startsWith("/"))
	{
		xpathBalise.setIsClosing(true);
		baliseName.remove(0, 1);
	}

	if (baliseName.contains(":"))
	{
		QStringList baliseNameSplitPrefix = baliseName.split(":");
		xpathBalise.setNameSpacePrefix(baliseNameSplitPrefix.at(0));
		xpathBalise.setBaliseName(baliseNameSplitPrefix.at(1));
	}
	else
	{
		xpathBalise.setBaliseName(baliseName);
	}
	xpathBalise.setAttributes(attributes);

	return xpathBalise;
}

QString XmlContextParser::xpathToString(const QList<XmlBalise> & xp)
{
	QString xps;
	foreach(const XmlBalise & balise, xp)
	{
		QString text = balise.baliseName();
		if (! balise.attributes().isEmpty())
		{
			text += "[";
			QHashIterator<QString,QString> i(balise.attributes());
			while (i.hasNext())
			{
				i.next();
				text += "@" + i.key() + "='" + i.value() + "'";
				if (i.hasNext())
					text += " and ";
			}
			text += "]";
		}
		xps.insert(0, "/" + text);
	}
	return xps;
}

XmlBalise XmlContextParser::currentBalise(const QDocumentCursor& cursor)  const
{
	QString nodeName, paramName;
	XmlContextType::Position position = editPosition(cursor, nodeName, paramName);
	if ((position == XmlContextType::COMMENT) || (position == XmlContextType::NONE)) return XmlBalise();

	QDocumentCursor baliseStart = editor()->textEdit()->find("<", cursor, XinxCodeEdit::FindBackward).selectionStart();
	QDocumentCursor baliseStop = editor()->textEdit()->find(QRegExp("[<>]"), cursor).selectionEnd();
	if (baliseStart.isNull() || baliseStop.isNull()) return XmlBalise();

	QDocumentCursor text(editor()->textEdit()->document());
	text.moveTo(baliseStart.selectionStart());
	text.movePosition(1, QDocumentCursor::Right);
	text.movePosition(baliseStop.selectionStart().position() - baliseStart.selectionStart().position() - 2,
					  QDocumentCursor::Right, QDocumentCursor::KeepAnchor);

	return createBalise(text.selectedText());
}

XmlContextType::Position XmlContextParser::editPosition(const QDocumentCursor& cursor, QString& nodeName, QString& paramName) const
{
	Q_ASSERT(! cursor.isNull());

	int cursorPosition = cursor.position();
	nodeName = QString();
	paramName = QString();

	QDocumentCursor cursorCommentTag = editor()->textEdit()->find(QRegExp("(<!--)|(-->)"), cursor, XinxCodeEdit::FindBackward);
	if (cursorCommentTag.isValid() && (cursorCommentTag.selectedText() == "<!--"))
	{
		return XmlContextType::COMMENT;
	}
	/*
		QDocumentCursor cursorLastChar = cursor;
		cursorLastChar.movePosition(1, QDocumentCursor::PreviousCharacter, QDocumentCursor::KeepAnchor);
		if (cursorLastChar.selectedText() == ">")
		{
			QDocumentCursor cursorFirstCar = editor()->textEdit()->find(QRegExp("<(?!!--)"), cursor, XinxCodeEdit::FindBackward);
			if (! cursorFirstCar.isNull())
			{
				cursorFirstCar.movePosition(1, QDocumentCursor::NextCharacter, QDocumentCursor::MoveAnchor);
				cursorFirstCar.movePosition(1, QDocumentCursor::NextCharacter, QDocumentCursor::KeepAnchor);
				if (cursorFirstCar.selectedText() != "/")
					return XmlContextType::CLOSING_NODE;
			}
		}

		cursorLastChar.movePosition(1, QDocumentCursor::PreviousCharacter, QDocumentCursor::KeepAnchor);
		if (cursorLastChar.selectedText() == "</")
		{
			return XmlContextType::CLOSING_BALISE;
		}
	*/
	QDocumentCursor cursorNodeTag = editor()->textEdit()->find(QRegExp("(<(?!!--))|>"), cursor, XinxCodeEdit::FindBackward);
	if (cursorNodeTag.isNull() || (cursorNodeTag.selectedText() == ">"))
	{
		return XmlContextType::NONE;
	}

	QDocumentCursor tc(editor()->textEdit()->document());
	tc.moveTo(cursorNodeTag.selectionEnd());
	tc.movePosition(cursorPosition - cursorNodeTag.position() - 1, QDocumentCursor::Right, QDocumentCursor::KeepAnchor);

	QString baliseContentStr =  tc.selectedText();
	QStringList baliseContent;// = baliseContentStr.split( QRegExp( "[0-9\\w:\\-]+" ), QString::SkipEmptyParts );

	// Split baliseContentStr into baliseContent by space. But space is keeped in quote.
	for (int i = 0; i < baliseContentStr.length(); i++)
	{
		QString text;
		bool quoted = false;
		while ((i < baliseContentStr.length()) && baliseContentStr.at(i).isSpace()) i++;
		while ((i < baliseContentStr.length()) && ((! baliseContentStr.at(i).isSpace()) || quoted))
		{
			text += baliseContentStr.at(i);
			if (baliseContentStr.at(i) == '"')
				quoted = ! quoted;
			i++;
		}
		if (! text.isEmpty())
			baliseContent.append(text);
	}

	if (baliseContent.size() == 0)
	{
		return XmlContextType::BALISE_NAME;
	}

	bool hasTextBeforeCursor = ! baliseContentStr.right(1).trimmed().isEmpty();
	nodeName = baliseContent.first();

	if (((baliseContent.size() == 1) && hasTextBeforeCursor) || nodeName.startsWith("/"))
	{
		return XmlContextType::BALISE_NAME;
	}

	paramName = baliseContent.last();

	int equalsIndex = paramName.indexOf("=");
	if (equalsIndex == -1)
	{
		return XmlContextType::ATTRIBUTE_NAME;
	}

	if (paramName.count("\"") == 2)
	{
		paramName = QString();
		return XmlContextType::ATTRIBUTE_NAME;
	}

	paramName = paramName.left(equalsIndex);

	return XmlContextType::ATTRIBUTE_CONTENT;
}

XmlContextParser::XmlContextParser(): ContextParser()
{

}

XmlContextParser::~XmlContextParser()
{

}

void XmlContextParser::updateContext()
{
	if (!editor()->fileTypePluginInterface()) return;
	if (editor()->fileTypePluginInterface()->name() != "XmlFileEditor"
			&&	editor()->fileTypePluginInterface()->name() != "StyleSheetEditor"
			&&	editor()->fileTypePluginInterface()->name() != "HtmlFileEditor") return;

	XmlContextType * type = new XmlContextType;
	QString baliseName, attributeName;
	XmlContextType::Position position = editPosition(editor()->textEdit()->textCursor(), baliseName, attributeName);

	type->setPosition(position);

	QStack<XmlBalise> path = xpath(editor()->textEdit()->textCursor());

	if (position >= XmlContextType::BALISE_NAME)
	{
		XmlBalise balise = currentBalise(editor()->textEdit()->textCursor());
		if ((! balise.baliseName().isEmpty()) || (! balise.nameSpacePrefix().isEmpty()))
		{
			path.push(balise);

			type->setBalise(balise);
			type->setAttributeName(attributeName);
		}
	}
	/*
	if (position == XmlContextType::CLOSING_BALISE)
	{
		type->setBalise(path.last());
	}
	*/

	type->setXpathLocation(path);

	QHash<QString,QString> fileNamespace;
	while (path.size())
	{
		QHash<QString,QString> baliseAttribute = path.pop().attributes();
		QHashIterator<QString,QString> i(baliseAttribute);
		while (i.hasNext())
		{
			i.next();
			if (i.key().startsWith("xmlns:"))
			{
				QString key   = i.key();
				QString value = i.value();

				key.remove(0, 6);

				if (! fileNamespace.contains(key))
				{
					fileNamespace.insert(key, value);
				}
			}
		}
	}
	type->setXmlnsList(fileNamespace);

	if (editor()->fileTypePluginInterface()->name() == "HtmlFileEditor")
	{
		type->setIsHtml(true);
		type->setIsXsl(false);
	}
	else if (editor()->fileTypePluginInterface()->name() == "StyleSheetEditor")
	{
		type->setIsHtml(true);
		type->setIsXsl(true);
	}
	else
	{
		type->setIsHtml(false);
		type->setIsXsl(false);
	}

	context().addContext(type);
}

}

}



