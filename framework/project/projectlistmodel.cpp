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

#include "projectlistmodel_p.h"
#include "xinxprojectmanager.h"
#include "xinxprojectproject.h"
#include <plugins/xinxpluginsloader.h>
#include <jobs/xinxjobmanager.h>

// Qt header
#include <QDir>
#include <QDirIterator>

using namespace XinxProject;

/* Static member */

bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
	return s1.toLower() < s2.toLower();
}

/* DirectoryFetcher */

DirectoryFetcher::DirectoryFetcher()
{
	_matchedFileList = XinxPluginsLoader::self()->managedFilters();
}

DirectoryFetcher::~DirectoryFetcher()
{

}

void DirectoryFetcher::setDirectory(const QString & directory)
{
	_directory = directory;
}

const QString & DirectoryFetcher::directory() const
{
	return _directory;
}

void DirectoryFetcher::setProject(Project * project)
{
	_project = project;
}

Project * DirectoryFetcher::project() const
{
	return _project;
}

QString DirectoryFetcher::description() const
{
	return tr("Fetch files of %1").arg (_directory);
}

void DirectoryFetcher::startJob()
{
	emit setProgress (0, 0);

	QTime benchmark_time;
	benchmark_time.start();

	// Step 1 : Re-loading the directory.
	QDirIterator directory_information(QFileInfo(_directory).canonicalFilePath (), _matchedFileList, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);

	QFileInfoList infos;
	QStringList filenames;
	bool first_time = true;
	int i = 0;
	QTime _timer;
	_timer.start ();

	while(directory_information.hasNext ())
	{
		directory_information.next (); ++i;

		const QFileInfo info   = directory_information.fileInfo ();

		infos.append (info);
		filenames.append (info.canonicalFilePath ());

		if (((i > 100) && first_time) || (_timer.elapsed () > 200))
		{
			first_time = false;
			emit progressFetchedInformations(_directory, infos);
			infos.clear ();
			_timer.start ();
		}
	}

	if (!infos.isEmpty ())
	{
		emit progressFetchedInformations(_directory, infos);
	}

	emit allFetchedFiles (_directory, filenames);

	// Step 2 : Loading the RCS files
	if (_project && _project->rcsProxy() && _project->rcsProxy()->currentRCSInterface())
	{
		QList<RCS::struct_rcs_infos> infos = _project->rcsProxy()->currentRCSInterface()->infos(_directory);
		emit updateFiles(_directory, infos);
	}
}

/* ModelFileNode */

ModelFileNode::ModelFileNode(PrivateProjectListModel * p) : _private_model(p), _is_project(false), _project(0), _parent(0), _is_children_populated(false)
{
	_rcs_info.state = RCS::Unknown;
}

ModelFileNode::~ModelFileNode()
{
	qDeleteAll(_children.values());
}

bool ModelFileNode::isProject() const
{
	return _is_project;
}

bool ModelFileNode::isDirectory() const
{
	return _info.isDir();
}

bool ModelFileNode::isFile() const
{
	return _info.isFile();
}

QIcon ModelFileNode::icon() const
{
	if (_is_project)
	{
		return QIcon(":/images/project_open.png");
	}
	else if (_private_model->_provider)
	{
		return _private_model->_provider->icon(_info);
	}

	return QIcon(":/images/typeunknown.png");
}

QString ModelFileNode::displayText() const
{
	if (_is_project)
	{
		return QString("%1 [%2]").arg(_project->projectName()).arg(_project->projectPath());
	}
	else
	{
		if (_private_model->_long_directory_name)
		{
			return QDir(_project->projectPath ()).relativeFilePath (_info.absoluteFilePath ());
		}

		return _info.fileName();
	}
}

void ModelFileNode::add(XinxProject::Project * project)
{
	ModelFileNode * node = new ModelFileNode (_private_model);
	node->_is_project = true;
	node->_project    = project;
	node->_info       = QFileInfo(project->projectPath());
	add(node);
}

void ModelFileNode::add(const QFileInfo & information)
{
	ModelFileNode * node = new ModelFileNode (_private_model);
	node->_info			 = information;
	add(node);
}

