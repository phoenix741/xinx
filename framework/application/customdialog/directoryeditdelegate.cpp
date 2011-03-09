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
#include "directoryeditdelegate.h"
#include <directoryeditwidget.h>
#include <directoryedit.h>

// Qt header
#include <QLayout>
#include <QPainter>
#include <QDir>

/* DirectoryEditDelegate */

DirectoryEditDelegate::DirectoryEditDelegate(QObject *parent) : QItemDelegate(parent)
{
}

QWidget * DirectoryEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(option);
	Q_UNUSED(index);
	DirectoryEditWidget * editor = new DirectoryEditWidget(parent);
	editor->setDirectory(false);
	editor->layout()->setSpacing(0);
	return editor;
}

void DirectoryEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString value = index.model()->data(index, Qt::EditRole).toString();
	DirectoryEdit * directoryEdit = qobject_cast<DirectoryEditWidget*>(editor)->lineEdit();
	directoryEdit->setText(QDir::toNativeSeparators(value));
	directoryEdit->setFocus();
}

void DirectoryEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	DirectoryEdit * directoryEdit = qobject_cast<DirectoryEditWidget*>(editor)->lineEdit();
	QString value = directoryEdit->text();

	model->setData(index, QDir::fromNativeSeparators(value), Qt::EditRole);
}

void DirectoryEditDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(index);
	editor->setGeometry(option.rect);
}

void DirectoryEditDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() == 1)
	{
		QString value = index.model()->data(index, Qt::DisplayRole).toString();

		painter->save();
		QStyleOptionViewItem o = option;
		if (! QFile(value).exists())
			o.palette.setColor(QPalette::Text, Qt::red);
		drawDisplay(painter, o, option.rect, value);
		painter->restore();
	}
	else
		QItemDelegate::paint(painter, option, index);
}
