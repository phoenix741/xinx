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

#ifndef TOOLSMODELINDEX_H_
#define TOOLSMODELINDEX_H_
#pragma once

// Qt header
#include <QAbstractTableModel>
#include <QHash>

class ToolsModelIndex : public QAbstractTableModel
{
	Q_OBJECT
public:
	ToolsModelIndex(QHash<QString,QString> * tools, QObject * parent = 0);
	virtual ~ToolsModelIndex();

	void setTools(QHash<QString,QString> * tools);

	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual Qt::ItemFlags flags(const QModelIndex & index) const;
private:
	QHash<QString,QString> * m_tools;
};

#endif /* TOOLSMODELINDEX_H_ */
