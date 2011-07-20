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

#ifndef XINXDOCKWIDGET_H
#define XINXDOCKWIDGET_H

#include <core/lib-config.h>
#include <QWidget>

class DToolView;

class LIBEXPORT XinxDockWidget : public QWidget
{
	Q_OBJECT
public:
	explicit XinxDockWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~XinxDockWidget();

	void setShortcut(const QKeySequence & shortcut);
	const QKeySequence & getShortcut() const;

	void setDock(DToolView * dock);
	DToolView * dock();

	virtual QString windowTitle() const;
signals:
	void dockChanged();
	void windowChanged();
private slots:
	void updateDockTitle();
private:
	DToolView * _dock;
	QKeySequence _shortcut;
};

#endif // XINXDOCKWIDGET_H
