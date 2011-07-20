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

#ifndef GENERIXPROJECTDOCK_H
#define GENERIXPROJECTDOCK_H
#pragma once

// Xinx header
#include <ui_gnxprojectdock.h>
#include <project/xinxprojectproject.h>
#include <application/xinxdockwidget.h>

// Qt header
#include <QWidget>
#include <QItemDelegate>


class GenerixProject;

class BusinessViewListDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	BusinessViewListDelegate(QObject * parent);
	virtual ~BusinessViewListDelegate();

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class GenerixProjectDockImpl : public XinxDockWidget, public Ui::GenerixProjectDockWidget
{
	Q_OBJECT
public:
	GenerixProjectDockImpl(QWidget * parent = 0);
	virtual ~GenerixProjectDockImpl();

	void setProject(XinxProject::ProjectPtr project);
private slots:
	void updateList();
	void on_m_prefixCombo_activated(QString text);
	void editorChanged(int index);
private:
	QWeakPointer<GenerixProject> m_gnxProject;
	int m_editorIndex;
	BusinessViewListDelegate * m_delegate;
};

#endif // GENERIXPROJECTDOCK_H
