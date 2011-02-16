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
#include "stylesheet_importnode.h"
#include <plugins/interfaces/files.h>
#include <plugins/xinxpluginsloader.h>
#include <contentview3/file.h>
#include <project/xinxprojectproject.h>

// Qt header
#include <QFileInfo>

namespace Core
{

namespace Stylesheet
{

QSharedPointer<ImportNode> ImportNode::create(const QString & name, ContentView3::NodePtr parent)
{
	QSharedPointer<ImportNode> ptr(new ImportNode(name));
	initalisation(ptr, parent);
	return ptr;
}

ImportNode::ImportNode(const QString& name) : ContentView3::Node()
{
	setName(name);
	setType("IMPORT");
}

ImportNode::~ImportNode()
{

}

void ImportNode::setName(const QString& name)
{
	ContentView3::Node::setName(name);

	QList<IFileTypePlugin*> fileTypes = XinxPluginsLoader::self()->matchedFileType(name);
	if (fileTypes.size() > 0)
	{
		setIcon(fileTypes.at(0)->icon());
	}
	else
	{
		setIcon(":/images/import.png");
	}

}

QString ImportNode::displayName() const
{
	ContentView3::FilePtr filePtr = file().toStrongRef();

	if (filePtr && filePtr->project() && name().contains(filePtr->project()->projectPath()))
	{
		return QDir(filePtr->project()->projectPath()).relativeFilePath(name());
	}
	else
	{
		return QFileInfo(name()).fileName();
	}
}

}

}
