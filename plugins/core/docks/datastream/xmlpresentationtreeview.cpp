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

#include "xmlpresentationtreeview.h"
#include <QMimeData>
#include <QDrag>

XmlPresentationTreeView::XmlPresentationTreeView(QWidget* parent) : QTreeView(parent)
{

}

XmlPresentationTreeView::~XmlPresentationTreeView()
{

}

void XmlPresentationTreeView::startDrag(Qt::DropActions supportedActions)
{
	QModelIndexList indexes = selectedIndexes();
	for(int i = indexes.count() - 1 ; i >= 0; --i)
	{
		if (! model()->flags(indexes.at(i)) & Qt::ItemIsDragEnabled)
		{
			indexes.removeAt(i);
		}
	}

	if (indexes.count() > 0)
	{
		QMimeData *data = model()->mimeData(indexes);
		if (!data)
			return;
		QRect rect;

		QDrag *drag = new QDrag(this);
		drag->setMimeData(data);

		drag->exec(supportedActions, Qt::IgnoreAction);
	}
}