void ModelFileNode::add(const RCS::struct_rcs_infos & information)
{
	ModelFileNode * node = new ModelFileNode (_private_model);
	node->_info     = QFileInfo(information.filename);
	node->_rcs_info = information;
	add(node);
}

void ModelFileNode::add(const QString & filename)
{
	ModelFileNode * node = new ModelFileNode(_private_model);
	node->_info			 = QFileInfo(filename);
	add(node);
}

void ModelFileNode::add(ModelFileNode * node)
{
	node->_filename = node->_info.canonicalFilePath();
	if (node->_filename.isEmpty ())
	{
		// Case of file doesn't exist.
		node->_filename = node->_info.fileName ();
	}
	node->_parent   = this;
	if (_project)
	{
		node->_project  = _project;
	}
	_children.insert(node->_filename, node);
}

ModelFileNode * ModelFileNode::remove(XinxProject::Project * project)
{
	return remove(project->projectPath());
}

ModelFileNode * ModelFileNode::remove(const QString & filename)
{
	ModelFileNode * node = _children.value(filename);
	node->_parent = 0;

	_children.remove(filename);
	return node;
}

void ModelFileNode::addVisibleChildren(const QString & filename)
{
	ModelFileNode * node = _children.value(filename);
	Q_ASSERT_X(node, "ModelFileNode::addVisibleChildren", "Node not in the children");

	QList<QString>::iterator it = qLowerBound(_visible_children.begin (), _visible_children.end (), filename);
	const int insert_row = it - _visible_children.begin ();

	_private_model->_model->beginInsertRows (_private_model->index(this), insert_row, insert_row);
	_visible_children.insert(it, node->_filename);
	_private_model->_model->endInsertRows ();
}

void ModelFileNode::removeVisibleChildren(const QString & filename)
{
	int row = _visible_children.indexOf(filename);
	if (row == -1) return;

	_private_model->_model->beginRemoveRows(_private_model->index(this), row, row);
	_visible_children.removeAt(row);
	_private_model->_model->endRemoveRows();
}

/* PrivateProjectListModel */

PrivateProjectListModel::PrivateProjectListModel(ProjectListModel* parent): QObject(parent), _model(parent), _selected_item(0), _provider(0), _long_directory_name(false)
{
	qRegisterMetaType<QFileInfoList>( "QFileInfoList" );
	qRegisterMetaType< QList<RCS::struct_rcs_infos> >( "QList<RCS::struct_rcs_infos>" );

	_root_node = new ModelFileNode(this);
	_root_node->_is_children_populated = false;
	_selected_font.setBold(true);

	_watcher = new QFileSystemWatcher(this);
	_fetcher = new DirectoryFetcher;

	_filter_type = ProjectListModel::FILTER_NONE;

	connect(XinxProject::Manager::self(), SIGNAL(selectionChanged(XinxProject::Project*)), this, SLOT(selectionChange(XinxProject::Project*)));
	connect(XinxProject::Manager::self(), SIGNAL(projectOpened(XinxProject::Project*)), this, SLOT(addProject(XinxProject::Project*)));
	connect(XinxProject::Manager::self(), SIGNAL(projectCustomized(XinxProject::Project*)), this, SLOT(updateProject(XinxProject::Project*)));
	connect(XinxProject::Manager::self(), SIGNAL(projectClosing(XinxProject::Project*)), this, SLOT(removeProject(XinxProject::Project*)));

	connect(_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(fetchPath(QString)));
}

PrivateProjectListModel::~PrivateProjectListModel()
{
	delete _root_node;
}

ModelFileNode * PrivateProjectListModel::node(ModelFileNode* currentNode, const QString& path)
{
	const QString asked_path = path + "/";

	foreach(const QString & children_path, currentNode->_children.keys ())
	{
		const QString children_path_test = children_path + "/";

		if (asked_path.contains(children_path_test))
		{
			ModelFileNode * child_node = currentNode->_children.value (children_path);
			if (asked_path == children_path_test)
			{
				return child_node;
			}
			else
			{
				return node(child_node, path);
			}
		}
	}

	return NULL;
}

