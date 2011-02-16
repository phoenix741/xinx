/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
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

#pragma once
#ifndef CONTENTVIEW3_PARSER_H
#define CONTENTVIEW3_PARSER_H

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>
#include <jobs/xinxjob.h>
#include <contentview3/node.h>
#include <contentview3/file.h>

// Qt header
#include <QIODevice>
#include <QApplication>
#include <QScopedPointer>

namespace ContentView3
{

class Node;
class PrivateParser;

class LIBEXPORT ParserException : public XinxException
{
public:
	ParserException(QString message, int line, int column = 0);

	int getLine() const;
	int getColumn() const;
private:
	int m_line, m_column;
};

class LIBEXPORT Parser : public XinxJob
{
	Q_OBJECT
public:
	Parser();
	virtual ~Parser();

	virtual QString name() const = 0;
	virtual QString description() const;

	void setFile(FilePtr file);
	FilePtr file() const;

	void setRootNode(NodePtr node);
	NodePtr rootNode() const;

	void setDevice(QIODevice * device);
	QIODevice * device() const;

	void setDecalage(int line);
	int decalage() const;

	void setWorkingPath(const QString & path);
	const QString & workingPath() const;

	virtual void run();
protected:
	virtual void parse() = 0;
	void loadExternalContent(Node * parent, const QString & content, const QString & parserType);
	QString addImport(const QString & import);
private:
	virtual void startJob();

	QScopedPointer<PrivateParser> d;
};

}

#endif // CONTENTVIEW3_PARSER_H
