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

#include "parser.h"
#include "file.h"
#include "filenode.h"

// Qt header
#include <QDebug>
#include <project/externalfileresolver.h>
#include <project/xinxprojectproject.h>

namespace ContentView3
{

/* ParserException */

/*!
 * \ingroup ContentView3
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

/* PrivateParser */

class PrivateParser
{
public:
	PrivateParser();
	~PrivateParser();

	QString locationOf(const QString & relativeFilename);

	FilePtr _file;
	QIODevice * _device;
	QString _workingPath;
	int _decalageLine;
	NodePtr _rootNode;
	FileNodePtr _fileRootNode;
};

PrivateParser::PrivateParser() : _device(0), _decalageLine(0)
{

}

PrivateParser::~PrivateParser()
{
	delete _device;
}

QString PrivateParser::locationOf(const QString& relativeFilename)
{
	if (_file && _file->project())
	{
		return _file->project()->resolver()->resolveFileName(relativeFilename, _workingPath);
	}
	else
	{
		return relativeFilename;
	}
}

/* Parser */

/*!
 * \ingroup ContentView3
 * \class Parser
 * \since 0.9.1.0
 *
 * \brief The content view parser create the content view tree
 *
 * This class must be sub-classing to complete the structure used for content view. You must have a parser
 * for each type of file.
 */

//! Create an instance of parser
Parser::Parser() : d(new PrivateParser)
{

}

//! Destroy the parser
Parser::~Parser()
{

}

QString Parser::description () const
{
	return tr("Parse the file %1").arg(QFileInfo(d->_file->filename ()).fileName ());
}

/*!
 * \fn virtual void parse() = 0
 * \brief Load the content of the given device.
 *
 * This method must be derivated in sub-class.
 */


/*!
 * \brief Set the file to \p file
 */
void Parser::setFile ( FilePtr file )
{
	d->_file = file;
	if (d->_file)
	{
		d->_fileRootNode = d->_file->rootNode ()->clone();
	}
}

/*!
 * \brief Return the file of the parser
 */
ContentView3::FilePtr Parser::file() const
{
	return d->_file;
}

/*!
 * \brief Set the device used to read the content of the file.
 *
 * The path can be a content of a file or the content of the memory.
 */
void Parser::setDevice ( QIODevice* device )
{
	d->_device = device;
}

/*!
 * \brief Get the root node that can be used by the parser.
 *
 * If the rootNode is not defined, the file rootNode is used.
 */
NodePtr Parser::rootNode() const
{
	if (d->_rootNode.isNull () && d->_file)
	{
		return d->_fileRootNode;
	}
	else
	{
		return d->_rootNode;
	}
}

/*!
 * \brief Define the root node to use in the parser.
 *
 * If not defined (0), the parser used the rootNode of the file.
 */
void Parser::setRootNode(ContentView3::NodePtr node)
{
	d->_rootNode = node;
}

/*!
 * \brief Return the current device of the parser (if set).
 */
QIODevice* Parser::device() const
{
	return d->_device;
}

/*!
 * \brief Set a decalage when attach a node to the parent
 */
void Parser::setDecalage(int line)
{
	d->_decalageLine = line;
}

/*!
 * \brief Return the current decalage
 */
int Parser::decalage() const
{
	return d->_decalageLine;
}

/*!
 * \brief Set the working path (where the parser must run and find import)
 */
void Parser::setWorkingPath(const QString& path)
{
	d->_workingPath = path;
}

/*!
 * \brief Get the working path (where the parser must run and find import)
 */
const QString& Parser::workingPath() const
{
	if (d->_workingPath.isEmpty() && d->_file && d->_file->project())
	{
		return d->_file->project()->projectPath();
	}
	else
	{
		return d->_workingPath;
	}
}



/*!
 * \brief Add the import to the list
 */
QString Parser::addImport(const QString& import)
{
	QString calculateImport = d->locationOf(import);
	if (d->_file)
	{
		d->_file->addImport(calculateImport);
	}
	return calculateImport;
}

void Parser::startJob()
{
	try
	{
		parse ();

		d->_file->setRootNode (d->_fileRootNode);

		QMetaObject::invokeMethod(ErrorManager::self (), "clearMessages", Qt::QueuedConnection,
								  Q_ARG(QString, d->_file ? d->_file->filename () : "XINX"));
	}
	catch(const ParserException & e)
	{
		QMetaObject::invokeMethod(ErrorManager::self (), "addMessage", Qt::QueuedConnection,
								  Q_ARG(QString, d->_file ? d->_file->filename () : "XINX"),
								  Q_ARG(int, e.getLine ()), Q_ARG(QtMsgType, QtCriticalMsg),
								  Q_ARG(XinxException, e));
	}
}

/*!
 * \brief Re-implemented to add the capability to know if the parser is running.
 */
void Parser::run()
{
	XinxJob::run ();

	if (d->_file && d->_file->project ())
	{
		XinxProject::ProjectPtr project = d->_file->project ();

		QMetaObject::invokeMethod(project->cache (), "parsed", Qt::QueuedConnection, Q_ARG(ContentView3::FilePtr, d->_file));
	}
}

}
