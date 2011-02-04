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

#include "projectdirectorywidgetimpl_p.h"
#include "xinxprojectmanager.h"
#include <versioncontrol/versioncontrolmanager.h>
#include <QTimer>
#include <core/xinxconfig.h>
#include "xinxprojectproject.h"
#include <QMenu>
#include <QClipboard>
#include <editors/editormanager.h>
#include <QTemporaryFile>
#include <QProcess>
#include <QInputDialog>
#include <QMessageBox>
#include <editors/newfilewizardimpl.h>

/* PrivateProjectDirectoryWidgetImpl */

PrivateProjectDirectoryWidgetImpl::PrivateProjectDirectoryWidgetImpl(ProjectDirectoryWidgetImpl* parent) : QObject(parent), _parent(parent)
{
	_refresh_model_timer = new QTimer(this);
	_refresh_model_timer->setSingleShot(true);
	connect(_refresh_model_timer, SIGNAL(timeout()), this, SLOT(filterTimeout()));

	_create_directory_action			= new QAction(QIcon(":/images/folder.png"), tr("C&reate directory"), _parent);
	connect(_create_directory_action, SIGNAL(triggered()), this, SLOT(createDirectory()));
	_new_file_action					= new QAction(QIcon(":/images/filenew.png"), tr("&Create file ..."), _parent);
	connect(_new_file_action, SIGNAL(triggered()), this, SLOT(newFile()));
	_open_files_action					= new QAction(QIcon(":/images/fileopen.png"), tr("&Open"), _parent);
	connect(_open_files_action, SIGNAL(triggered()), this, SLOT(openFiles()));
	_remove_files_action				= new QAction(QIcon(":/images/editdelete.png"), tr("Remo&ve"), _parent);
	connect(_remove_files_action, SIGNAL(triggered()), this, SLOT(removeFiles()));
	_rename_file_action					= new QAction(tr("Rename"), _parent);
	connect(_rename_file_action, SIGNAL(triggered()), this, SLOT(renameFiles()));

	_copy_filename_action 				= new QAction(tr("&Copy filename to Clipboard"), _parent);
	connect(_copy_filename_action, SIGNAL(triggered()), this, SLOT(copyFileNameTriggered()));
	_copy_pathname_action 				= new QAction(tr("Co&py path to clipboard"), _parent);
	connect(_copy_pathname_action, SIGNAL(triggered()), this, SLOT(copyPathNameTriggered()));

	_set_project_as_default_action 		= new QAction(tr("Set the project as &default"), this);
	connect(_set_project_as_default_action, SIGNAL(triggered()), this, SLOT(setProjectAsDefaultTriggered()));
	_project_property_action			= new QAction(tr("Pro&ject Property"), this);
	connect(_project_property_action, SIGNAL(triggered()), this, SLOT(projectPropertyTriggered()));

	_compare_with_workingcopy_action 	= new QAction(QIcon(":/images/vcs_diff.png"), tr("Compare with the version management"), this);
	connect(_compare_with_workingcopy_action, SIGNAL(triggered()), this, SLOT(compareWithVersionControlTriggered()));
	_compare_action 					= new QAction(tr("Co&mpare files"), this);
	connect(_compare_action, SIGNAL(triggered()), this, SLOT(compareFilesTriggered()));

	_update_action						= new QAction(QIcon(":/images/vcs_update.png"), tr("Update project"), this);
	connect(_update_action, SIGNAL(triggered()), this, SLOT(updateFromVersionControlTriggered()));
	_commit_action						= new QAction(QIcon(":/images/vcs_commit.png"), tr("Commit project"), this);
	connect(_commit_action, SIGNAL(triggered()), this, SLOT(commitToVersionControlTriggered()));
	_add_action 						= new QAction(QIcon(":/images/vcs_add.png"), tr("Add file(s) to project"), this);
	connect(_add_action, SIGNAL(triggered()), this, SLOT(addToVersionControlTriggered()));
	_remove_action						= new QAction(QIcon(":/images/vcs_remove.png"), tr("Delete file(s) from project"), this);
	connect(_remove_action, SIGNAL(triggered()), this, SLOT(removeFromVersionControlTriggered()));
	_revert_action						= new QAction(tr("Revert file"), this);
	connect(_revert_action, SIGNAL(triggered()), this, SLOT(revertFileTriggered()));

	_popup_menu = new QMenu(_parent->_directory_view);
	_popup_menu->addAction(_set_project_as_default_action);
	_popup_menu->addAction(_project_property_action);
	_popup_menu->addSeparator();
	_popup_menu->addAction(_create_directory_action);
	_popup_menu->addAction(_new_file_action);
	_popup_menu->addAction(_open_files_action);
	_popup_menu->addAction(_remove_files_action);
	_popup_menu->addAction(_rename_file_action);
	_popup_menu->addSeparator();
	_popup_menu->addAction(_compare_action);
	_popup_menu->addSeparator();
	_popup_menu->addAction(_compare_with_workingcopy_action);
	_popup_menu->addSeparator();
	_popup_menu->addAction(_update_action);
	_popup_menu->addAction(_commit_action);
	_popup_menu->addSeparator();
	_popup_menu->addAction(_add_action);
	_popup_menu->addAction(_remove_action);
	_popup_menu->addAction(_revert_action);
	_popup_menu->addSeparator();
	_popup_menu->addAction(_copy_filename_action);
	_popup_menu->addAction(_copy_pathname_action);

}

