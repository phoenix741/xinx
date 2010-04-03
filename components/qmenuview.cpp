/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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
/*
 * This class is inspired from the Nokia class MenuModel, used in the browser
 * application in demos.
 */

// Xinx header
#include "qmenuview.h"

Q_DECLARE_METATYPE(QModelIndex);

/* QMenuView */

/*!
 * \ingroup Components
 * \class QMenuView
 * \since 0.9.0.0
 *
 * \brief The QMenuView provide a menu based view on a QAbstractItemModel class.
 *
 * This class is used to transform a hierarchical model based on the class
 * QAbstractItemModel into a menu. It can be used to create an action menu, history,
 * or snipets menu.
 *
 * \image html qmenuview.png
 * \image latex qmenuview.png
 *
 * When the model is defined, the structure of the menu is automatically generated. This
 * class ignore call to QAbstractItemModel::beginInsertRows() and QAbstractItemModel::endInsertRows().
 * Menu is generated when the user open it.
 */

/*!
 * \brief Create the new menu view based on a QMenu object.
 * \param parent The parent object of the menu.
 */
QMenuView::QMenuView(QWidget * parent) : QMenu(parent)
{
	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(triggered(QAction*)));
	connect(this, SIGNAL(hovered(QAction*)), this, SLOT(hovered(QAction*)));
	connect(this, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
}

//! Destroy the menu.
QMenuView::~QMenuView()
{
	setModel(0);
}

/*!
 * \fn void QMenuView::hovered(const QString &text) const
 * \brief The signal when a menu action is highlighted.
 *
 * \p text is the Qt::StatusTipRole of the index that caused the signal to be emitted.
 *
 * Often this is used to update status information.
 *
 * \sa triggered()
 */

/*!
 * \fn void QMenuView::triggered(const QModelIndex & index) const
 * \brief This signal is emitted when an action in this menu is triggered.
 *
 * \p index is the index's action that caused the signal to be emitted.
 *
 * \sa hovered()
 */

//! Add any actions before the tree, return true if any actions are added.
bool QMenuView::prePopulated()
{
	return false;
}

//! Add any actions after the tree
void QMenuView::postPopulated()
{
}

/*!
 * \brief Set the new model to \p model.
 * \param model The new model to use for the creation of menus.
 */
void QMenuView::setModel(QAbstractItemModel * model)
{
	m_model = model;
}

/*!
 * \brief Return the current model of the menu.
 */
QAbstractItemModel * QMenuView::model() const
{
	return m_model;
}

/*!
 * \brief Change the root index to \p index.
 *
 * This can be used to show only a part of the QAbstractItemModel.
 * \param index The index to use to show the menu. if QModelIndex(), all the model is show.
 */
void QMenuView::setRootIndex(const QModelIndex & index)
{
	m_root = index;
}

/*!
 * \brief Return the current root index.
 *
 * Default root index is QModelIndex()
 */
QModelIndex QMenuView::rootIndex() const
{
	return m_root;
}

void QMenuView::triggered(QAction *action)
{
	QVariant v = action->data();
	if (v.canConvert<QModelIndex>())
	{
		QModelIndex idx = qvariant_cast<QModelIndex>(v);
		emit triggered(idx);
	}
}

void QMenuView::hovered(QAction *action)
{
	QVariant v = action->data();
	if (v.canConvert<QModelIndex>())
	{
		QModelIndex idx = qvariant_cast<QModelIndex>(v);
		QString hoveredString = idx.data(Qt::StatusTipRole).toString();
		if (!hoveredString.isEmpty())
			emit hovered(hoveredString);
	}
}

void QMenuView::aboutToShow()
{
	QMenu * menu = qobject_cast<QMenu*>(sender());
	if (menu)
	{
		QVariant v = menu->menuAction()->data();
		if (v.canConvert<QModelIndex>())
		{
			QModelIndex idx = qvariant_cast<QModelIndex>(v);
			createMenu(idx, menu, menu);
			disconnect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
			return;
		}
	}

	clear();

	if (prePopulated())
		addSeparator();

	createMenu(m_root, this, this);

	postPopulated();
}

//! Put all of the children of parent into menu
void QMenuView::createMenu(const QModelIndex &parent, QMenu *parentMenu, QMenu *menu)
{
	if (! menu)
	{
		QIcon icon = qvariant_cast<QIcon>(parent.data(Qt::DecorationRole));

		QVariant v;
		v.setValue(parent);

		menu = new QMenu(parent.data().toString(), this);
		menu->setIcon(icon);
		parentMenu->addMenu(menu);
		menu->menuAction()->setData(v);
		menu->setEnabled(parent.flags().testFlag(Qt::ItemIsEnabled));

		connect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));

		return;
	}

	int end = m_model->rowCount(parent);
	for (int i = 0; i < end; ++i)
	{
		QModelIndex idx = m_model->index(i, 0, parent);
		if (m_model->hasChildren(idx))
		{
			createMenu(idx, menu);
		}
		else
		{
			menu->addAction(makeAction(idx));
		}
	}
}

QAction * QMenuView::makeAction(const QModelIndex &index)
{
	QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
	QAction * action = new QAction(icon, index.data().toString(), this);
	action->setEnabled(index.flags().testFlag(Qt::ItemIsEnabled));
	QVariant v;
	v.setValue(index);
	action->setData(v);

	return action;
}
