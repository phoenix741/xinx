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

#ifndef _QMENUVIEW_H_
#define _QMENUVIEW_H_
#pragma once

// Qt header
#include <QMenu>
#include <QAbstractItemModel>

class QMenuView : public QMenu
{
	Q_OBJECT
public:
	QMenuView(QWidget * parent = 0);
	virtual ~QMenuView();

	virtual void setModel(QAbstractItemModel * model);
	QAbstractItemModel * model() const;

	virtual void setRootIndex(const QModelIndex & index);
	QModelIndex rootIndex() const;
protected:
	virtual bool prePopulated();
	virtual void postPopulated();
	void createMenu(const QModelIndex &parent, QMenu *parentMenu = 0, QMenu *menu = 0);
signals:
	void hovered(const QString &text) const;
	void triggered(const QModelIndex & index) const;
private slots:
	void aboutToShow();
	void triggered(QAction *action);
	void hovered(QAction *action);
private:
	QAction *makeAction(const QModelIndex &index);

	QAbstractItemModel * m_model;
	QPersistentModelIndex m_root;
};


#endif /* _QMENUVIEW_H_ */
