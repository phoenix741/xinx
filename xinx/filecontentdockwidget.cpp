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
#include "filecontentdockwidget.h"
#include <contentview2/contentview2treemodel.h>
#include <contentview2/contentview2manager.h>
#include <contentview2/contentview2file.h>
#include <contentview2/contentview2node.h>

// Qt header
#include <QVBoxLayout>
#include <QTreeView>
#include <QAbstractItemModel>
#include <QDebug>
#include <QFileInfo>

//#include <modeltest.h>

/* ContentDockWidget */

FileContentDockWidget::FileContentDockWidget(QWidget * parent) : DToolView(tr("File Content"), QIcon(), parent), m_model(0)
{
	init();
}

FileContentDockWidget::~FileContentDockWidget()
{

}

void FileContentDockWidget::init()
{
	m_contentTreeView = new QTreeView(this);

	QVBoxLayout * vlayout = new QVBoxLayout();
	vlayout->setSpacing(0);
	vlayout->setMargin(0);
	vlayout->addWidget(m_contentTreeView);

	QWidget * m_contentWidget = new QWidget(this);
	m_contentWidget->setLayout(vlayout);

	setWidget(m_contentWidget);
	connect(m_contentTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(contentTreeViewDblClick(QModelIndex)));
}

void FileContentDockWidget::contentTreeViewDblClick(QModelIndex index)
{
	ContentView2::Node node(ContentView2::Manager::self()->database(), index.data(ContentView2::Node::NODE_ID).toInt());
	const QString nodeName = node.data(ContentView2::Node::NODE_NAME).toString();

	if (QFileInfo(nodeName).exists())
	{
		emit open(nodeName, 1);
	}
	else
	{
		emit open(QString(), node.line());
	}
}

void FileContentDockWidget::updateModel(QAbstractItemModel * model)
{
	if (model == m_model) return;
//  if( model ) {
//      new ModelTest(model, this);
//  }
	m_contentTreeView->setModel(model);
	m_model = model;
}

void FileContentDockWidget::positionChanged(const QModelIndex & index)
{
	m_contentTreeView->setCurrentIndex(index);
	/*QItemSelectionModel * selectionModel = m_contentTreeView->selectionModel();
	QItemSelection selection( index, index );
	selectionModel->select( selection, QItemSelectionModel::ToggleCurrent );*/
}

