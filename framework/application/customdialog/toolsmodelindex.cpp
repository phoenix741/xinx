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
#include "toolsmodelindex.h"

// Qt header
#include <QDir>

/* ToolsModelIndex */

ToolsModelIndex::ToolsModelIndex(QHash<QString,QString> * tools, QObject * parent) : QAbstractTableModel(parent)
{
	m_tools = tools;
}

ToolsModelIndex::~ToolsModelIndex()
{

}

void ToolsModelIndex::setTools(QHash<QString,QString> * tools)
{
	emit layoutAboutToBeChanged();
	m_tools = tools;
	emit layoutChanged();
}

int ToolsModelIndex::rowCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent);

	return m_tools->count();
}

int ToolsModelIndex::columnCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent);

	return 2;
}

bool ToolsModelIndex::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (index.isValid() && (role == Qt::EditRole))
	{
		m_tools->insert(m_tools->keys().at(index.row()), value.toString());
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

QVariant ToolsModelIndex::data(const QModelIndex & index, int role) const
{
	if (! index.isValid()) return QVariant();
	if (role == Qt::DisplayRole)
	{
		QString key;
		switch (index.column())
		{
		case 0:
			key = m_tools->keys().at(index.row());
			return key.at(0).toUpper() + key.mid(1);
		case 1:
			return QDir::toNativeSeparators(m_tools->values().at(index.row()));
		}
	}
	else if (role == Qt::EditRole)
	{
		QString key;
		switch (index.column())
		{
		case 0:
			return m_tools->keys().at(index.row());
		case 1:
			return m_tools->values().at(index.row());
		}
	}

	return QVariant();
}

QVariant ToolsModelIndex::headerData(int section, Qt::Orientation orientation, int role) const
{
	if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
		switch (section)
		{
		case 0:
			return tr("Tool name");
		case 1:
			return tr("Tool path");
		}
	return QVariant();
}

Qt::ItemFlags ToolsModelIndex::flags(const QModelIndex & index) const
{
	if (index.isValid() && (index.column() == 1))
		return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
	return QAbstractTableModel::flags(index);
}
