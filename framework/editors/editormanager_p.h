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

#pragma once
#ifndef EDITORMANAGER_P_H
#define EDITORMANAGER_P_H

// Xinx header
#include "editormanager.h"
#include "editorfactory.h"
#include "textfileeditor.h"
#include "xinxcodeedit.h"
#include <scripts/scriptmanager.h>
#include <plugins/xinxpluginsloader.h>
#include <project/xinxprojectproject.h>
#include <core/xinxconfig.h>
#include "savingdialog.h"

// Qt header
#include <QTabWidget>
#include <QFileDialog>
#include <QTabBar>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QMenu>
#include <QPrintDialog>
#include <QPrinter>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QToolButton>

class PrivateTabWidget : public QTabWidget
{
	Q_OBJECT
public:
    PrivateTabWidget ( EditorManager* parent );
    virtual ~PrivateTabWidget();

	inline QTabBar * tabBar() const;
protected:
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);
private:
	EditorManager * _manager;
};

class PrivateEditorManager : public QObject
{
	Q_OBJECT
public:
	PrivateEditorManager(EditorManager * manager);
	~PrivateEditorManager();

	void addTab(AbstractEditor * editor);
	void updateTabWidget(AbstractEditor * editor);

	void connectEditor(AbstractEditor * editor);
	void disconnectEditor(AbstractEditor * editor);

	bool editorMayBeSave(QList< AbstractEditor* > editors);

	int getClickedTab();
	int tabPosition(QPoint point);

	bool eventFilter(QObject * obj, QEvent *event);

	void createOpenSubMenu();
	void createActions();
	void updateActions();
	void createCloseButton();

	PrivateTabWidget * tabWidget();

	QString m_lastOpenedFileName;
	EditorManager * _manager;
	QPointer<PrivateTabWidget> _tab_widget;
	int _clicked_item;

	QToolButton * _close_tab_button;

	QAction * _recent_actions[ MAXRECENTFILES ];
	QAction * _recent_separator;

	QAction * _new_action;
	QAction * _recent_action;
	QAction * _refresh_action;
	QAction * _save_action;
	QAction * _save_as_action;
	QAction * _save_all_action;
	QAction * _print_action;
	QAction * _close_action;
	QAction * _close_all_action;
	QAction * _copy_filename_action;
	QAction * _copy_path_action;

	QAction * _next_tab_action;
	QAction * _previous_tab_action;
public slots:
	void newFile();
	void openRecentFile();
	void updateRecentFiles();
	void tabCloseRequested(int index);
	void slotModifiedChange();
	void updateConfigElement();

};


#endif // EDITORMANAGER_P_H
