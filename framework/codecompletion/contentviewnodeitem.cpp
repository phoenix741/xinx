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

#include "contentviewnodeitem.h"
#include <contentview3/node.h>

namespace CodeCompletion
{

/* PrivateContentViewNodeItem */

class PrivateContentViewNodeItem
{
public:
	ContentView3::NodePtr _node;
};

/* ContentViewNodeItem */

ContentViewNodeItem::ContentViewNodeItem(const ContentView3::NodePtr & node) : d(new PrivateContentViewNodeItem)
{
	d->_node = node;

	setIcon(QIcon(node->icon()));
	setText(node->displayName());
	setCompletionText(node->name());
	setCompletionHelper(node->completionString());
	setCompletionType(node->type());
	setKeyString(node->keyString());
}

ContentViewNodeItem::~ContentViewNodeItem()
{

}

const ContentView3::NodePtr & ContentViewNodeItem::node() const
{
	return d->_node;
}

}