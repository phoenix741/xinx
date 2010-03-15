/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "customsnipetimpl.h"
#include <core/xinxconfig.h>
#include <snipets/snipetmanager.h>
#include <snipets/snipetitemmodel.h>
#include <utils/recursivesortfilterproxymodel.h>

// Qt header
#include <QMenu>
#include <QFileDialog>
#include <QHeaderView>

/* CustomSnipetImpl */

CustomSnipetImpl::CustomSnipetImpl(QWidget * parent) : QWidget(parent)
{
	setupUi(this);

	// Change Button
	QMenu * menu = new QMenu(this);
	QAction * actionAddSnipet   = new QAction(QIcon(":/images/add.png"), tr("Add a new Snipet"), menu);
	QAction * actionAddCategory = new QAction(QIcon(":/images/add.png"), tr("Add a new Category"), menu);

	connect(actionAddSnipet, SIGNAL(triggered()), this, SLOT(m_addSnipetPushButton_clicked()));
	connect(actionAddCategory, SIGNAL(triggered()), this, SLOT(m_addCategoryPushButton_clicked()));

	menu->addAction(actionAddSnipet);
	menu->addAction(actionAddCategory);

	m_addPushButton->setMenu(menu);

	// Snipet
	m_snipetFilterModel = new RecursiveSortFilterProxyModel(m_snipetTreeView);
	m_snipetFilterModel->setShowAllChild(true);
	m_snipetFilterModel->setDynamicSortFilter(true);
	m_snipetFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	//m_snipetFilterModel->setFilterKeyColumn( -1 );

	m_snipetModel = SnipetManager::self()->createSnipetItemModel(m_snipetFilterModel);
	m_snipetFilterModel->setSourceModel(m_snipetModel);

	m_snipetTreeView->setModel(m_snipetFilterModel);
	m_snipetTreeView->setSortingEnabled(true);
	connect(m_snipetTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(m_snipetTreeView_selectionChanged()));
}

CustomSnipetImpl::~CustomSnipetImpl()
{
}

QPixmap CustomSnipetImpl::image()
{
	return QPixmap(":/images/preferences-desktop-keyboard.png");
}

QString CustomSnipetImpl::name()
{
	return windowTitle();
}

QWidget * CustomSnipetImpl::settingsDialog()
{
	return this;
}

bool CustomSnipetImpl::loadSettingsDialog()
{
	// Snipet
	SnipetManager::self()->database().transaction();

	m_snipetModel->select();
	m_snipetFilterModel->setIncludeIndex(QModelIndexList() << m_snipetFilterModel->mapToSource(m_snipetFilterModel->index(0, 0)));
	m_snipetTreeView->header()->setResizeMode(QHeaderView::ResizeToContents);
	m_snipetTreeView->header()->setResizeMode(2, QHeaderView::Stretch);
	m_snipetTreeView->sortByColumn(0, Qt::AscendingOrder);
	m_snipetTreeView->expandAll();
	m_snipetTreeView_selectionChanged();


	return true;
}

bool CustomSnipetImpl::saveSettingsDialog()
{
	SnipetManager::self()->database().commit();

	return true;
}

bool CustomSnipetImpl::cancelSettingsDialog()
{
	SnipetManager::self()->database().rollback();

	return true;
}

bool CustomSnipetImpl::isSettingsValid()
{
	return true;
}

bool CustomSnipetImpl::isVisible()
{
	return true;
}


/*!
 * \internal
 * Get the id of the selected category, or if a snipet is selected, get the
 * id of the parent category
 */
int CustomSnipetImpl::getCategory(const QModelIndex & index)
{
	if (index.data(SnipetItemModel::SnipetTypeRole).toString() == "CATEGORY")
	{
		return index.data(SnipetItemModel::SnipetIdRole).toInt();
	}
	else
	{
		return index.data(SnipetItemModel::SnipetParentIdRole).toInt();
	}
}

void CustomSnipetImpl::on_m_importPushButton_clicked()
{
	QString importedFilename = QFileDialog::getOpenFileName(this, tr("Import snipets"), "datas:/", "*.xml");
	if (! importedFilename.isEmpty())
	{
		SnipetList list;
		list.loadFromFile(importedFilename);
		SnipetManager::self()->importSnipetList(list);
		m_snipetModel->select();
		m_snipetTreeView->expandAll();
	}
}

void CustomSnipetImpl::on_m_exportPushButton_clicked()
{
	QItemSelection selection = m_snipetTreeView->selectionModel()->selection();
	QItemSelection sourceSelection = m_snipetFilterModel->mapSelectionToSource(selection);
	QModelIndexList indexes = sourceSelection.indexes();

	foreach(const QModelIndex & i, indexes)
	{
		if (i.column() > 0) indexes.removeAll(i);
	}

	QString exportedFilename = QFileDialog::getSaveFileName(this, tr("Export snipets"), "datas:/", "*.xml");
	if (! exportedFilename.isEmpty())
	{
		QList<int> ids = m_snipetModel->indexesToIds(indexes);
		SnipetList list;
		if (SnipetManager::self()->exportSnipetList(ids, &list, this))
			list.saveToFile(exportedFilename);
	}
}

