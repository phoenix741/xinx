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

#include "treemodel_p.h"

namespace ContentView3
{

TreeNode::TreeNode(NodePtr node) : QStandardItem(QIcon(node->icon ()), node->displayName ()), _project(0)
{
	_is_children_populate = node->type () != "IMPORT";
	setEditable (false);
	updateFromNode (node);
}

void TreeNode::updateFromNode(NodePtr node)
{
	FilePtr file = node->file ().toStrongRef ();

	setToolTip (node->tips ());
	_name = node->name ();
	_key = node->keyString ();
	_project = file->project ();

	if (node->type() != "IMPORT")
	{
		_filename = node->filename ();
		_line = node->line ();
	}
	else
	{
		_filename = node->name();
		_line = 0;
	}
}

QVariant TreeNode::data (int role) const
{
	switch(role)
	{
	case Qt::UserRole + 1:
		return _filename;
		break;
	case Qt::UserRole + 2:
		return _line;
		break;
	case Qt::UserRole + 3:
		return QVariant::fromValue(_project);
		break;
	}

	return QStandardItem::data (role);
}

/* PrivateTreeModel */

QStandardItem * PrivateTreeModel::appendRow (QStandardItem * item, NodePtr child_node)
{
	TreeNode* child_item = 0;
	child_item = new TreeNode (child_node);
	item->appendRow (child_item);
	return child_item;
}

void PrivateTreeModel::synchronize(QStandardItem * item, NodePtr node)
{
	// Create the node list
	QHash<QString,QStandardItem*> childs;

	for(int i = 0; i < item->rowCount (); i++)
	{
		TreeNode* child = static_cast<TreeNode*>(item->child (i));
		childs.insert (child->_key, child);
	}

	QSet<QString> mark_deleted = QSet<QString>::fromList (childs.keys ());
	foreach(NodePtr child_node, node->childs())
	{
		const QString key = child_node->keyString ();
		QStandardItem * child_item;
		if (childs.contains (key))
		{
			child_item = childs.value (key);
			mark_deleted.remove (key);
		}
		else
		{
			child_item = appendRow (item, child_node);
		}

		synchronize (child_item, child_node);
	}

	foreach(const QString & to_delete, mark_deleted)
	{
		item->removeRow (childs.value (to_delete)->row ());
	}

	item->sortChildren (0);
}

void PrivateTreeModel::updateModel(FilePtr f)
{
	// FIXME: Should update files if a file is opened
}

/* TreeModel */

TreeModel::TreeModel(FilePtr file, QObject* parent): QStandardItemModel(parent), d(new PrivateTreeModel(this))
{
	d->_file = file;
}

TreeModel::TreeModel(QObject* parent): QStandardItemModel(parent), d(new PrivateTreeModel(this))
{

}

TreeModel::~TreeModel()
{

}

FilePtr TreeModel::file() const
{
	return d->_file;
}

void TreeModel::setFile(FilePtr file)
{
	if (d->_file != file)
	{
		d->_file = file;
	}
}

void TreeModel::reload()
{
	if (d->_file)
	{
		d->synchronize (invisibleRootItem (), d->_file->rootNode());
	}
}

bool TreeModel::hasChildren (const QModelIndex & parent) const
{
	if (parent.isValid ())
	{
		TreeNode* node = static_cast<TreeNode*> (itemFromIndex (parent));
		return (node && ! node->_is_children_populate) || QStandardItemModel::hasChildren (parent);
	}

	return QStandardItemModel::hasChildren (parent);
}

bool TreeModel::canFetchMore (const QModelIndex & parent) const
{
	if (parent.isValid ())
	{
		TreeNode* node = static_cast<TreeNode*> (itemFromIndex (parent));
		return ! node->_is_children_populate || QStandardItemModel::canFetchMore (parent);
	}

	return QStandardItemModel::canFetchMore (parent);
}

void TreeModel::fetchMore (const QModelIndex & parent)
{
	if (parent.isValid () && d->_file)
	{
		TreeNode* node = static_cast<TreeNode*> (itemFromIndex (parent));
		if (! node->_is_children_populate)
		{
			FilePtr imported_file = d->_file->project ()->cache ()->cachedFile (node->_name);

			if (imported_file)
			{
				d->synchronize (node, imported_file->rootNode ());
			}

			node->_is_children_populate = true;
		}
	}

	QStandardItemModel::fetchMore (parent);
}

QString TreeModel::filenameOfIndex(const QModelIndex & index) const
{
	if (index.isValid ())
	{
		TreeNode* node = static_cast<TreeNode*> (itemFromIndex (index));
		return node->_filename;
	}
	else
	{
		return QString();
	}
}

int TreeModel::lineOfIndex(const QModelIndex & index) const
{
	if (index.isValid ())
	{
		TreeNode* node = static_cast<TreeNode*> (itemFromIndex (index));
		return node->_line;
	}
	else
	{
		return -1;
	}
}

}

