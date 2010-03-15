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

/*!
 * \file contentviewparser.h
 * \brief Contains the element for manage the parser.
 */

#ifndef CONTENTVIEWPARSER_H
#define CONTENTVIEWPARSER_H
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>
#include <contentview2/contentview2node.h>

// Qt header
#include <QApplication>
#include <QSqlDatabase>

namespace ContentView2
{

/*!
 * \class ParserException
 * \brief Exception throw when the model can't be updated.
 */
class LIBEXPORT ParserException : public XinxException
{
public:
	/*!
	 * Create the exception with a message and a line.
	 * \param message Error of the exception.
	 * \param line Line where the error is.
	 */
	ParserException(QString message, QString filename, int line, int column = 0);

	/*!
	 * Return the line where the error is.
	 * \return The line of the error.
	 */
	int getLine() const;

	/*!
	 * Return the column where the error is.
	 * \return the column of the error.
	 */
	int getColumn() const;

	/*!
	 * Return the filename where find the error
	 * \return the filename of the exception.
	 */
private:
	int m_line, m_column;
	QString m_filename;
};

/*!
 * \class Parser
 * \brief The content view parser create the content view tree (and fill the base)
 *
 */
class LIBEXPORT Parser
{
	Q_DECLARE_TR_FUNCTIONS(Parser);
public:
	Parser();
	virtual ~Parser();

	/*! Set the root node */
	void setRootNode(const Node & node);
	/*! Return the root node */
	Node rootNode() const;


	/*! Set the filename */
	virtual void setFilename(const QString & filename);
	/*! Get the filename */
	QString filename() const;
	/*! Set the device */
	void setInputDevice(QIODevice * device);
	/*! Return the device */
	QIODevice * inputDevice() const;
	/*! Set the database */
	void setDatabase(const QSqlDatabase & db);
	/*! Return the database */
	QSqlDatabase database() const;

	/*! Load the content of the givent device and return true if sucessfully loaded */
	virtual void load() = 0;
	/*! Get the list of import */
	const QStringList & imports() const;

	/*! Set a decalage when attach a node to the parent */
	void setDecalage(int line);
	/*! Return the current decalage */
	int decalage() const;

	virtual QTextCodec * codec();
protected:
	/*!
	 * Attach the node \e child to \e parent if this node isn't already attached.
	 */
	void attachNode(const Node & parent, Node & child);

	//! Load all child from the given \e rootNode for future detach
	void loadAttachedNode(const Node & rootNode);
	//! Detach all node again in the list
	void detachAttachedNode();
	//! Remove \e rootNode from the attachedNodeList
	void removeAttachedNode(const Node & rootNode);
	//! Remove all node in the list
	void removeAttachedNodes();

	//! Return the location (absolute path) of the filename, with the help of the \e parent node
	QString locationOf(const QString & relativeFilename);

	//! Add the import to the list
	void addImport(const QString & import);
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
