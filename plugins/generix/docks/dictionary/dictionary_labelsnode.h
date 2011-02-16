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

#pragma once
#ifndef GENERIX_DICTIONARY_LABELSNODE_H
#define GENERIX_DICTIONARY_LABELSNODE_H

#include <contentview3/node.h>

namespace Generix
{

namespace Dictionary
{

class LabelsNode : public ContentView3::Node
{
public:
	static QSharedPointer<LabelsNode> create(const QString & name, ContentView3::NodePtr parent = ContentView3::NodePtr());
	virtual ~LabelsNode();

	virtual QString completionString() const;
protected:
	LabelsNode(const QString& name);
};

}

}

#endif /* GENERIX_DICTIONARY_LABELSNODE_H */
