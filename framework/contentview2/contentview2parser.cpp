/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2010 by Ulrich Van Den Hekke                              *
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
#include "contentview2parser.h"
#include <project/xinxproject.h>
#include <project/externalfileresolver.h>

// Qt header
#include <QFileInfo>
#include <QFile>

namespace ContentView2
{

/* ParserException */

ParserException::ParserException(QString message, int line, int column) : XinxException(message), m_line(line), m_column(column)
{

}

int ParserException::getLine() const
{
	return m_line;
}

int ParserException::getColumn() const
{
	return m_column;
}

/* Parser */

Parser::Parser() : m_decaledLine(0), m_device(0)
{
}

Parser::~Parser()
{
}

void Parser::setDecalage(int line)
{
	m_decaledLine = line;
}

int Parser::decalage() const
{
	return m_decaledLine;
}

void Parser::setRootNode(const Node & node)
{
	m_rootNode = node;
}

Node Parser::rootNode() const
{
	return m_rootNode;
}

void Parser::setFilename(const QString & filename)
{
	QFile * file = new QFile(filename);

	// Open the file
	if (! file->open(QFile::ReadOnly))
	{
		QString errorString = file->errorString();
		delete file;
		throw ParserException(errorString, -1);
	}

	delete m_device;
	m_filename = filename;
	m_device = file;
}

QString Parser::filename() const
{
	return m_filename;
}

void Parser::setInputDevice(QIODevice * device)
{
	m_device = device;
}

QIODevice * Parser::inputDevice() const
{
	return m_device;
}

void Parser::setDatabase(const QSqlDatabase & db)
{
	m_db = db;
}

QSqlDatabase Parser::database() const
{
	return m_db;
}

QString Parser::addImport(const QString & import)
{
	QString calculateImport = locationOf(import);
	m_imports.append(calculateImport);
	return calculateImport;
}

const QStringList & Parser::imports() const
{
	return m_imports;
}

void Parser::attachNode(const Node & parent, Node & child)
{
	Q_ASSERT(parent.isValid());

	int id;
	child.setLine(child.line() + m_decaledLine);
	if ((id = parent.indexOfChild(m_db, child)) >= 0)
	{
		child.load(m_db, id);
		removeAttachedNode(child);
	}
	else
	{
		child.create(m_db);
		child.attach(m_db, parent.nodeId());
		removeAttachedNode(child);
	}
}

void Parser::loadAttachedNode(const Node & rootNode)
{
	if (! rootNode.isValid()) return;
	foreach(uint n, rootNode.childs(m_db))
	{
		m_attachedNode.append(qMakePair(rootNode.nodeId(), n));
	}
}

void Parser::detachAttachedNode()
{
	QPair<uint,uint> p;
	foreach(p, m_attachedNode)
	{
		Node child(m_db, p.second);
		child.detach(m_db, p.first);
		child.destroy(m_db);
	}
	m_attachedNode.clear();
}

void Parser::removeAttachedNode(const Node & n)
{
	QMutableListIterator< QPair<uint,uint> > i(m_attachedNode);
	while (i.hasNext())
	{
		QPair<uint,uint> p = i.next();
		if (p.second == n.nodeId())
		{
			i.remove();
		}
	}
}

void Parser::removeAttachedNodes()
{
	m_attachedNode.clear();
}

QString Parser::locationOf(const QString & relativeFilename)
{
	QString fn = filename();
	if (XINXProjectManager::self()->project() && fn.isEmpty())
		fn = XINXProjectManager::self()->project()->projectPath();
	else
		fn = QFileInfo(filename()).absolutePath();

	return ExternalFileResolver::self()->resolveFileName(relativeFilename, fn);
}

QTextCodec * Parser::codec()
{
	return 0;
}

} // namespace ContentView2
