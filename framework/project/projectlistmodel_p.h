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
#include <QFileSystemWatcher>
#include <QStack>

namespace XinxProject
{

class PrivateProjectListModel;

//! \private
class DirectoryFetcher : public XinxJob
{
	Q_OBJECT
public:
	DirectoryFetcher();
	~DirectoryFetcher();

	virtual bool isUnique() const;
	virtual QString uniqKey() const;

	void setModelDirectory(const QString & directory);
	const QString & modelDirectory() const;

	void setListingDirectory(const QString & directory);
	const QString & listingDirectory() const;

	void setProject(ProjectPtr project);
	ProjectPtr project() const;

	void setRetrieveRcsInfos(bool value);
	bool isRetrieveRcsInfos() const;

	virtual QString description() const;
signals:
	void progressFetchedInformations(const QString & directory, QFileInfoList filenames);
	void allFetchedFiles(const QString & directory, QStringList files);
	void updateFiles(const QString & directory, QList<RCS::struct_rcs_infos> files);
protected:
	virtual void startJob();
private:
	bool _retrieveRcsInfos;
	QString _listingDirectory, _modelDirectory;
	QStringList _matchedFileList;
	XinxProject::ProjectPtr _project;
};

//! \private
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

	inline ModelFileNode * add(XinxProject::ProjectPtr project);
	inline ModelFileNode * add(const QFileInfo & information);
	inline ModelFileNode * add(const RCS::struct_rcs_infos & information);
	inline ModelFileNode * add(const QString & filename);
	inline void add(ModelFileNode * node);

	inline ModelFileNode * remove(XinxProject::ProjectPtr project);
	inline ModelFileNode * remove(const QString & key);

	inline void addVisibleChildren(const QString & key);
	inline void removeVisibleChildren(const QString & key);

	QString modelDirectory() const;

	PrivateProjectListModel * _private_model;

	bool _is_project, _is_linked_path;
	XinxProject::ProjectPtrWeak _project;
	QFileInfo _info;
	RCS::struct_rcs_infos _rcs_info;
	QString _filename, _key;

	ModelFileNode * _parent;
	QHash<QString,ModelFileNode*> _children;
	QStringList _visible_children;

	bool _is_children_populated;
};

//! \private
class PrivateProjectListModel : public QObject
{
	Q_OBJECT
public:
	PrivateProjectListModel(ProjectListModel * parent = 0);
	~PrivateProjectListModel();

	void updateVisibleChildren(XinxProject::ModelFileNode* node, bool recursive);
	bool isNodeMustBeShow(ModelFileNode* node) const;

	ModelFileNode * node(ModelFileNode * currentNode, const QString & path);
	QModelIndex index(ModelFileNode * node) const;

	void addVisibleChildren(XinxProject::ModelFileNode* node, const QString& key);
	void removeVisibleChildren(ModelFileNode * node, const QString & key);

	ProjectListModel * _model;
	ModelFileNode * _root_node;
	ProjectPtrWeak _selected_item;
	IconProjectProvider * _provider;
	bool _long_directory_name;

	QMultiHash<QString,QString> _watcherDirectory;
	QFileSystemWatcher * _watcher;

	QScopedPointer<QTimer> _changePathTimer;
	QStack<QString> _changedPath;

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
	void _updateState(const QString & path, RCS::struct_rcs_infos info);

	void fetchNode(ModelFileNode * node);
	void fetchPath(const QString & path);
	void wantFetchPath(const QString & path);

	void fetchPathTimeout();

	void addProject(XinxProject::ProjectPtr project);
	void removeProject(XinxProject::ProjectPtr project);
	void updateProject(XinxProject::ProjectPtr project);
	void selectionChange(XinxProject::ProjectPtr project);

	void updateLinkedDirectories(XinxProject::ProjectPtr project);
private:

	friend class ModelFileNode;
};

}

#endif // _PRIVATEPROJECTLISTMODEL_
