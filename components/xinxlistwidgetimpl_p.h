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

#pragma once
#ifndef XINXLISTWIDGETIMPL_P_H_
#define XINXLISTWIDGETIMPL_P_H_

// Xinx header
#include "ui_xinxlistwidget.h"
#include "xinxlistwidgetimpl.h"
#include <components-config.h>

class XinxListWidgetImplPrivate : public QObject
{
	Q_OBJECT
public:
	XinxListWidgetImpl * _parent;
	int m_defaultValue;
	QString m_defaultProposedValue;
	QScopedPointer<Ui::XinxListWidget> _ui;

	XinxListWidgetImplPrivate(XinxListWidgetImpl* parent);

	void updateDefault(const QString & def);
public slots:
	void on_m_btnDef_clicked();
	void on_m_btnAdd_clicked();
	void on_m_btnDel_clicked();
	void on_m_btnUp_clicked();
	void on_m_btnDown_clicked();
	void on_m_list_currentRowChanged(int row);
	void on_m_list_itemChanged(QListWidgetItem * item);
};

#endif /* XINXLISTWIDGETIMPL_P_H_ */