PrivateProjectDirectoryWidgetImpl::~PrivateProjectDirectoryWidgetImpl()
{
}

void PrivateProjectDirectoryWidgetImpl::updateActions(QModelIndexList selectedRows)
{
	const int nb_selected = selectedRows.size ();
	XinxProject::Project * project = _model->fileProject(selectedRows.at(0));

	bool is_directory = true;
	bool is_file = true;
	bool is_project  = true;
	bool is_mutli_project = false;
	bool is_selected = project == XinxProject::Manager::self()->selectedProject();
	bool is_rcs_actived = project && project->rcsProxy() && ! project->projectRCS().isEmpty();
	bool is_rcs_enabled = is_rcs_actived && ! VersionControl::Manager::self ()->isExecuting ();
	bool is_rcs_modified = true;
	bool is_rcs_unknown  = true;
	bool is_rcs_removed  = true;

	foreach(QModelIndex index, selectedRows)
	{
		if (_model->isDir (index))
		{
			is_file = false;
		}
		else
		{
			is_directory = false;
		}
		if (!_model->isProject(index)) is_project = false;
		if (_model->fileProject(index) != project) is_mutli_project = true;

		switch (_model->fileState(index))
		{
			case RCS::Unknown:
				is_rcs_modified = false;
				is_rcs_removed = false;
				break;
			case RCS::LocallyAdded:
			case RCS::LocallyModified:
				is_rcs_unknown    = false;
				is_rcs_removed    = false;
				break;
			case RCS::LocallyRemoved:
				is_rcs_unknown = false;
				is_rcs_modified = false;
				break;
			case RCS::NeedsCheckout:
				is_rcs_modified = false;
				is_rcs_unknown  = false;
			default:
				is_rcs_modified = false;
				is_rcs_unknown  = false;
				is_rcs_removed  = false;
				break;
		}
	}

	_set_project_as_default_action->setVisible ((nb_selected == 1) && is_project && !is_selected);
	_project_property_action->setVisible ((nb_selected == 1) && is_project);
	_create_directory_action->setVisible ((nb_selected == 1) && is_directory);
	_new_file_action->setVisible ((nb_selected == 1) && is_directory);
	_rename_file_action->setVisible ((nb_selected == 1) && is_file);
	_remove_files_action->setVisible (is_file);
	_open_files_action->setVisible(is_file);

	_compare_action->setVisible (nb_selected == 2 && is_file);
	_compare_with_workingcopy_action->setVisible (is_rcs_actived && (nb_selected == 1) && is_file);
	_update_action->setVisible (is_rcs_actived && ! is_mutli_project);
	_commit_action->setVisible (is_rcs_actived && ! is_mutli_project);
	_add_action->setVisible (is_rcs_actived && ! is_mutli_project && ! is_project && is_rcs_unknown);
	_remove_action->setVisible (is_rcs_actived && ! is_mutli_project && ! is_project && is_rcs_modified);
	_revert_action->setVisible (is_rcs_actived && ! is_mutli_project && ! is_project && is_file && (is_rcs_removed || is_rcs_modified));

	_compare_with_workingcopy_action->setEnabled(is_rcs_enabled);
	_update_action->setEnabled (is_rcs_enabled);
	_commit_action->setEnabled (is_rcs_enabled);
	_add_action->setEnabled (is_rcs_enabled);
	_remove_action->setEnabled (is_rcs_enabled);
	_revert_action->setEnabled (is_rcs_enabled);
}