QModelIndex PrivateProjectListModel::index (ModelFileNode * node) const
{
	ModelFileNode * parent_node = node->_parent;
	if (! parent_node) return QModelIndex();

	const QString filename = node->_filename;
	int row = parent_node->_visible_children.indexOf(filename);

	return _model->createIndex(row, 0, node);
}

bool PrivateProjectListModel::isNodeMustBeShow(ModelFileNode* node) const
{
	if (node->isProject()) return true;
	if (node->isDirectory())
	{
		// If an element is a directory we do not show it only if we are sure to not have file
		return ! (node->_is_children_populated && node->_visible_children.isEmpty ());
		//return true;
	}

	// else if (node->isFile())

	bool show_node = false;
	switch(_filter_type)
	{
	case ProjectListModel::FILTER_NONE:
		// If no filter we add the node
		show_node = true;
		break;
	case ProjectListModel::FILTER_FILENAME:
		return node->_filename.contains(_filter_filename);
		break;
	case ProjectListModel::FILTER_SIZE:
		switch (_filter_operator)
		{
		case ProjectListModel::FILTER_GT:
			show_node = node->_info.size() >  _filter_size;
			break;
		case ProjectListModel::FILTER_GEQT:
			show_node = node->_info.size() >= _filter_size;
			break;
		case ProjectListModel::FILTER_EQ:
			show_node = node->_info.size() == _filter_size;
			break;
		case ProjectListModel::FILTER_LEQT:
			show_node = node->_info.size() <= _filter_size;
			break;
		case ProjectListModel::FILTER_LT:
			show_node = node->_info.size() <  _filter_size;
			break;
		case ProjectListModel::FILTER_DIFF:
			show_node = node->_info.size() != _filter_size;
			break;
		};
		break;
	case ProjectListModel::FILTER_DATE:
		show_node = node->_info.lastModified().date() == _filter_date;
		break;
	case ProjectListModel::FILTER_CONTAINS:
		// Launch a search thread, the result must be updated later
		// So default is false
		break;
	case ProjectListModel::FILTER_STATUS:
		// If we have the information we use it, else when the thread will updating the thread
		// we use it.
		show_node = (node->_rcs_info.state == _filter_state);
		break;
	};
	return show_node;
}

void PrivateProjectListModel::addVisibleChildren(ModelFileNode * node, const QString & filename)
{
	if (node->_parent)
	{
		QStringList::const_iterator it = qBinaryFind(node->_parent->_visible_children.constBegin(), node->_parent->_visible_children.constEnd(), node->_filename);
		if (it == node->_parent->_visible_children.constEnd())
		{
			addVisibleChildren(node->_parent, node->_filename);
		}
	}

	node->addVisibleChildren(filename);
}

void PrivateProjectListModel::removeVisibleChildren(ModelFileNode * node, const QString & filename)
{
	node->removeVisibleChildren(filename);
	if (node->_parent && ! isNodeMustBeShow(node))
	{
		removeVisibleChildren(node->_parent, node->_filename);
	}
}

void PrivateProjectListModel::updateVisibleChildren(ModelFileNode * node, bool recursive)
{
	// If there is no node to show, we can leave no.
	if (!node->_children.size()) return;

	// We backup already visible node in a set.
	// It's not necessary to re-add a node already in the list.
	// A set is used to search quickly if the node exist.
	QSet<QString> files = QSet<QString>::fromList(node->_visible_children);
	QHashIterator<QString,ModelFileNode*> it(node->_children);
	while (it.hasNext())
	{
		it.next();

		const QString & key = it.key();
		bool show_node = isNodeMustBeShow(it.value());

		// If the node must be shown and is not shown we show it
		if (show_node && ! files.contains(key))
		{
			addVisibleChildren(node, key);
		}
		// else if the node must not be shown and the node is present, we hide it
		// in other case, there is nothing to do
		else if (!show_node && files.contains(key))
		{
			removeVisibleChildren(node, key);
			// In this case it's not necessary to hide recursively
		}

		// If recursive is true, we must propag the filter to child.
		// If the child is not populated, we don't care
		if (recursive && it.value()->_is_children_populated)
		{
			updateVisibleChildren(it.value(), true);
		}
	}
}

