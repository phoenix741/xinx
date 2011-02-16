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
#include "file.h"
#include "filenode.h"
#include <project/xinxprojectproject.h>

// Qt header
#include <QStringList>
#include <QPointer>

namespace ContentView3
{

/* PrivateFile */

class PrivateFile : public QSharedData
{
public:
	PrivateFile();
	~PrivateFile();

	XinxProject::ProjectPtrWeak _project;
	QString _filename;
	QStringList _imports;
	QList<File::Symbole> _symbols;
	FileNodePtr _rootNode;
	FilePtrWeak _this;
};

PrivateFile::PrivateFile()
{

}

PrivateFile::~PrivateFile()
{

}

/* File */

FilePtr File::create(XinxProject::ProjectPtr project)
{
	FilePtr ptr(new File(project));
	ptr->d->_this = ptr.toWeakRef();
	return ptr;
}

File::File(XinxProject::ProjectPtr project) : d(new PrivateFile)
{
	d->_project = project.toWeakRef();

	//qDebug() << "Create the file" << this << "with noname";
}

FilePtr File::create(const QString & filename, XinxProject::ProjectPtr project)
{
	FilePtr ptr(new File(filename, project));
	ptr->d->_this = ptr.toWeakRef();
	return ptr;
}

File::File(const QString& filename, XinxProject::ProjectPtr project) : d(new PrivateFile)
{
	d->_filename = filename;
	d->_project  = project;

	//qDebug() << "Create the file" << this << "with name" << filename;
}

File::~File()
{
	//qDebug() << "Destroy the file" << this << "with name" << d->_filename;
}

void File::setFilename(const QString & filename)
{
	d->_filename = QFileInfo(filename).absoluteFilePath();
}

const QString & File::filename() const
{
	return d->_filename;
}

void File::setProject(XinxProject::ProjectPtr project)
{
	d->_project = project.toWeakRef();
}

XinxProject::ProjectPtr File::project() const
{
	return d->_project.toStrongRef();
}

void File::addImport(const QString& import)
{
	if (! d->_imports.contains(import))
	{
		try
		{
			d->_imports.append(import);
			d->_project.toStrongRef()->cache()->addFileToCache(import);
		}
		catch (CacheParserNotFoundException e)
		{
			qDebug() << e.getMessage();
		}
	}
}

void File::clearImports()
{
	d->_imports.clear();
}

const QStringList& File::imports() const
{
	return d->_imports;
}

void File::addSymbole(const ContentView3::File::Symbole& symbole)
{
	d->_symbols.append(symbole);
}

void File::clearSymboles()
{
	d->_symbols.clear();
}

const QList< File::Symbole >& File::symboles() const
{
	return d->_symbols;
}

/*!
 * \brief Can be used to redefine the root node of type FileNode.
 *
 * File class take owner ship of the given FileNode and delete it automatically
 * when necessary.
 */
void File::setRootNode(FileNodePtr node)
{
	if (d->_rootNode.data() != node.data())
	{
		d->_rootNode = node;
		d->_rootNode->setFile(d->_this);
	}
}

FileNodePtr File::rootNode() const
{
	if (d->_rootNode.isNull())
	{
		d->_rootNode = FileNode::create(d->_filename);
		d->_rootNode->setFile(d->_this);
	}
	return d->_rootNode;
}

}
