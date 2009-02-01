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

#ifndef __P_PROJECTDIRECTORYDOCKWIDGET_H__
#define __P_PROJECTDIRECTORYDOCKWIDGET_H__

// Xinx header
#include "../projectdirectorydockwidget.h"
#include "ui_projectdirectorywidget.h"
#include "../iconprojectprovider.h"
#include "xslproject.h"

// Qt header
#include <QObject>
#include <QTimer>
#include <QDirModel>
#include <QAbstractItemModel>

class PrivateProjectDirectoryDockWidget : public QObject {
	Q_OBJECT
public:
	PrivateProjectDirectoryDockWidget( ProjectDirectoryDockWidget * parent );
	~PrivateProjectDirectoryDockWidget();

	Ui::ProjectDirectoryWidget * m_projectDirWidget;
	QAction * m_selectedUpdateAction;
	QAction * m_selectedCommitAction;
	QAction * m_selectedAddAction;
	QAction * m_selectedRemoveAction;
	QAction * m_selectedCompareWithHeadAction;
	QAction * m_selectedCompareWithStdAction;
	QAction * m_selectedCompareAction;
	QAction * m_copyFileNameAction;
	QAction * m_copyPathNameAction;

	QTimer * m_modelTimer;
	QDirModel * m_dirModel;
	QAbstractItemModel * m_flatModel;
	IconProjectProvider * m_iconProvider;

	XSLProject * m_project;

	bool eventFilter( QObject *obj, QEvent *event );
public slots:
	void projectChange();
	void filtreChange();
	void on_m_filtreLineEdit_returnPressed();
	void on_m_filtreLineEdit_textChanged( QString filtre );
	void on_m_projectDirectoryTreeView_doubleClicked( QModelIndex index );
	void on_m_prefixComboBox_activated( QString prefix );

	void copyFileNameTriggered();
	void copyPathNameTriggered();
private:
	ProjectDirectoryDockWidget * m_parent;
};

#endif // __P_PROJECTDIRECTORYDOCKWIDGET_H__
