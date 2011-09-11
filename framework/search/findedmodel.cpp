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

#include "findedmodel.h"
#include "findedfile.h"
#include "findedline.h"
#include <core/xinxconfig.h>

/* Comparator */

bool fileFindedLessThan(FindedFile * file1, FindedFile * file2)
{
	Q_ASSERT_X(file1, "fileFindedLessThan", "File file1 can't be null");
	Q_ASSERT_X(file2, "fileFindedLessThan", "File file2 can't be null");

	return file1->filename() < file2->filename();
}

/* FindedModel */

FindedModel::FindedModel(QObject* parent): QAbstractItemModel(parent)
{

}

FindedModel::~FindedModel()
{

}

FindedFile * FindedModel::filesValue(const QString & filename) const
{
	int index = _filesIndex.value(filename, -1);
	if (index >= 0)
	{
		return _files.value(index);
	}

	return NULL;
}

void FindedModel::append(const QString& filename, int line, const QString& content)
{
	FindedFile * file = filesValue(filename);
	if (! file)
	{
		file = new FindedFile(filename);

		QList<FindedFile*>::iterator i = qBinaryFind(_files.begin(), _files.end(), file, fileFindedLessThan);
		int index = i - _files.begin();

		beginInsertRows(QModelIndex(), index, index);

		_files.insert(i, file);
		_filesIndex.insert(filename, index);

		endInsertRows();
	}

	beginInsertRows(index(file), file->findedLineSize(), file->findedLineSize());
	file->addFindedLine(new FindedLine(line, content));
	endInsertRows();
}

void FindedModel::addLine(const QString& filename, int ln)
{
	FindedFile * file = filesValue(filename);
	QModelIndex index1, index2;

	if (file)
	{
		foreach (FindedLine * line, *file)
		{
			if (line->line() >= ln)
			{
				line->setLine(line->line() + 1);

				index2 = index(file, line);
				if (!index1.isValid())
				{
					index2 = index1;
				}
			}
		}

		emit dataChanged(index1, index2);
	}
}

void FindedModel::removeLine(const QString& filename, int ln)
{
	FindedFile * file = filesValue(filename);
	QModelIndex index1, index2;

	if (file)
	{
		foreach (FindedLine * line, *file)
		{
			if (line->line() >= ln)
			{
				line->setLine(line->line() - 1);

				index2 = index(file, line);
				if (!index1.isValid())
				{
					index2 = index1;
				}
			}
		}

		emit dataChanged(index1, index2);
	}
}

void FindedModel::clear()
{
	beginRemoveRows(QModelIndex(), 0, _files.size());
	qDeleteAll(_files);
	_files.clear();
	_filesIndex.clear();
	endRemoveRows();
}

bool FindedModel::isIndexFile(const QModelIndex & index) const
{
	return index.isValid() && (index.internalId() == -1);
}

bool FindedModel::isIndexLine(const QModelIndex & index) const
{
	return index.isValid() && (index.internalId() >= 0);
}

int FindedModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return 1;
}

int FindedModel::rowCount(const QModelIndex& parent) const
{
	if (!parent.isValid())
	{
		return _files.size();
	}

	if (parent.internalId() == -1)
	{
		FindedFile * file = _files.at(parent.row());
		return file->findedLineSize();
	}

	return 0;
}

QModelIndex FindedModel::index(FindedFile* file) const
{
	return createIndex(_files.indexOf(file), 0, -1);
}

QModelIndex FindedModel::index(FindedFile* file, FindedLine* line) const
{
	return createIndex(file->indexOfFindedLine(line), 0, _files.indexOf(file));
}

QModelIndex FindedModel::index(int row, int column, const QModelIndex& parent) const
{
	if (! parent.isValid())
	{
		if ((row < 0) || (row >= _files.size())) return QModelIndex();

		return createIndex(row, column, -1);
	}

	FindedFile * file;
	if (parent.internalId() == -1)
	{
		file = _files.at(parent.row());

		if ((row < 0) || (row >= file->findedLineSize())) return QModelIndex();

		return createIndex(row, column, parent.row());
	}

	return QModelIndex();
}

QModelIndex FindedModel::parent(const QModelIndex& child) const
{
	if (! child.isValid())
	{
		return QModelIndex();
	}

	if (child.internalId() >= 0)
	{
		return createIndex(child.internalId(), child.column(), -1);
	}

	return QModelIndex();
}

QVariant FindedModel::data(const QModelIndex& index, int role) const
{
	if (index.isValid())
	{
		if (index.internalId() == -1) /* FindedFile */
		{
			FindedFile * file = _files.at(index.row());
			switch(role)
			{
			case Qt::DisplayRole:
				return file->filename();
				break;
			case Qt::ForegroundRole:
				return Qt::blue;
			case FilenameRole:
				return file->filename();
			case LineRole:
				return QVariant::fromValue(1);
			}
		}
		else /* FindedLine */
		{
			FindedLine * line = _files.at(index.internalId())->findedLineAt(index.row());
			switch(role)
			{
			case Qt::DisplayRole:
				return QString("%1: %2").arg(line->line(), 5).arg(line->content());
			case Qt::FontRole:
				return XINXConfig::self()->config().editor.defaultFormat;
			case FilenameRole:
				return _files.at(index.internalId())->filename();
			case LineRole:
				return line->line();
			case ContentRole:
				return line->content();
			}
		}
	}

	return QVariant();
}

