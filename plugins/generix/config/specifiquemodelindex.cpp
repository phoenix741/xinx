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

// Xinx header
#include "specifiquemodelindex.h"
#include <directoryedit.h>
#include <plugins/xinxpluginsloader.h>

// Qt header
#include <QModelIndex>

/* SpecifiqueModelIndex */

SpecifiqueModelIndex::SpecifiqueModelIndex(QHash<QString,QString> * extentions, QObject * parent) : QAbstractTableModel(parent)
{
	m_extentions = extentions;

	foreach(IFileTypePlugin * plugin, XinxPluginsLoader::self()->fileTypes())
	{
		QStringList matchingValues = plugin->match().split(" ");
		QString value;

		if (matchingValues.size() && ! plugin->match().contains("*.xsl"))
		{
			QString matchingValue = matchingValues.at(0);
			value = matchingValue.remove("*.") + "/";
		}

		if (! m_extentions->contains(plugin->description()))
			m_extentions->insert(plugin->description(), extentions->value(plugin->description(), value));
	}
}

SpecifiqueModelIndex::~SpecifiqueModelIndex()
{

}

int SpecifiqueModelIndex::rowCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent);
	return m_extentions->count();
}

int SpecifiqueModelIndex::columnCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent);
	return 1;
}

QVariant SpecifiqueModelIndex::headerData(int section, Qt::Orientation orientation, int role) const
{
	if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
	{
		switch (section)
		{
		case 0 :
			return tr("Sub-directory");
		}
	}
	if ((orientation == Qt::Vertical) && (role == Qt::DisplayRole))
	{
		return m_extentions->keys().at(section);
	}
	return QVariant();
}

bool SpecifiqueModelIndex::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (index.isValid() && (role == Qt::EditRole))
	{
		QString key = m_extentions->keys().at(index.row());
		QString ext = m_extentions->value(key);
		switch (index.column())
		{
		case 1:
			ext = value.toString();
			break;
		}
		m_extentions->insert(key, ext);
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

QVariant SpecifiqueModelIndex::data(const QModelIndex & index, int role) const
{
	if (index.isValid() && ((role == Qt::DisplayRole) || (role == Qt::EditRole)))
	{
		switch (index.column())
		{
		case 0 :
			return m_extentions->values().at(index.row());
		}
	}

	return QVariant();
}

Qt::ItemFlags SpecifiqueModelIndex::flags(const QModelIndex & index) const
{
	if (index.isValid() && (index.column() >= 1))
		return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
	return QAbstractTableModel::flags(index);
}
