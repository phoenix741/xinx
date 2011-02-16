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
#include "cssmodeldata.h"
#include "css_propertynode.h"
#include "css_identifiernode.h"
#include "css_classnode.h"
#include "css_idnode.h"
#include "css_tagnode.h"

// Qt header
#include <QFileInfo>
#include <QTextStream>
#include <QRegExp>
#include <QImage>
#include <QVariant>

namespace Core
{

namespace CascadingStyleSheet
{

/* Parser */

Parser::Parser() : ContentView3::Parser()
{

}

Parser::~Parser()
{

}

QString Parser::name() const
{
	return "CSS";
}

void Parser::parse()
{
	ContentView3::NodePtr node = rootNode();
	node->clearChilds();
	node->setIcon(":/images/typecss.png");

	QString text = device()->readAll();

	if (text.isEmpty())
	{
		return;
	}

	QRegExp commentStartExpression("^/\\*");
	QRegExp commentEndExpression("\\*/");
	QRegExp keywordExpression("[A-Za-z_][A-Za-z0-9_-:.]*");
	QRegExp indentifierExpression("[^\n]*;");

	QList<ContentView3::NodePtr> elements;

	int pos;
	ParsingState state = CssDefault;

	m_line = 1;

	for (int i = 0; i < text.length(); i++)
	{
		char c = text.at(i).toLower().toAscii();
		if (c == '/')
		{
			pos = commentStartExpression.indexIn(text, i, QRegExp::CaretAtOffset);

			if (pos == i)
			{
				const int posEnd = commentEndExpression.indexIn(text, pos + 2, QRegExp::CaretAtOffset);
				if (posEnd == -1)
					break;

				m_line += text.mid(pos, posEnd - pos).count(QRegExp("\\n"));

				i = posEnd;
			}
		}
		else if ((c >= 'a') && (c <= 'z'))
		{
			if (state == CssDefault)
			{
				// TAG
				pos = keywordExpression.indexIn(text, i, QRegExp::CaretAtOffset);

				if (pos == i)
				{
					const int iLength = keywordExpression.matchedLength();
					elements.append(attacheNewTagNode(rootNode(), keywordExpression.cap(), m_line));
					i += iLength - 1;
				}
			}
			else if (state == CssIdentifier)
			{
				// Identifier
				pos = indentifierExpression.indexIn(text, i, QRegExp::CaretAtOffset);

				if (pos == i)
				{
					const int iLength = indentifierExpression.matchedLength();
					QListIterator<ContentView3::NodePtr> it(elements);
					while (it.hasNext())
					{
						attacheNewPropertyNode(it.next(), indentifierExpression.cap(), m_line);
					}
					i += iLength - 1;
				}
			}
		}
		else if (c == ':')
		{
			if (state != CssIdentifier)
			{
				pos = keywordExpression.indexIn(text, i + 1, QRegExp::CaretAtOffset);
				if (pos == (i + 1))
				{
					const int iLength = keywordExpression.matchedLength();
					// Pseudo class
					elements.append(attacheNewClassNode(rootNode(), ":" + keywordExpression.cap(), m_line));
					i += iLength;
				}
			}
		}
		else if (c == '.')
		{
			if (state != CssIdentifier)
			{
				pos = keywordExpression.indexIn(text, i + 1, QRegExp::CaretAtOffset);
				if (pos == (i + 1))
				{
					const int iLength = keywordExpression.matchedLength();
					// Class
					elements.append(attacheNewClassNode(rootNode(), "." + keywordExpression.cap(), m_line));
					i += iLength;
				}
			}
		}
		else if (c == '#')
		{
			if (state != CssIdentifier)
			{
				pos = keywordExpression.indexIn(text, i + 1, QRegExp::CaretAtOffset);
				if (pos == (i + 1))
				{
					const int iLength = keywordExpression.matchedLength();
					// ID
					elements.append(attacheNewIdNode(rootNode(), "#" + keywordExpression.cap(), m_line));
					i += iLength;
				}
			}
		}
		else if (c == '*')
		{
			if (state == CssDefault)
			{
				elements.append(attacheNewTagNode(rootNode(), "*", m_line));
			}
		}
		else if (c == '{')
		{
			if (state == CssDefault)
				state = CssIdentifier;
		}
		else if (c == '}')
		{
			state = CssDefault;
			elements.clear();
		}
		else if (c == '\n')
		{
			m_line++;
		}
	}
}

QSharedPointer<PropertyNode> Parser::attacheNewPropertyNode(const ContentView3::NodePtr & parent, const QString& name, int line)
{
	QStringList property = name.split(':');
	QString n, v;

	int cnt = property.count();
	if (cnt > 0)
		n = property.at(0).trimmed();
	if (cnt > 1)
		v = property.at(1).trimmed();
	if (v.endsWith(';'))
		v.chop(1);

	QSharedPointer<PropertyNode> node = PropertyNode::create(n, parent);
	node->setLine(line);
	node->setValue(v);

	return node;
}

QSharedPointer<IdentifierNode> Parser::attacheNewIdentifierNode(const ContentView3::NodePtr & parent, const QString& name, int line)
{
	QSharedPointer<IdentifierNode> node = IdentifierNode::create(name, parent);
	node->setLine(line);
	return node;
}

QSharedPointer<ClassNode> Parser::attacheNewClassNode(const ContentView3::NodePtr & parent, const QString& name, int line)
{
	QSharedPointer<ClassNode> node = ClassNode::create(name, parent);
	node->setLine(line);
	return node;
}

QSharedPointer<TagNode> Parser::attacheNewTagNode(const ContentView3::NodePtr & parent, const QString& name, int line)
{
	QSharedPointer<TagNode> node = TagNode::create(name, parent);
	node->setLine(line);
	return node;
}

QSharedPointer<IdNode> Parser::attacheNewIdNode(const ContentView3::NodePtr & parent, const QString& name, int line)
{
	QSharedPointer<IdNode> node = IdNode::create(name, parent);
	node->setLine(line);
	return node;
}

}

}
