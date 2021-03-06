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

// Xinx header
#include "parser.h"
#include "file.h"
#include "filenode.h"
#include <project/externalfileresolver.h>
#include <project/xinxprojectproject.h>
#include <core/errormanager.h>
#include <contentview3/parserexception.h>
#include <contentview3/cacheexception.h>

// Qt header
#include <QDebug>

namespace ContentView3
{

/* PrivateParser */

class PrivateParser
{
public:
	PrivateParser();
	~PrivateParser();

	FilePtr _file;
	QIODevice * _device;
	QString _workingPath;
	int _decalageLine;
	NodePtr _rootNode;
	FileNodePtr _fileRootNode;
	ResolverContextInformation _ctxt;
};

PrivateParser::PrivateParser() : _device(0), _decalageLine(0)
{

}

PrivateParser::~PrivateParser()
{
	delete _device;
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

QString Parser::description() const
{
	return tr("Parse the file %1").arg(QFileInfo(d->_file->filename()).fileName());
}

/*!
 * \fn virtual void Parser::parse() = 0
 * \brief Load the content of the given device.
 *
 * This method must be derivated in sub-class.
 */


/*!
 * \brief Set the file to \p file
 */
void Parser::setFile(FilePtr file)
{
	d->_file = file;
	if (d->_file)
	{
		d->_fileRootNode = d->_file->rootNode()->clone();
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
 * The device can be the content of a file or the content of the memory.
 * Parser take ownerhip of the device. Let the parser destroy the device. If you want
 * keep ownership, call setDevice(0) before destroying the parser.
 */
void Parser::setDevice(QIODevice* device)
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
	if (d->_rootNode.isNull() && d->_file)
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

void Parser::setContext(ResolverContextInformation ctxt)
{
	d->_ctxt = ctxt;
}

const ResolverContextInformation & Parser::context()
{
	if (d->_ctxt.isEmpty() && d->_file)
	{
		XinxProject::ProjectPtr projectPtr = d->_file->project();
		if (projectPtr)
		{
			d->_ctxt = projectPtr->resolver()->createContextInformation(d->_file->filename());
		}
	}
	return d->_ctxt;
}

/*!
 * \brief Add the import to the list
 *
 * The name of the import is not resolved here. This method do nothing if no ContentView3::File is
 * attached to the parser.
 *
 * If possible, we try to pre-load the cache.
 */
QString Parser::addImport(const QString& import)
{
	if (d->_file)
	{
		d->_file->addImport(import);

		/* After adding the import, we pre-load the cache */
		XinxProject::ProjectPtr projectPtr = d->_file->project();
		try
		{
			projectPtr->cache()->addFileToCache(import, context());
		}
		catch (CacheParserNotFoundException e)
		{
			addWarning(-1, e.getMessage());
		}
	}
	return import;
}

void Parser::addWarning(int line, const QString& message)
{
	QMetaObject::invokeMethod(	ErrorManager::self(), "addMessage", Qt::QueuedConnection,
								Q_ARG(QString, d->_file ? d->_file->filename() : "XINX"),
								Q_ARG(int, line),
								Q_ARG(QtMsgType, QtWarningMsg),
								Q_ARG(QString, message));
}

void Parser::addError(int line, const QString& message)
{
	QMetaObject::invokeMethod(	ErrorManager::self(), "addMessage", Qt::QueuedConnection,
								Q_ARG(QString, d->_file ? d->_file->filename() : "XINX"),
								Q_ARG(int, line),
								Q_ARG(QtMsgType, QtCriticalMsg),
								Q_ARG(QString, message));
}

void Parser::startJob()
{
	try
	{
		QMetaObject::invokeMethod(ErrorManager::self(), "clearMessages", Qt::QueuedConnection,
								  Q_ARG(QString, d->_file ? d->_file->filename() : "XINX"));

		// Open the file to read it
		qDebug() << tr("The file %1 will be locked");
		if (d->_device && !d->_device->open(QIODevice::ReadOnly))
		{
			throw ParserException(tr("Can't open \"%1\" : %2").arg(description()).arg(d->_device->errorString()), -1);
		}

		parse();

		d->_file->setRootNode(d->_fileRootNode);

		// Close the file to not lock the file.
		if (d->_device)
		{
			d->_device->close();
		}
		qDebug() << tr("The file %1 is unlocked");
	}
	catch (const ParserException & e)
	{
		QMetaObject::invokeMethod(ErrorManager::self(), "addMessage", Qt::QueuedConnection,
								  Q_ARG(QString, d->_file ? d->_file->filename() : "XINX"),
								  Q_ARG(int, e.getLine()), Q_ARG(QtMsgType, QtCriticalMsg),
								  Q_ARG(XinxException, e));
	}

	if (d->_file && d->_file->project())
	{
		XinxProject::ProjectPtr project = d->_file->project();

		QMetaObject::invokeMethod(project->cache(), "parsed", Qt::QueuedConnection, Q_ARG(ContentView3::FilePtr, d->_file));
	}
}

}
