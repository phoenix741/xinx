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

// Xinx header
#include "ui_projectdirectorywidget.h"

// Qt header
#include <QDockWidget>
#include <QString>
#include <QPointer>

class QAbstractItemModel;
class QAction;
class XSLProject;
class RCS;
class IconProjectProvider;

class ProjectDirectoryDockWidget : public QDockWidget  {
	Q_OBJECT
public:
	ProjectDirectoryDockWidget( const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	ProjectDirectoryDockWidget( QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	virtual ~ProjectDirectoryDockWidget();

	void setGlobalUpdateAction( QAction * action );
	void setGlobalCommitAction( QAction * action );
	void setSelectedUpdateAction( QAction * action );
	void setSelectedCommitAction( QAction * action );
	void setSelectedAddAction( QAction * action );
	void setSelectedRemoveAction( QAction * action );
	void setSelectedCompareWithHeadAction( QAction * action );
	void setSelectedCompareWithStdAction( QAction * action );
	void setSelectedCompareAction( QAction * action );
	void setToggledViewAction( QAction * action );

	bool isViewFlat();
	QStringList selectedFiles();
	void setProjectPath( XSLProject * project );

	void refreshPath( const QString & path );
	bool removeFile( const QString & path );

	RCS * rcs();
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
	void on_m_prefixComboBox_activated( QString prefix );

	void copyFileNameTriggered();
	void copyPathNameTriggered();

private:
	void init();
	bool eventFilter( QObject *obj, QEvent *event );

	QPointer<QAction> m_selectedUpdateAction;
	QPointer<QAction> m_selectedCommitAction;
	QPointer<QAction> m_selectedAddAction;
	QPointer<QAction> m_selectedRemoveAction;
	QPointer<QAction> m_selectedCompareWithHeadAction;
	QPointer<QAction> m_selectedCompareWithStdAction;
	QPointer<QAction> m_selectedCompareAction;
	QPointer<QAction> m_copyFileNameAction;
	QPointer<QAction> m_copyPathNameAction;

	QPointer<QTimer> m_modelTimer;
	QPointer<QDirModel> m_dirModel;
	QPointer<QAbstractItemModel> m_flatModel;
	IconProjectProvider* m_iconProvider;

	QPointer<XSLProject> m_project;

	Ui::ProjectDirectoryWidget * m_projectDirWidget;
};

#endif // __PROJECTDIRECTORYDOCKWIDGET_H__