void PrivateProjectListModel::addProject(Project* project)
{
	_root_node->add(project);
	_root_node->addVisibleChildren(project->projectPath());
}

void PrivateProjectListModel::removeProject(Project* project)
{
	if (project == _selected_item)
	{
		_selected_item = 0;
	}
	_root_node->removeVisibleChildren(project->projectPath());
	delete _root_node->remove(project);
}

void PrivateProjectListModel::updateProject(Project* project)
{
	QModelIndex index = _model->index (project);
	if (! index.isValid ()) return;

	emit _model->dataChanged (index, index);
}

void PrivateProjectListModel::selectionChange(XinxProject::Project * project)
{
	if (_selected_item)
	{
		updateProject (_selected_item);
	}
	if (project)
	{
		_selected_item = project;
		updateProject (project);
	}
	else
	{
		_selected_item = 0;
	}
}

void PrivateProjectListModel::_updateFiles(const QString & directory, QList<RCS::struct_rcs_infos> files)
{
	ModelFileNode * node_path = node(_root_node, directory);

	foreach(const RCS::struct_rcs_infos & info, files)
	{
		if (node_path->_children.contains(info.filename))
		{
			node_path->_children.value(info.filename)->_rcs_info = info;
		}
		else
		{
			// In this case the file exist in Version Control but not in the project list.
			// We add it if the file match managed file;

			QStringList managed_files = XinxPluginsLoader::self()->managedFilters();
			foreach(const QString & filetype, managed_files)
			{
				QRegExp expr(filetype, Qt::CaseSensitive, QRegExp::WildcardUnix);
				if (expr.exactMatch (info.filename))
				{
					node_path->add (info);
					break;
				}
			}
		}
	}

	if (node_path->_visible_children.size ())
	{
		const QString first_file_name = node_path->_visible_children.first ();
		const QString last_file_name  = node_path->_visible_children.last ();

		ModelFileNode * first_node = node_path->_children.value (first_file_name);
		ModelFileNode * last_node  = node_path->_children.value (last_file_name);

		emit _model->dataChanged(index(first_node), index(last_node));
	}

	// We don't update recursively, this will be made when necessary
	updateVisibleChildren(node_path, false);
}

void PrivateProjectListModel::emitUpdate(ModelFileNode * node)
{
	Q_ASSERT_X(node, "PrivateProjectListModel::emitUpdate", "Node cannot be null");

	if (node->_visible_children.size ())
	{
		const QString first_file_name = node->_visible_children.first ();
		const QString last_file_name  = node->_visible_children.last ();

		ModelFileNode * first_node = node->_children.value (first_file_name);
		ModelFileNode * last_node  = node->_children.value (last_file_name);

		emit _model->dataChanged(index(first_node), index(last_node));

		foreach(const QString & path, node->_visible_children)
		{
			emitUpdate (node->_children.value (path));
		}
	}
}

void PrivateProjectListModel::_progressFetchedInformations(const QString & directory, QFileInfoList filenames)
{
	ModelFileNode * node_path = node(_root_node, directory);

	if (! node_path)
	{
		qWarning() << tr("Node for directory %1 not found").arg (directory);
		return;
	}

	foreach(const QFileInfo & info, filenames)
	{
		if (node_path->_children.contains(info.canonicalFilePath()))
		{
			node_path->_children.value(info.canonicalFilePath())->_info = info;
		}
		else
		{
			node_path->add (info);
		}
	}

	// We don't update recursively, this will be made when necessary
	updateVisibleChildren(node_path, false);
}

void PrivateProjectListModel::_allFetchedFiles(const QString & directory, QStringList files)
{
	ModelFileNode * node_path = node(_root_node, directory);

	// If after all adds, there is no more files, we remove the directory
	if (node_path->_parent && !isNodeMustBeShow(node_path))
	{
		removeVisibleChildren(node_path->_parent, node_path->_filename);
	}

	QSet<QString> to_delete_files = QSet<QString>::fromList(node_path->_children.keys());

	foreach(const QString & filename, files)
	{
		if (to_delete_files.contains(filename))
		{
			to_delete_files.remove(filename);
		}
	}

	foreach (const QString & filename, to_delete_files.values())
	{
		removeVisibleChildren(node_path, filename);
		node_path->remove(filename);
	}
}

