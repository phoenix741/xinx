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

#ifndef _PRIVATEPROJECTLISTMODEL_
#define _PRIVATEPROJECTLISTMODEL_

#include "projectlistmodel.h"
#include <QObject>
#include <QFileInfo>
#include "iconprojectprovider.h"
#include "xinxprojectproject.h"
#include <jobs/xinxjob.h>
#include <QMutex>

namespace XinxProject
{

class PrivateProjectListModel;

class DirectoryFetcher : public XinxJob
{
	Q_OBJECT
public:
	DirectoryFetcher();
	~DirectoryFetcher();

	void setDirectory(const QString & directory);
	const QString & directory() const;

	void setProject(Project * project);
	Project * project() const;

	virtual QString description() const;
signals:
	void progressFetchedInformations(const QString & directory, QFileInfoList filenames);
	void allFetchedFiles(const QString & directory, QStringList files);
	void updateFiles(const QString & directory, QList<RCS::struct_rcs_infos> files);
protected:
	virtual void startJob();
private:
	QString _directory;
	QStringList _matchedFileList;
	XinxProject::Project * _project;
};

class ModelFileNode
{
public:
	inline ModelFileNode(PrivateProjectListModel * p);
	inline ~ModelFileNode();

	inline bool isProject() const;
	inline bool isDirectory() const;
	inline bool isFile() const;

	inline QIcon icon() const;
	inline QString displayText() const;

	inline void add(XinxProject::Project * project);
	inline void add(const QFileInfo & information);
	inline void add(const RCS::struct_rcs_infos & information);
	inline void add(const QString & filename);
	inline void add(ModelFileNode * node);

	inline ModelFileNode * remove(XinxProject::Project * project);
	inline ModelFileNode * remove(const QString & filename);

	inline void addVisibleChildren(const QString & filename);
	inline void removeVisibleChildren(const QString & filename);

	PrivateProjectListModel * _private_model;

	bool _is_project;
	XinxProject::Project * _project;
	QFileInfo _info;
	RCS::struct_rcs_infos _rcs_info;
	QString _filename;

	ModelFileNode * _parent;
	QHash<QString,ModelFileNode*> _children;
	QStringList _visible_children;

	bool _is_children_populated;
};

class PrivateProjectListModel : public QObject
{
	Q_OBJECT
public:
	PrivateProjectListModel(ProjectListModel * parent = 0);
	~PrivateProjectListModel();

	void updateVisibleChildren(XinxProject::ModelFileNode* node, bool recursive);
	bool isNodeMustBeShow(ModelFileNode* node) const;

	ModelFileNode * node(ModelFileNode * currentNode, const QString & path);
	QModelIndex index (ModelFileNode * node) const;

	void addVisibleChildren(XinxProject::ModelFileNode* node, const QString& filename);
	void removeVisibleChildren(ModelFileNode * node, const QString & filename);

	ProjectListModel * _model;
	ModelFileNode * _root_node;
	Project * _selected_item;
	IconProjectProvider * _provider;
	bool _long_directory_name;

	QFileSystemWatcher * _watcher;
	DirectoryFetcher * _fetcher;

	QFont _selected_font;

	QRegExp _filter_filename;
	qint64 _filter_size;
	ProjectListModel::FilterOperator _filter_operator;
	QDate _filter_date;
	QRegExp _filter_contains;
	RCS::rcsState _filter_state;
	enum ProjectListModel::FilterType _filter_type;

	static QString rcsStateToString(RCS::rcsState state);

	void emitUpdate(ModelFileNode * node);
public slots:
	void _progressFetchedInformations(const QString & directory, QFileInfoList filenames);
	void _allFetchedFiles(const QString & directory, QStringList files);
	void _updateFiles(const QString & directory, QList<RCS::struct_rcs_infos> files);

	void fetchNode(ModelFileNode * node);
	void fetchPath(const QString & path);

	void addProject(XinxProject::Project * project);
	void removeProject(XinxProject::Project * project);
	void updateProject(XinxProject::Project * project);
	void selectionChange(XinxProject::Project * project);
private:

	friend class ModelFileNode;
};

}

#endif // _PRIVATEPROJECTLISTMODEL_