void PrivateProjectDirectoryWidgetImpl::rcsLogTerminated()
{
	if (sender())
	{
		sender()->disconnect(this, SLOT(rcsLogTerminated));
	}

	if (! _head_content.isEmpty())
	{
		try
		{
			QTemporaryFile * headContentFile = new QTemporaryFile(this);   // Delete when the main windows is destroyed
			if (headContentFile->open())
			{
				headContentFile->write(_head_content);
				headContentFile->close();
			}
			QProcess::startDetached(XINXConfig::self()->getTools("diff"), QStringList() << _compare_file_name << headContentFile->fileName());
			_head_content.clear();
		}
		catch (ToolsNotDefinedException e)
		{
			qWarning() << e.getMessage();
		}
	}
}

void PrivateProjectDirectoryWidgetImpl::createDirectory()
{
	QStringList list = _parent->selectedFiles();
	Q_ASSERT_X(list.size(), "PrivateProjectDirectoryWidgetImpl::createDirectory", "The list musn't be empty.");

	QString new_directory = QInputDialog::getText(_parent, tr("Create a new directory"), tr("Enter the name of the new directory"));
	if (! new_directory.isEmpty())
	{
		QDir(list.at(0)).mkdir(new_directory);
	}
}

void PrivateProjectDirectoryWidgetImpl::newFile()
{
	const QModelIndexList & indexes = _parent->_directory_view->selectionModel()->selectedRows();
	Q_ASSERT_X(indexes.count () == 1, "PrivateProjectDirectoryWidgetImpl::newFile", "Too much directory selected");
	NewFileWizardImpl dlg(qApp->activeWindow ());
	dlg.setProject(_model->fileProject(indexes.at (0)));
	dlg.setPath (_model->filePath (indexes.at (0)));
	dlg.setFileName (QString());
	if (dlg.exec () == QDialog::Accepted)
	{
		Q_ASSERT_X(dlg.selectedType(), "PrivateEditorManager::newFile", "No interface editor defined");
		Q_ASSERT_X(dlg.project(), "PrivateEditorManager::newFile", "No project defined");

		EditorManager::self()->openFile(dlg.filename(), dlg.selectedType(), dlg.project());
	}
}

void PrivateProjectDirectoryWidgetImpl::openFile(const QModelIndex & index)
{
	const QFileInfo file_info = _model->fileInfo(index);
	const QString file_path = _model->filePath(index);
	XinxProject::Project * project = _model->fileProject(index);

	if (! file_info.isDir())
	{
		emit _parent->open(file_path, 0, project);
	}
}

void PrivateProjectDirectoryWidgetImpl::openFiles()
{
	foreach(const QModelIndex & index, _parent->_directory_view->selectionModel()->selectedRows())
	{
		openFile(index);
	}
}

