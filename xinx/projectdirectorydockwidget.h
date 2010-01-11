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

#ifndef __PROJECTDIRECTORYDOCKWIDGET_H__
#define __PROJECTDIRECTORYDOCKWIDGET_H__
#pragma once

// Xinx header
#include "ui_projectdirectorywidget.h"

// Qt header
#include <QDockWidget>
#include <QString>
#include <QPointer>
#include <QDirModel>

class QAbstractItemModel;
class QAction;
class XinxProject;
class RCS;
class IconProjectProvider;

class ProjectDirectoryDockWidget : public QDockWidget  {
	Q_OBJECT
public:
	ProjectDirectoryDockWidget( const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	ProjectDirectoryDockWidget( QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	virtual ~ProjectDirectoryDockWidget();

	void setToggledViewAction( QAction * action );

	bool isViewFlat();
	QStringList selectedFiles();

	void refreshPath( const QString & path );
	bool removeFile( const QString & path );

public slots:
	void toggledView();
	void toggledView( bool flat );
signals:
	void open( const QString & name );

private slots:
	void projectChange();
	void filtreChange();
	void on_m_filtreLineEdit_returnPressed();
	void on_m_filtreLineEdit_textChanged( QString filtre );
	void on_m_projectDirectoryTreeView_doubleClicked( QModelIndex index );

	void copyFileNameTriggered();
	void copyPathNameTriggered();

	void selectedUpdateFromVersionManager();
	void selectedCommitToVersionManager();
	void selectedAddToVersionManager();
	void selectedRemoveFromVersionManager();
	void selectedCompareWithVersionManager();
	void selectedCompare();
	void rcsLogTerminated();

	void updateActions();
private:
	void init();
	bool eventFilter( QObject *obj, QEvent *event );
	void setProjectPath( XinxProject * project );

	QAction* m_selectedUpdateAction;
	QAction* m_selectedCommitAction;
	QAction* m_selectedAddAction;
	QAction* m_selectedRemoveAction;
	QAction* m_selectedCompareWithHeadAction;
	QAction* m_selectedCompareAction;
	QAction* m_copyFileNameAction;
	QAction* m_copyPathNameAction;

	QTimer* m_modelTimer;
	QPointer<QDirModel> m_dirModel;
	QPointer<QAbstractItemModel> m_flatModel;
	IconProjectProvider* m_iconProvider;

	QPointer<XinxProject> m_project;
	QString m_projectPath;
	QString m_compareFileName;
	QByteArray m_headContent;

	Ui::ProjectDirectoryWidget * m_projectDirWidget;
};

#endif // __PROJECTDIRECTORYDOCKWIDGET_H__
