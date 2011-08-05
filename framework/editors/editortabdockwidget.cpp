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

#include "editortabdockwidget.h"
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>
#include <QVBoxLayout>

EditorTabDockWidget::EditorTabDockWidget(QWidget *parent) : XinxDockWidget(parent)
{
	setWindowTitle(tr("Open Documents"));
	setWindowIcon(QIcon(":/images/editcopy.png"));

	_documents = new QListWidget;
	_documents->setSortingEnabled(true);

	QVBoxLayout * layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addWidget(_documents);

	setLayout(layout);

	connect(EditorManager::self(), SIGNAL(currentChanged(int)), this, SLOT(updateCurrentEditor(int)));
	connect(EditorManager::self(), SIGNAL(fileOpened(QString)), this, SLOT(openEditor(QString)));
	connect(EditorManager::self(), SIGNAL(fileClosed(QString)), this, SLOT(closeEditor(QString)));

	connect(_documents, SIGNAL(itemSelectionChanged()), this, SLOT(listChanged()));
}

void EditorTabDockWidget::listChanged()
{
	QListWidgetItem * item = _documents->currentItem();
	if (item)
	{
		emit open(item->data(Qt::UserRole).toString());
	}
}

void EditorTabDockWidget::updateCurrentEditor(int index)
{
	if (index == -1)
	{
		_documents->setCurrentRow(-1);
		return;
	}


	AbstractEditor * editor = EditorManager::self()->editor(index);
	if (! editor) return;

	for(int i = 0; i < _documents->count(); i++)
	{
		if (_documents->item(i)->data(Qt::UserRole).toString() == editor->lastFileName())
		{
			_documents->setCurrentRow(i);
			break;
		}
	}
}

void EditorTabDockWidget::openEditor(const QString & filename)
{
	AbstractEditor * editor = EditorManager::self()->editor(filename);

	QListWidgetItem * item = new QListWidgetItem(editor->icon(), editor->getTitle());
	item->setToolTip(editor->getLongTitle());
	item->setData(Qt::UserRole, filename);
	_documents->addItem(item);
}

void EditorTabDockWidget::closeEditor(const QString & filename)
{
	for(int i = 0; i < _documents->count(); i++)
	{
		if (_documents->item(i)->data(Qt::UserRole).toString() == filename)
		{
			delete _documents->takeItem(i);
			break;
		}
	}
}

void EditorTabDockWidget::saveEditor(const QString & filename, const QString & oldfilename)
{
	if (filename != oldfilename)
	{
		AbstractEditor * editor = EditorManager::self()->editor(filename);
		Q_ASSERT_X(editor, "EditorTabDockWidget::saveEditor", "Editor must exist");
		for(int i = 0; i < _documents->count(); i++)
		{
			QListWidgetItem * item;
			if ((item = _documents->item(i))->data(Qt::UserRole).toString() == oldfilename)
			{
				item->setText(editor->getTitle());
				item->setToolTip(editor->getLongTitle());
				item->setIcon(editor->icon());
				item->setData(Qt::UserRole, filename);
				break;
			}
		}
	}
}
