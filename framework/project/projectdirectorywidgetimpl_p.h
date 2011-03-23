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

#ifndef _PROJECTDIRECTORYWIDGETIMPL_P_
#define _PROJECTDIRECTORYWIDGETIMPL_P_

#include "projectdirectorywidgetimpl.h"
#include "projectlistmodel.h"
#include "iconprojectprovider.h"

class QTimer;
class QAction;
class QDockWidget;

class PrivateProjectDirectoryWidgetImpl : public QObject
{
	Q_OBJECT
public:
	explicit PrivateProjectDirectoryWidgetImpl(ProjectDirectoryWidgetImpl* parent);
	virtual ~PrivateProjectDirectoryWidgetImpl();

	ProjectDirectoryWidgetImpl * _parent;
	XinxProject::ProjectListModel * _model;
	QDockWidget * _dock_widget;
	IconProjectProvider * _provider;
	QTimer * _refresh_model_timer;

	QMenu* _popup_menu;

	QAction* _plugin_separator;

	QAction* _create_directory_action;
	QAction* _new_file_action;
	QAction* _open_files_action;
	QAction* _remove_files_action;
	QAction* _rename_file_action;

	QAction* _toggled_view_action;
	QAction* _project_property_action;
	QAction* _set_project_as_default_action;
	QAction* _compare_with_workingcopy_action;
	QAction* _compare_action;
	QAction* _update_action;
	QAction* _commit_action;
	QAction* _add_action;
	QAction* _remove_action;
	QAction* _revert_action;
	QAction* _blame_action;
	QAction* _log_action;
	QAction* _copy_filename_action;
	QAction* _copy_pathname_action;

	QString _compare_file_name;
	QByteArray _head_content;

	// List of filter
	QString _filter_filename;
	qint64 _filter_size;
	XinxProject::ProjectListModel::FilterOperator _filter_operator;
	QDate _filter_date;
	QString _filter_contains;
	RCS::rcsState _filter_state;
	enum XinxProject::ProjectListModel::FilterType _filter_type;

	virtual bool eventFilter(QObject *obj, QEvent *event);
	void updateActions(QModelIndexList selectedRows);

	void openFile(const QModelIndex & index);
public slots:
	void createPluginsActions();

	void toggledFlatView(bool flat);
	void doubleClicked(const QModelIndex & index);
	void rowsInserted(const QModelIndex & index, int start, int end);
	void updateFilter();
	void filterTimeout();
	void returnPressed();

	void newFile();
	void createDirectory();
	void openFiles();
	void removeFiles();
	void renameFiles();

	void copyFileNameTriggered();
	void copyPathNameTriggered();

	void setProjectAsDefaultTriggered();
	void projectPropertyTriggered();

	void compareWithVersionControlTriggered();
	void compareFilesTriggered();

	void updateFromVersionControlTriggered();
	void commitToVersionControlTriggered();
	void addToVersionControlTriggered();
	void removeFromVersionControlTriggered();
	void revertFileTriggered();
	void blameFileTriggered();
	void showLogTriggered();

	void rcsLogTerminated();
};


#endif // _PROJECTDIRECTORYWIDGETIMPL_P_