void PrivateProjectDirectoryWidgetImpl::removeFiles()
{
	QStringList list = _parent->selectedFiles();
	QMessageBox::StandardButton result = QMessageBox::question(qApp->activeWindow(), tr("Delete file(s)"), tr("Can you confirm that you want remove %Ln file(s)", "", list.size()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (result == QMessageBox::Yes)
	{
		foreach(const QString & file, list)
		{
			QFile::remove(file);
		}
	}
}

void PrivateProjectDirectoryWidgetImpl::renameFiles()
{
	QStringList list = _parent->selectedFiles();
	Q_ASSERT_X(list.size() == 1, "PrivateProjectDirectoryWidgetImpl::renameFiles", "The list must have only one element.");

	QString new_filename = QInputDialog::getText(_parent, tr("Rename a file"), tr("Enter the name of the new file name"));
	if (! new_filename.isEmpty())
	{
		QDir directory = QFileInfo(list.at (0)).absoluteDir ();
		QFile::rename (list.at (0), directory.absoluteFilePath (new_filename));
	}
}

void PrivateProjectDirectoryWidgetImpl::copyFileNameTriggered()
{
	QStringList list = _parent->selectedFiles();
	QString names;
	foreach(const QString & name, list)
	{
		names += QFileInfo(name).fileName() + "\n";
	}
	qApp->clipboard()->setText(names);

}

void PrivateProjectDirectoryWidgetImpl::copyPathNameTriggered()
{
	QStringList list = _parent->selectedFiles();
	qApp->clipboard()->setText(list.join("\n"));
}

void PrivateProjectDirectoryWidgetImpl::setProjectAsDefaultTriggered()
{
	QModelIndexList list = _parent->_directory_view->selectionModel()->selectedRows();
	if (list.size() != 1) return;

	XinxProject::Manager::self()->setSelectedProject(_model->fileProject(list.at(0)));
}

void PrivateProjectDirectoryWidgetImpl::projectPropertyTriggered()
{
	QModelIndexList list = _parent->_directory_view->selectionModel()->selectedRows();
	if (list.size() != 1) return;

	XinxProject::Manager::self()->customizeProject(_model->fileProject(list.at(0)));
}

void PrivateProjectDirectoryWidgetImpl::compareWithVersionControlTriggered()
{
	QModelIndexList rows = _parent->_directory_view->selectionModel()->selectedRows();
	if (rows.size() != 1) return;

	XinxProject::Project * project = _model->fileProject(rows.at(0));

	Q_ASSERT(! project->projectRCS().isEmpty() && project->rcsProxy());

	QStringList list = _parent->selectedFiles();
	Q_ASSERT(list.size() == 1);

	_compare_file_name = list.at(0);
	_head_content.clear();
	QString revision = _model->fileVersion(rows.at(0));

	connect(project->rcsProxy(), SIGNAL(stopOperations()), this, SLOT(rcsLogTerminated()));
	project->rcsProxy()->updateToRevision(_compare_file_name, revision, &_head_content);
}

void PrivateProjectDirectoryWidgetImpl::compareFilesTriggered()
{
	try
	{
		QStringList list = _parent->selectedFiles();
		Q_ASSERT(list.size() == 2);
		QProcess::startDetached(XINXConfig::self()->getTools("diff"), QStringList() << list.at(0) << list.at(1));
	}
	catch (ToolsNotDefinedException e)
	{
		qWarning() << e.getMessage();
	}
}

void PrivateProjectDirectoryWidgetImpl::updateFromVersionControlTriggered()
{
	QModelIndexList rows = _parent->_directory_view->selectionModel()->selectedRows();
	if (rows.size() != 1) return;

	XinxProject::Project * project = _model->fileProject(rows.at(0));

	Q_ASSERT(! project->projectRCS().isEmpty() && project->rcsProxy());

	QStringList list = _parent->selectedFiles();
	if (list.count() > 0)
		project->rcsProxy()->updateWorkingCopy(list);
}

void PrivateProjectDirectoryWidgetImpl::commitToVersionControlTriggered()
{
	QModelIndexList rows = _parent->_directory_view->selectionModel()->selectedRows();
	if (rows.size() != 1) return;

	XinxProject::Project * project = _model->fileProject(rows.at(0));

	Q_ASSERT(! project->projectRCS().isEmpty() && project->rcsProxy());

	QStringList list = _parent->selectedFiles();
	if (list.count() > 0)
		project->rcsProxy()->validWorkingCopy(list, _parent);
}

void PrivateProjectDirectoryWidgetImpl::addToVersionControlTriggered()
{
	QModelIndexList rows = _parent->_directory_view->selectionModel()->selectedRows();
	if (rows.size() != 1) return;

	XinxProject::Project * project = _model->fileProject(rows.at(0));

	Q_ASSERT(! project->projectRCS().isEmpty() && project->rcsProxy());

	QStringList list = _parent->selectedFiles();
	if (list.count() > 0)
	{
		project->rcsProxy()->addFileOperation(VersionControl::RCSProxy::RCS_ADD, list, _parent, false);
		project->rcsProxy()->validFileOperations();
	}
}

void PrivateProjectDirectoryWidgetImpl::removeFromVersionControlTriggered()
{
	QModelIndexList rows = _parent->_directory_view->selectionModel()->selectedRows();
	if (rows.size() != 1) return;

	XinxProject::Project * project = _model->fileProject(rows.at(0));

	Q_ASSERT(! project->projectRCS().isEmpty() && project->rcsProxy());

	QStringList list = _parent->selectedFiles();
	if (list.count() > 0)
	{
		project->rcsProxy()->addFileOperation(VersionControl::RCSProxy::RCS_REMOVE, list, _parent, false);
		project->rcsProxy()->validFileOperations();
	}
}

void PrivateProjectDirectoryWidgetImpl::revertFileTriggered()
{
	// FIXME: For RCS with the option add the possibility to revert a file.
}

void PrivateProjectDirectoryWidgetImpl::doubleClicked (const QModelIndex & index)
{
	openFile(index);
}

void PrivateProjectDirectoryWidgetImpl::rowsInserted(const QModelIndex & index, int start, int end)
{
	if (_toggled_view_action->isChecked())
	{
		for (int i = start; i <= end; i++)
		{
			_parent->_directory_view->expand(_model->index(i, 0, index));
		}
	}
}

void PrivateProjectDirectoryWidgetImpl::updateFilter()
{
	_refresh_model_timer->stop();

	switch(_parent->_filter_type->currentIndex())
	{
	case 0:
		_filter_type = XinxProject::ProjectListModel::FILTER_FILENAME;
		break;
	case 1:
		_filter_type = XinxProject::ProjectListModel::FILTER_SIZE;
		break;
	case 2:
		_filter_type = XinxProject::ProjectListModel::FILTER_DATE;
		break;
	case 3:
		_filter_type = XinxProject::ProjectListModel::FILTER_CONTAINS;
		break;
	case 4:
		_filter_type = XinxProject::ProjectListModel::FILTER_STATUS;
		break;
	default:
		_filter_type = XinxProject::ProjectListModel::FILTER_NONE;
		break;
	}

	_filter_filename = _parent->_filter_filename_edit->text();

	switch(_parent->_filter_operator->currentIndex())
	{
	case 0:
		_filter_operator = XinxProject::ProjectListModel::FILTER_GT;
		break;
	case 1:
		_filter_operator = XinxProject::ProjectListModel::FILTER_GEQT;
		break;
	case 2:
		_filter_operator = XinxProject::ProjectListModel::FILTER_EQ;
		break;
	case 3:
		_filter_operator = XinxProject::ProjectListModel::FILTER_LEQT;
		break;
	case 4:
		_filter_operator = XinxProject::ProjectListModel::FILTER_LT;
		break;
	case 5:
		_filter_operator = XinxProject::ProjectListModel::FILTER_DIFF;
		break;
	}

	_filter_size = _parent->_filter_size->value();
	_filter_date = _parent->_filter_date->date();
	_filter_contains = _parent->_filter_contains->text();

	switch(_parent->_filter_state->currentIndex())
	{
	case 0:
		_filter_state = RCS::LocallyModified;
		break;
	case 1:
		_filter_state = RCS::LocallyAdded;
		break;
	case 2:
		_filter_state = RCS::LocallyRemoved;
		break;
	case 3:
		_filter_state = RCS::NeedsCheckout;
		break;
	case 4:
		_filter_state = RCS::Updated;
		break;
	}

	if ((_filter_type == XinxProject::ProjectListModel::FILTER_FILENAME) && (_filter_filename.isEmpty()))
	{
		_filter_type = XinxProject::ProjectListModel::FILTER_NONE;
	}
	else if ((_filter_type == XinxProject::ProjectListModel::FILTER_CONTAINS) && (_filter_contains.isEmpty()))
	{
		_filter_type = XinxProject::ProjectListModel::FILTER_NONE;
	}

	int timeout = XINXConfig::self()->config().project.automaticProjectDirectoryRefreshTimeout;
	if (timeout > 0)
	{
		_refresh_model_timer->setInterval(timeout);
		_refresh_model_timer->start();
	}
}

void PrivateProjectDirectoryWidgetImpl::returnPressed()
{
	int timeout = XINXConfig::self()->config().project.automaticProjectDirectoryRefreshTimeout;
	if (timeout == 0)
	{
		filterTimeout();
	}
}

void PrivateProjectDirectoryWidgetImpl::filterTimeout()
{
	_refresh_model_timer->stop();

	// We unapply the flat view before unapply filter, or apply flat view after apply filter
	if (_filter_type == XinxProject::ProjectListModel::FILTER_NONE)
	{
		_toggled_view_action->setChecked (false);
	}

	_model->setFilterType(_filter_type);
	_model->setFilterFileName(_filter_filename);
	_model->setFilterSize(_filter_operator, _filter_size * 1024);
	_model->setFilterDate(_filter_date);
	_model->setFilterContains(_filter_contains);
	_model->setFilterState(_filter_state);
	//FIXME:_model->setFilterEmptyDirectory(_filter_type != XinxProject::ProjectListModel::FILTER_NONE);
	_model->applyFilter();

	if (_filter_type != XinxProject::ProjectListModel::FILTER_NONE)
	{
		_toggled_view_action->setChecked (true);
	}
}

void PrivateProjectDirectoryWidgetImpl::toggledFlatView(bool flat)
{
	if (flat)
	{
		_model->setLongDirectoryName (true);
		_parent->_directory_view->setIndentation(0);
		_parent->_directory_view->setRootIsDecorated(false);
		_parent->_directory_view->setItemsExpandable(false);
		_parent->_directory_view->expandAll();
	}
	else
	{
		_model->setLongDirectoryName (false);
		_parent->_directory_view->collapseAll();
		_parent->_directory_view->setIndentation(20);
		_parent->_directory_view->setRootIsDecorated(true);
		_parent->_directory_view->setItemsExpandable(true);
		_parent->_directory_view->collapseAll();
	}
}

bool PrivateProjectDirectoryWidgetImpl::eventFilter(QObject *obj, QEvent *event)
{
	if ((obj == _parent->_directory_view) && (event->type() == QEvent::ContextMenu))
	{
		QModelIndexList selectedRows = _parent->_directory_view->selectionModel()->selectedRows();
		int nbSelected = selectedRows.size();
		if (nbSelected == 0) return QObject::eventFilter(obj, event);

		updateActions (selectedRows);
		_popup_menu->exec(static_cast<QContextMenuEvent*>(event)->globalPos());
	}
	return QObject::eventFilter(obj, event);
}

/* ProjectDirectoryWidgetImpl */

ProjectDirectoryWidgetImpl::ProjectDirectoryWidgetImpl(QWidget* parent): QWidget(parent)
{
	setupUi (this);

	d = new PrivateProjectDirectoryWidgetImpl (this);

	_directory_view->installEventFilter(d);

	_filter_date->setDate(QDate::currentDate());

	setWindowTitle(tr("Project Directory"));
	setWindowIcon(QIcon(":/images/project_open.png"));

	d->_toggled_view_action = new QAction(QIcon(":/images/flatlist.png"), tr("Toggled Flat View"), this);
	d->_toggled_view_action->setWhatsThis(tr("If checked the list is showed as flat instead of tree. Each list of file is preceded of a directory header."));
	d->_toggled_view_action->setCheckable(true);
	connect(d->_toggled_view_action, SIGNAL(toggled(bool)), d, SLOT(toggledFlatView(bool)));

	_flat_view_button->setDefaultAction(d->_toggled_view_action);

	_new_project_button->setDefaultAction(XinxProject::Manager::self()->newProjectAction());
	_open_project_button->setDefaultAction(XinxProject::Manager::self()->openProjectAction());
	_close_selected_project_button->setDefaultAction(XinxProject::Manager::self()->closeProjectAction());

	_update_selected_project_button->setDefaultAction(VersionControl::Manager::self()->updateAllAction());
	_commit_selected_project_button->setDefaultAction(VersionControl::Manager::self()->commitAllAction());

	d->_provider = new IconProjectProvider();
	d->_model = new XinxProject::ProjectListModel(_directory_view);
	d->_model->setFileIconProvider(d->_provider);
	_directory_view->setModel(d->_model);
	_directory_view->header()->setResizeMode(QHeaderView::Fixed);
	_directory_view->header()->resizeSection(0, 1024);

	connect(_directory_view, SIGNAL(doubleClicked(QModelIndex)), d, SLOT(doubleClicked(QModelIndex)));
	connect(d->_model, SIGNAL(rowsInserted(QModelIndex, int, int)), d, SLOT(rowsInserted(QModelIndex, int, int)));

	connect(_filter_contains, SIGNAL(textChanged(QString)), d, SLOT(updateFilter()));
	connect(_filter_contains, SIGNAL(returnPressed()), d, SLOT(returnPressed()));

	connect(_filter_date, SIGNAL(dateChanged(QDate)), d, SLOT(updateFilter()));
	connect(_filter_date, SIGNAL(editingFinished()), d, SLOT(returnPressed()));

	connect(_filter_filename_edit, SIGNAL(textChanged(QString)), d, SLOT(updateFilter()));
	connect(_filter_filename_edit, SIGNAL(returnPressed()), d, SLOT(returnPressed()));

	connect(_filter_operator, SIGNAL(currentIndexChanged(int)), d, SLOT(updateFilter()));
	connect(_filter_operator, SIGNAL(currentIndexChanged(int)), d, SLOT(filterTimeout()));

	connect(_filter_size, SIGNAL(valueChanged(double)), d, SLOT(updateFilter()));
	connect(_filter_size, SIGNAL(editingFinished()), d, SLOT(returnPressed()));

	connect(_filter_state, SIGNAL(currentIndexChanged(int)), d, SLOT(updateFilter()));
	connect(_filter_state, SIGNAL(currentIndexChanged(int)), d, SLOT(filterTimeout()));

	connect(_filter_type, SIGNAL(currentIndexChanged(int)), d, SLOT(updateFilter()));
	connect(_filter_type, SIGNAL(currentIndexChanged(int)), d, SLOT(filterTimeout()));
}

ProjectDirectoryWidgetImpl::~ProjectDirectoryWidgetImpl()
{
	d->_model->setFileIconProvider(NULL);
	delete d->_provider;
	delete d;
}

QStringList ProjectDirectoryWidgetImpl::selectedFiles() const
{
	QStringList paths;
	QModelIndexList list = _directory_view->selectionModel()->selectedRows();
	foreach(const QModelIndex & index, list)
		paths << d->_model->filePath(index);

	return paths;
}

QAction* ProjectDirectoryWidgetImpl::toggledViewAction() const
{
	return d->_toggled_view_action;
}

void ProjectDirectoryWidgetImpl::setDock(QDockWidget* dock)
{
	d->_dock_widget = dock;
}
