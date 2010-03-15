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

#ifndef WELCOMDLGIMPL_P_H
#define WELCOMDLGIMPL_P_H

// Qt header
#include <QListWidget>

class WelcomTreeWidget : public QListWidget
{
	Q_OBJECT
public:
	WelcomTreeWidget(QWidget *parent = 0);
	QListWidgetItem *addItem(const QString &label, const QString &data);

signals:
	void activated(const QString &data);

protected:
	virtual QSize minimumSizeHint() const;
	virtual QSize sizeHint() const;

private slots:
	void slotItemClicked(QListWidgetItem *item);

private:
	QIcon m_bullet;
};

#endif // WELCOMDLGIMPL_P_H