void PrivateProjectListModel::fetchNode(ModelFileNode * node)
{
	QFileInfo & info = node->_info;
	if (! info.isDir()) return;

	if (! _watcher->directories().contains(info.canonicalFilePath()))
	{
		_watcher->addPath(info.canonicalFilePath());
	}

	DirectoryFetcher * fetcher = new DirectoryFetcher;
	fetcher->setDirectory (info.canonicalFilePath ());
	fetcher->setProject (node->_project);

	connect(fetcher, SIGNAL(allFetchedFiles(QString,QStringList)), this, SLOT(_allFetchedFiles(QString,QStringList)));
	connect(fetcher, SIGNAL(progressFetchedInformations(QString,QFileInfoList)), this, SLOT(_progressFetchedInformations(QString,QFileInfoList)));
	connect(fetcher, SIGNAL(updateFiles(QString,QList<RCS::struct_rcs_infos>)), this, SLOT(_updateFiles(QString,QList<RCS::struct_rcs_infos>)));

	XinxJobManager::self ()->addJob(fetcher);
}

void PrivateProjectListModel::fetchPath(const QString & path)
{
	ModelFileNode * node_path = node(_root_node, path);
	if (node_path)
	{
		fetchNode(node_path);
	}
}


QString PrivateProjectListModel::rcsStateToString(RCS::rcsState state)
{
	if (state == RCS::Unknown)
		return tr("Unknown");
	if (state == RCS::LocallyModified)
		return tr("Locally modified");
	if (state == RCS::LocallyAdded)
		return tr("Locally added");
	if (state == RCS::LocallyRemoved)
		return tr("Locally removed");
	if ((state == RCS::UnresolvedConflict) || (state == RCS::FileHadConflictsOnMerge))
		return tr("Has conflict");
	if (state == RCS::NeedsCheckout)
		return tr("No modified") + ", " + tr("Need checkout");
	return tr("No modified");
}


/* ProjectListModel */

ProjectListModel::ProjectListModel(QObject* parent): QAbstractItemModel(parent)
{
	d = new PrivateProjectListModel(this);
}

ProjectListModel::~ProjectListModel()
{

}

void ProjectListModel::setFileIconProvider(IconProjectProvider* provider)
{
	d->_provider = provider;
}

IconProjectProvider* ProjectListModel::fileIconProvider() const
{
	return d->_provider;
}

void ProjectListModel::setLongDirectoryName(bool value)
{
	if (d->_long_directory_name != value)
	{
		d->_long_directory_name = value;
		d->emitUpdate(d->_root_node);
	}
}

bool ProjectListModel::longDirectoryName() const
{
	return d->_long_directory_name;
}

void ProjectListModel::applyFilter()
{
	// We update all children fetched with the new filter
	d->updateVisibleChildren(d->_root_node, true);
}

void ProjectListModel::setFilterType(ProjectListModel::FilterType type)
{
	d->_filter_type = type;
}

ProjectListModel::FilterType ProjectListModel::filterType() const
{
	return d->_filter_type;
}

void ProjectListModel::setFilterFileName(const QString& value)
{
	d->_filter_filename = QRegExp(value);
	d->_filter_filename.setPatternSyntax(QRegExp::WildcardUnix);
}

QString ProjectListModel::filterFileName() const
{
	return d->_filter_filename.pattern();
}

void ProjectListModel::setFilterSize(ProjectListModel::FilterOperator op, qint64 size)
{
	d->_filter_operator = op;
	d->_filter_size = size;
}

ProjectListModel::FilterOperator ProjectListModel::filterOperator() const
{
	return d->_filter_operator;
}

qint64 ProjectListModel::filterSize() const
{
	return d->_filter_size;
}

