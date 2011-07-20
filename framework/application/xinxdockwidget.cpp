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

#include "xinxdockwidget.h"
#include <dtoolview.h>
#include <dviewbutton.h>

XinxDockWidget::XinxDockWidget(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f), _dock(0)
{
	connect(this, SIGNAL(dockChanged()), this, SLOT(updateDockTitle()));
	connect(this, SIGNAL(windowChanged()), this, SLOT(updateDockTitle()));
}

XinxDockWidget::~XinxDockWidget()
{

}

QString XinxDockWidget::windowTitle() const
{
	if (!_shortcut.isEmpty())
	{
		return QWidget::windowTitle() + QString(" (%1)").arg(_shortcut.toString());
	}
	return QWidget::windowTitle();
}

void XinxDockWidget::setDock(DToolView* dock)
{
	if (_dock != dock)
	{
		_dock = dock;

		emit dockChanged();
	}
}

DToolView* XinxDockWidget::dock()
{
	return _dock;
}

void XinxDockWidget::setShortcut(const QKeySequence & shortcut)
{
	if (_shortcut != shortcut)
	{
		_shortcut = shortcut;

		emit windowChanged();
	}
}

const QKeySequence & XinxDockWidget::getShortcut() const
{
	return _shortcut;
}

void XinxDockWidget::updateDockTitle()
{
	if (_dock && _dock->button())
	{
		_dock->button()->setText(windowTitle());
	}
}
