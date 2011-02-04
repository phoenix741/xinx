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
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QSharedPointer>
#include <QWeakPointer>

namespace ContentView3
{

class File;

typedef QSharedPointer<File>	FilePtr;
typedef QWeakPointer<File>		FilePtrWeak;

class Node;

typedef QSharedPointer<Node>	NodePtr;
typedef QWeakPointer<Node>		NodePtrWeak;

class FileNode;

typedef QSharedPointer<FileNode> FileNodePtr;
typedef QWeakPointer<FileNode> FileNodePtrWeak;

}

#endif // DEFINITIONS_H
