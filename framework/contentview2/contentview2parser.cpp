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

/*!
 * \ingroup ContentView2
 * \class ParserException
 * \brief Exception throw when the model can't be updated.
 */

/*!
 * \brief Create the exception with a message and a line.
 * \param message Error of the exception.
 * \param line Line where the error is.
 * \param column Column where the error is.
 */
ParserException::ParserException(QString message, int line, int column) : XinxException(message), m_line(line), m_column(column)
{

}

/*!
 * \brief Return the line where the error is.
 * \return The line of the error.
 */
int ParserException::getLine() const
{
	return m_line;
}

/*!
 * \brief Return the column where the error is.
 * \return the column of the error.
 */
int ParserException::getColumn() const
{
	return m_column;
}

/* Parser */

/*!
 * \ingroup ContentView2
 * \class Parser
 * \since 0.9.0.0
 *
 * \brief The content view parser create the content view tree (and fill the base)
 *
 * This class must be sub-classing to complete the database with the content of file. You must have a parser
 * for each type of file.
 */

//! Create an instance of parser
Parser::Parser() : m_decaledLine(0), m_device(0)
{
}

//! Destroy the parser
Parser::~Parser()
{
}

/*!
 * \fn virtual void Parser::load() = 0
 * \brief Load the content of the givent device and return true if sucessfully loaded
 *
 * This method must be derivated in sub-class.
 */

/*!
 * \brief Set a decalage when attach a node to the parent
 */
void Parser::setDecalage(int line)
{
	m_decaledLine = line;
}

/*!
 * \brief Return the current decalage
 */
int Parser::decalage() const
{
	return m_decaledLine;
}

/*!
 * \brief Set the root node
 */
void Parser::setRootNode(const Node & node)
{
	m_rootNode = node;
}

/*!
 * \brief Return the root node
 */
Node Parser::rootNode() const
{
	return m_rootNode;
}

/*!
 * \brief Set the filename
 */
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

/*!
 * \brief Get the filename
 */
QString Parser::filename() const
{
	return m_filename;
}

/*!
 * \brief Set the device
 */
void Parser::setInputDevice(QIODevice * device)
{
	m_device = device;
}

/*!
 * \brief Return the device
 */
QIODevice * Parser::inputDevice() const
{
	return m_device;
}

/*!
 * \brief Set the database
 */
void Parser::setDatabase(const QSqlDatabase & db)
{
	m_db = db;
}

/*!
 * \brief Return the database
 */
QSqlDatabase Parser::database() const
{
	return m_db;
}

//! Add the import to the list
QString Parser::addImport(const QString & import)
{
	QString calculateImport = locationOf(import);
	m_imports.append(calculateImport);
	return calculateImport;
}

/*!
 * \brief Get the list of import
 */
const QStringList & Parser::imports() const
{
	return m_imports;
}

/*!
 * \brief Destroy all node of \p rootNode
 */
void Parser::clearNodes(Node rootNode)
{
	rootNode.destroyChildsAsRoot(m_db);
}


/*!
 * \brief Attach the node \e child to \e parent if this node isn't already attached.
 */
void Parser::attachNode(const Node & parent, Node & child)
{
	Q_ASSERT(parent.isValid());

	child.setLine(child.line() + m_decaledLine);
	child.attach(parent.nodeId());
	child.create(m_db);
}

//! Return the location (absolute path) of the filename, with the help of the \e parent node
//! Return the location (absolute path) of the filename, with the help of the \e parent node
QString Parser::locationOf(const QString & relativeFilename)
{
	QString fn = filename();
	if (XINXProjectManager::self()->project() && fn.isEmpty())
		fn = XINXProjectManager::self()->project()->projectPath();
	else
		fn = QFileInfo(filename()).absolutePath();

	return ExternalFileResolver::self()->resolveFileName(relativeFilename, fn);
}

//! By default return NULL, must be sub-class to change the default codec
QTextCodec * Parser::codec()
{
	return 0;
}

} // namespace ContentView2
