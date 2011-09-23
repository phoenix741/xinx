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

#include "dockwidget.h"
#include <QIcon>
#include <QTreeView>
#include <QVBoxLayout>
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>

namespace ContentView3
{

/*!
 * \class DockWidget
 * \ingroup contentview
 * \since 0.10.1
 *
 * \brief This class is used to show the content of a file in a dock.
 *
 * The content view can be used to show import too.
 */

/*!
 * \fn void DockWidget::open(const QString & filename, int line, IFileTypePluginPtr interface, XinxProject::ProjectPtr project)
 * \brief The signal is emited when a user click on an element.
 * \param filename The file to open
 * \param line The line in the file where move the cursor
 * \param interface The interface used to open the file (with the type of file), actually, the interface is not filled.
 * \param project The project to used when opening the file. The project must be the same that the current project.
 */

//! Create the dock that show the content view
DockWidget::DockWidget(QWidget* parent, Qt::WindowFlags f): XinxDockWidget(parent, f)
{
	setWindowTitle(tr("File Content"));
	setWindowIcon(QIcon(":/images/listbox.png"));

	_contents_tree_view = new QTreeView(this);
	_contents_tree_view->setHeaderHidden(true);

	QVBoxLayout * vlayout = new QVBoxLayout;
	vlayout->setSpacing(0);
	vlayout->setMargin(0);
	vlayout->addWidget(_contents_tree_view);

	setLayout(vlayout);

	connect(EditorManager::self(), SIGNAL(currentChanged(int)), this, SLOT(showModelOfEditor(int)));
	connect(_contents_tree_view, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
}

//! Destroy the dock that show content view.
DockWidget::~DockWidget()
{

}

void DockWidget::doubleClicked(QModelIndex index)
{
	if (index.isValid())
	{
		emit open(index.data(Qt::UserRole + 1).toString(),
				  index.data(Qt::UserRole + 2).toInt(),
				  IFileTypePluginPtr(),
				  index.data(Qt::UserRole + 3).value<XinxProject::ProjectPtrWeak>().toStrongRef());
	}
}

void DockWidget::showModelOfEditor(int editorIndex)
{
	if (editorIndex >= 0)
	{
		AbstractEditor * editor = EditorManager::self()->editor(editorIndex);
		_content_model = editor->model();
	}
	else
	{
		_content_model = NULL;
	}

	_contents_tree_view->setModel(_content_model);
}

}