void ProjectListModel::setFilterDate(const QDate& value)
{
	d->_filter_date = value;
}

const QDate& ProjectListModel::filterDate() const
{
	return d->_filter_date;
}

void ProjectListModel::setFilterContains(const QString& text)
{
	d->_filter_contains = QRegExp(text);
}

QString ProjectListModel::filterContains() const
{
	return d->_filter_contains.pattern();
}

void ProjectListModel::setFilterState(RCS::rcsState state)
{
	d->_filter_state = state;
}

RCS::rcsState ProjectListModel::filterState() const
{
	return d->_filter_state;
}

QIcon ProjectListModel::fileIcon(const QModelIndex& index) const
{
	if (! index.isValid()) return QIcon();
	ModelFileNode* node = static_cast<ModelFileNode*> (index.internalPointer());
	if (!node) return QIcon();
	return node->icon();
}

QFileInfo ProjectListModel::fileInfo(const QModelIndex& index) const
{
	if (! index.isValid()) return QFileInfo();
	ModelFileNode* node = static_cast<ModelFileNode*> (index.internalPointer());
	if (!node) return QFileInfo();
	return node->_info;
}

QString ProjectListModel::fileName(const QModelIndex& index) const
{
	if (! index.isValid()) return QString();
	ModelFileNode* node = static_cast<ModelFileNode*> (index.internalPointer());
	if (!node) return QString();
	return node->_info.fileName();
}

QString ProjectListModel::filePath(const QModelIndex& index) const
{
	if (! index.isValid()) return QString();
	ModelFileNode* node = static_cast<ModelFileNode*> (index.internalPointer());
	if (!node) return QString();
	return node->_info.canonicalFilePath();
}

Project* ProjectListModel::fileProject(const QModelIndex& index) const
{
	if (! index.isValid()) return NULL;
	ModelFileNode* node = static_cast<ModelFileNode*> (index.internalPointer());
	if (!node) return NULL;
	return node->_project;
}

enum RCS::rcsState ProjectListModel::fileState(const QModelIndex& index) const
{
	if (! index.isValid()) return RCS::Unknown;
	ModelFileNode* node = static_cast<ModelFileNode*> (index.internalPointer());
	if (!node) return RCS::Unknown;
	return node->_rcs_info.state;
}

QString ProjectListModel::fileVersion(const QModelIndex& index) const
{
	if (! index.isValid()) return QString();
	ModelFileNode* node = static_cast<ModelFileNode*> (index.internalPointer());
	if (!node) return QString();
	return node->_rcs_info.version;
}

bool ProjectListModel::isDir(const QModelIndex & index) const
{
	return fileInfo(index).isDir();
}

bool ProjectListModel::isProject(const QModelIndex & index) const
{
	if (! index.isValid()) return false;
	ModelFileNode* node = static_cast<ModelFileNode*> (index.internalPointer());
	if (!node) return false;
	return node->isProject();
}

int ProjectListModel::columnCount (const QModelIndex & parent) const
{
	return 1;
}

QModelIndex ProjectListModel::index(const QString& path, int column) const
{
	if (column > 0) return QModelIndex();

	return (d->index(d->node(d->_root_node, QFileInfo(path).canonicalFilePath())));
}

QModelIndex ProjectListModel::index (XinxProject::Project * project) const
{
	if (d->_root_node->_children.contains (project->projectPath()))
	{
		ModelFileNode * node = d->_root_node->_children.value (project->projectPath());
		return createIndex(d->_root_node->_visible_children.indexOf(project->projectPath()), 0, node);
	}
	else
	{
		return QModelIndex();
	}
}

QModelIndex ProjectListModel::index (int row, int column, const QModelIndex & parent) const
{
	ModelFileNode * node = d->_root_node;
	if (parent.isValid())
	{
		node = static_cast<ModelFileNode*>(parent.internalPointer());
	}

	if (column != 0) return QModelIndex();
	if ((row < 0) || (row >= node->_visible_children.size ())) return QModelIndex();

	const QString & node_name = node->_visible_children.at(row);
	ModelFileNode * visible_node = node->_children.value(node_name);

	return createIndex (row, column, visible_node);
}

