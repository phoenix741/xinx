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

#ifndef TREEMODEL_P_H
#define TREEMODEL_P_H

#include "treemodel.h"
#include <contentview3/filenode.h>
#include <contentview3/cache.h>
#include <project/xinxprojectproject.h>
#include <QSet>

namespace ContentView3
{

class TreeNode : public QStandardItem
{
public:
	TreeNode(NodePtr node);

	void updateFromNode(NodePtr node);
	virtual QVariant data (int role = Qt::UserRole + 1) const;

	bool _is_children_populate;
	XinxProject::ProjectPtrWeak _project;
	QString _key;
	QString _filename, _name;
	int _line;
};


class PrivateTreeModel : public QObject
{
	Q_OBJECT
public:
	PrivateTreeModel(TreeModel * parent) : _parent(parent) {}

	QStandardItem * appendRow (QStandardItem * item, NodePtr child_node);
	void synchronize(QStandardItem * item, NodePtr node);
	QStandardItem * createItemFromNode(NodePtr node);

	FilePtr _file;
	TreeModel * _parent;
private slots:
	void updateModel(FilePtr f);
};

}

#endif // TREEMODEL_P_H
