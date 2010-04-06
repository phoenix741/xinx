/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

#pragma once
#ifndef CONTENTVIEWPARSER_H
#define CONTENTVIEWPARSER_H

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>
#include <contentview2/contentview2node.h>

// Qt header
#include <QApplication>
#include <QSqlDatabase>

namespace ContentView2
{

class LIBEXPORT ParserException : public XinxException
{
public:
	ParserException(QString message, int line, int column = 0);

	int getLine() const;
	int getColumn() const;
private:
	int m_line, m_column;
};

class LIBEXPORT Parser
{
	Q_DECLARE_TR_FUNCTIONS(Parser);
public:
	Parser();
	virtual ~Parser();

	void setRootNode(const Node & node);
	Node rootNode() const;


	virtual void setFilename(const QString & filename);
	QString filename() const;
	void setInputDevice(QIODevice * device);
	QIODevice * inputDevice() const;
	void setDatabase(const QSqlDatabase & db);
	QSqlDatabase database() const;

	virtual void load() = 0;
	const QStringList & imports() const;

	void setDecalage(int line);
	int decalage() const;

	virtual QTextCodec * codec();
protected:
	void attachNode(const Node & parent, Node & child);

	void loadAttachedNode(const Node & rootNode);
	void detachAttachedNode();
	void removeAttachedNode(const Node & rootNode);
	void removeAttachedNodes();

	QString locationOf(const QString & relativeFilename);

	QString addImport(const QString & import);
private:
	QList< QPair<uint,uint> > m_attachedNode;
	unsigned long m_decaledLine;
	QString m_filename;
	QIODevice * m_device;
	Node m_rootNode;
	QStringList m_imports;
	QSqlDatabase m_db;
};

} // namespace ContentView

#endif // CONTENTVIEWPARSER_H