void CustomSnipetImpl::m_addSnipetPushButton_clicked()
{
	int categoryId = 1;
	QModelIndexList indexes = m_snipetTreeView->selectionModel()->selectedRows();
	if (indexes.size())
	{
		QModelIndex index = m_snipetFilterModel->mapToSource(indexes.at(0));
		categoryId = getCategory(index);
	}

	SnipetManager::self()->addSnipet(categoryId, this);
	m_snipetModel->select();
	m_snipetTreeView_selectionChanged();
}

void CustomSnipetImpl::m_addCategoryPushButton_clicked()
{
	int categoryId = 1;
	QModelIndexList indexes = m_snipetTreeView->selectionModel()->selectedRows();
	if (indexes.size())
	{
		QModelIndex index = m_snipetFilterModel->mapToSource(indexes.at(0));
		categoryId = getCategory(index);
	}

	SnipetManager::self()->addCategory(categoryId, true, this);
	m_snipetModel->select();
	m_snipetTreeView_selectionChanged();
}

void CustomSnipetImpl::on_m_removePushButton_clicked()
{
	QItemSelection selection = m_snipetTreeView->selectionModel()->selection();
	QItemSelection sourceSelection = m_snipetFilterModel->mapSelectionToSource(selection);
	QModelIndexList indexes = sourceSelection.indexes();

	foreach(QModelIndex i, indexes)
	{
		if (i.column() > 0) indexes.removeAll(i);
	}

	QList<int> snipetIds, categoryIds;
	m_snipetModel->indexesToIds(indexes, snipetIds, categoryIds);

	foreach(int id, snipetIds)
	{
		SnipetManager::self()->removeSnipet(id, this);
	}

	foreach(int id, categoryIds)
	{
		SnipetManager::self()->removeCategory(id, this);
	}

	m_snipetModel->select();
}

void CustomSnipetImpl::on_m_modifyPushButton_clicked()
{
	QModelIndexList indexes = m_snipetTreeView->selectionModel()->selectedRows();
	on_m_snipetTreeView_doubleClicked(indexes.at(0));
}

void CustomSnipetImpl::on_m_duplicatePushButton_clicked()
{
	QModelIndexList indexes = m_snipetTreeView->selectionModel()->selectedRows();
	QModelIndex index = m_snipetFilterModel->mapToSource(indexes.at(0));
	int snipetId = index.data(SnipetItemModel::SnipetIdRole).toInt();

	SnipetManager::self()->duplicateSnipet(snipetId, this);

	m_snipetModel->select();
	m_snipetTreeView_selectionChanged();
}

void CustomSnipetImpl::on_m_snipetTreeView_doubleClicked(const QModelIndex & index)
{
	/* Modify */
	QModelIndex sourceIndex = m_snipetFilterModel->mapToSource(index);
	int id = sourceIndex.data(SnipetItemModel::SnipetIdRole).toInt();
	if (sourceIndex.data(SnipetItemModel::SnipetTypeRole).toString() == "CATEGORY")
	{
		SnipetManager::self()->modifyCategory(id, this);
	}
	else
	{
		SnipetManager::self()->modifySnipet(id, this);
	}
	m_snipetModel->select();
	m_snipetTreeView_selectionChanged();
}

void CustomSnipetImpl::m_snipetTreeView_selectionChanged()
{
	QItemSelection selection = m_snipetTreeView->selectionModel()->selection();
	QItemSelection sourceSelection = m_snipetFilterModel->mapSelectionToSource(selection);
	QModelIndexList indexes = sourceSelection.indexes();

	bool hasCategory = false;
	foreach(const QModelIndex & i, indexes)
	{
		if (i.column() > 0) indexes.removeAll(i);
		if (i.data(SnipetItemModel::SnipetTypeRole).toString() == "CATEGORY") hasCategory = true;
	}
	m_removePushButton->setEnabled(indexes.count() > 0);
	m_duplicatePushButton->setEnabled((!hasCategory) && (indexes.count() == 1));
	m_modifyPushButton->setEnabled(indexes.count() == 1);
	m_exportPushButton->setEnabled(indexes.count() > 0);
}

void CustomSnipetImpl::on_m_snipetFilterLineEdit_textChanged(const QString & filterText)
{
	m_snipetFilterModel->invalidate();
	m_snipetFilterModel->setFilterRegExp(filterText);
	m_snipetTreeView->expandAll();
	//m_snipetTreeView_selectionChanged();
}