QModelIndex ProjectListModel::parent (const QModelIndex & index) const
{
	if (index.isValid ())
	{
		ModelFileNode * node = static_cast<ModelFileNode*>(index.internalPointer());
		if ((!node) || (node == d->_root_node))
		{
			return QModelIndex();
		}
		ModelFileNode * parentNode = node->_parent;
		if ((!parentNode) || (parentNode == d->_root_node) || (!parentNode->_parent))
		{
			return QModelIndex();
		}
		const QString filename = parentNode->_filename;
		int index = parentNode->_parent->_visible_children.indexOf(filename);

		return createIndex(index, 0, parentNode);
	}
	return QModelIndex();
}

int ProjectListModel::rowCount (const QModelIndex & parent) const
{
	ModelFileNode * node = d->_root_node;
	if (parent.isValid ())
	{
		node = static_cast<ModelFileNode*>(parent.internalPointer());
	}

	return node->_visible_children.size();
}

QVariant ProjectListModel::data (const QModelIndex & index, int role) const
{
	if (!index.isValid ()) return QVariant();
	if (index.column () != 0) return QVariant();

	ModelFileNode * node = static_cast<ModelFileNode*>(index.internalPointer());
	if (! node) return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return node->displayText();
			break;
		case Qt::FontRole:
			if (node->isProject() && node->_project == d->_selected_item)
			{
				return d->_selected_font;
			}
			break;
		case Qt::DecorationRole:
			return node->icon();
		case Qt::ToolTipRole:
			{
				const QString filename		= node->_info.fileName();
				const QString filedate		= node->_info.lastModified().toString(Qt::SystemLocaleShortDate);
				const QString date			= node->_rcs_info.rcsDate.toString(Qt::SystemLocaleShortDate);
				const QString version		= node->_rcs_info.version;
				const QString status 		= d->rcsStateToString(node->_rcs_info.state);

				const QString rcs_string	= node->_rcs_info.filename.isEmpty() ? QString() : tr("\nDate in Version Control : %1\nStatus : %2\nVersion : %3").arg(date).arg(status).arg(version);
				const QString tips = tr("Filename : %1\nDate of file : %2%3").arg(filename).arg(filedate).arg(rcs_string);

				return tips;
			}
		case Qt::BackgroundRole:
			if (node->isProject() || node->_project->projectRCS().isEmpty())
			{
				return QVariant();
			}

			switch(node->_rcs_info.state)
			{
			case RCS::Unknown:
				return QBrush(Qt::gray);
				break;
			case RCS::NeedPatch:
			case RCS::NeedsCheckout:
				return QBrush(Qt::cyan);
				break;
			case RCS::LocallyModified:
				return QBrush(Qt::yellow);
				break;
			case RCS::LocallyRemoved:
			case RCS::LocallyAdded:
				return QBrush(Qt::green);
				break;
			case RCS::UnresolvedConflict:
			case RCS::FileHadConflictsOnMerge:
				return QBrush(Qt::red);
				break;
			case RCS::Updated:
				return QVariant();
			}
		}
	return QVariant();
}

bool ProjectListModel::hasChildren(const QModelIndex& parent) const
{
	ModelFileNode * node = d->_root_node;

	if (! parent.isValid())
	{
		return node->_visible_children.size();
	}
	else if (parent.column() != 0)
	{
		return false;
	}

	// Parent is valid
	node = static_cast<ModelFileNode*>(parent.internalPointer());
	return node->isDirectory();
}

bool ProjectListModel::canFetchMore(const QModelIndex& parent) const
{
    ModelFileNode * node = d->_root_node;
	if (parent.isValid())
	{
		node = static_cast<ModelFileNode*>(parent.internalPointer());
	}

	return node->isDirectory() && (! node->_is_children_populated);
}

void ProjectListModel::fetchMore(const QModelIndex& parent)
{
	ModelFileNode * node = d->_root_node;
	if (parent.isValid())
	{
		node = static_cast<ModelFileNode*>(parent.internalPointer());
	}

	d->fetchNode(node);

	node->_is_children_populated = true;
}


