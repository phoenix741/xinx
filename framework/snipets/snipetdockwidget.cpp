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

// Xinx header
#include "snipetdockwidget.h"
#include "snipetdockwidget_p.h"
#include <utils/recursivesortfilterproxymodel.h>
#include <snipets/snipetdockitemmodel.h>
#include <snipets/snipetmanager.h>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>
#include <editors/editormanager.h>
#include <application/customdialog/customdialogimpl.h>

// Qt header
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QVariant>

/* SnipetDockWidgetPrivate */

SnipetDockWidgetPrivate::SnipetDockWidgetPrivate(SnipetDockWidget* parent) : QObject(parent), _ui(new Ui::SnipetsDockWidget), _dock(0), _widget(parent)
{

}

SnipetDockWidgetPrivate::~SnipetDockWidgetPrivate()
{

}

void SnipetDockWidgetPrivate::init()
{
	_ui->setupUi(_widget);
	_widget->setWindowTitle(tr("Snipets"));
	_widget->setWindowIcon(QIcon(":/images/template.png"));

	/* Snipets Tree */
	m_snipetFilterModel = new RecursiveSortFilterProxyModel(_ui->m_snipetTreeView);
	m_snipetFilterModel->setShowAllChild(true);
	m_snipetFilterModel->setDynamicSortFilter(true);
	m_snipetFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

	m_snipetModel = SnipetManager::self()->createSnipetDockItemModel(m_snipetFilterModel);
	m_snipetModel->select();
	m_snipetFilterModel->setSourceModel(m_snipetModel);
	m_snipetFilterModel->setIncludeIndex(QModelIndexList() << m_snipetFilterModel->mapToSource(m_snipetFilterModel->index(0, 0)));

	_ui->m_snipetTreeView->setModel(m_snipetFilterModel);
	_ui->m_snipetTreeView->setSortingEnabled(true);

	_ui->m_snipetTreeView->sortByColumn(0, Qt::AscendingOrder);
	_ui->m_snipetTreeView->expandAll();

	connect(_ui->m_createSnipetBtn, SIGNAL(clicked()), this, SLOT(createSnipet()));
	connect(_ui->m_paramSnipetBtn, SIGNAL(clicked()), this, SLOT(customizeSnipet()));

	connect(_ui->m_snipetFilter, SIGNAL(textChanged(QString)), this, SLOT(filterChanged(QString)));
	connect(_ui->m_snipetTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(callSnipet(QModelIndex)));

	connect(XINXConfig::self(), SIGNAL(changed()), m_snipetModel, SLOT(select()));

	_widget->setEditor(0);
}

void SnipetDockWidgetPrivate::createSnipet()
{
	SnipetManager::self()->addSnipet(1, _widget);
	m_snipetModel->select();
}

void SnipetDockWidgetPrivate::customizeSnipet()
{
	CustomDialogImpl dlg(_widget);
	dlg.loadConfig();

	if (dlg.execUniquePage(5))
	{
		dlg.saveConfig();
		XINXConfig::self()->save();
	}
}

void SnipetDockWidgetPrivate::callSnipet(const QModelIndex & index)
{
	if (index.isValid() && EditorManager::self()->currentEditor())
	{
		if (index.data(SnipetDockItemModel::SnipetTypeRole).toString() == "SNIPET")
		{
			TextFileEditor * ed = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
			int id = index.data(SnipetDockItemModel::SnipetIdRole).toInt();
			QString result;
			if (SnipetManager::self()->callSnipet(id, &result, _widget))
			{
				ed->textEdit()->insertText(result);
			}
		}
	}
}

void SnipetDockWidgetPrivate::filterChanged(const QString & filterText)
{
	m_snipetFilterModel->invalidate();
	m_snipetFilterModel->setFilterRegExp(filterText);
	_ui->m_snipetTreeView->expandAll();
}

/* SnipetDockWidget */

SnipetDockWidget::SnipetDockWidget(QWidget * parent) : QWidget(parent), d(new SnipetDockWidgetPrivate(this))
{
	d->init();
	setFocusProxy(d->_ui->m_snipetFilter);
}

SnipetDockWidget::~SnipetDockWidget()
{

}

void SnipetDockWidget::setEditor(AbstractEditor * ed)
{
	if ((!ed) || (d->m_editor != ed))
	{
		d->m_editor = ed;
		bool enabled = false;
		if (qobject_cast<TextFileEditor*>(ed))
			enabled = true;
	}
}



