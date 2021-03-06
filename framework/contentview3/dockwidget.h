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

#ifndef CONTENTVIEW3_DOCKWIDGET_H
#define CONTENTVIEW3_DOCKWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include <project/xinxprojectproject.h>
#include <application/xinxdockwidget.h>
#include <plugins/interfaces/files.h>

class QAbstractItemModel;
class QTreeView;

typedef QSharedPointer<IFileTypePlugin> IFileTypePluginPtr;

namespace ContentView3
{

class DockWidget : public XinxDockWidget
{
	Q_OBJECT
public:
	DockWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~DockWidget();
signals:
	void open(const QString & filename, int line, IFileTypePluginPtr interface, XinxProject::ProjectPtr project);
private slots:
	void showModelOfEditor(int editorIndex);
	void doubleClicked(QModelIndex index);
private:
	QTreeView * _contents_tree_view;
	QAbstractItemModel * _content_model;
};

}

#endif // CONTENTVIEW3_DOCKWIDGET_H
