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

#ifndef XINXPROJECT_PROJECTLISTMODEL_H
#define XINXPROJECT_PROJECTLISTMODEL_H

#include <QStandardItemModel>
#include <QFileInfo>
#include <QDate>
#include <versioncontrol/rcs.h>
#include "xinxprojectproject.h"

class IconProjectProvider;

namespace XinxProject
{

class PrivateProjectListModel;
class ModelFileNode;

class ProjectListModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	enum FilterType
	{
		FILTER_NONE,
		FILTER_FILENAME,
		FILTER_SIZE,
		FILTER_DATE,
		FILTER_CONTAINS,
		FILTER_STATUS
	};

	enum FilterOperator
	{
		FILTER_GT,
		FILTER_GEQT,
		FILTER_EQ,
		FILTER_LEQT,
		FILTER_LT,
		FILTER_DIFF
	};

	ProjectListModel(QObject* parent = 0);
	virtual ~ProjectListModel();

	void setFileIconProvider(IconProjectProvider * provider);
	IconProjectProvider * fileIconProvider() const;

	void setLongDirectoryName(bool value);
	bool longDirectoryName() const;

	void setFilterType(enum FilterType type = ProjectListModel::FILTER_NONE);
	enum FilterType filterType() const;

	void setFilterFileName(const QString & value);
	QString filterFileName() const;

	void setFilterSize(enum FilterOperator op, qint64 size);
	enum FilterOperator filterOperator() const;
	qint64 filterSize() const;

	void setFilterDate(const QDate & value);
	const QDate & filterDate() const;

	void setFilterContains(const QString & text);
	QString filterContains() const;

	void setFilterState(enum RCS::rcsState state);
	enum RCS::rcsState filterState() const;

	void applyFilter();

	ProjectPtr fileProject(const QModelIndex & index) const;
	QIcon fileIcon(const QModelIndex & index) const;
	QFileInfo fileInfo(const QModelIndex & index) const;
	QString fileName(const QModelIndex & index) const;
	QString filePath(const QModelIndex & index) const;
	enum RCS::rcsState fileState(const QModelIndex & index) const;
	QString fileVersion(const QModelIndex & index) const;

	bool isDir(const QModelIndex & index) const;
	bool isProject(const QModelIndex & index) const;

	QModelIndex index(const QString& path, int column = 0) const;
	QModelIndex index(XinxProject::ProjectPtr project) const;
	virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;

	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual QModelIndex parent(const QModelIndex & index) const;
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

	virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const;
	virtual bool canFetchMore(const QModelIndex& parent) const;
	virtual void fetchMore(const QModelIndex& parent);
private:
	PrivateProjectListModel * d;
	friend class PrivateProjectListModel;
	friend class ModelFileNode;
};

}

#endif // XINXPROJECT_PROJECTLISTMODEL_H
