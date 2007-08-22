/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __PROJECTDIRECTORYDOCKWIDGET_H__
#define __PROJECTDIRECTORYDOCKWIDGET_H__

#include <QDockWidget>
#include <QString>

class PrivateProjectDirectoryDockWidget;
class QAbstractItemModel;
class QAction;
class XSLProject;

class ProjectDirectoryDockWidget : public QDockWidget {
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
	void setToggledViewAction( QAction * action );
	
	bool isViewFlat();
	QStringList selectedFiles();
	void setProjectPath( XSLProject * project );
	
	void refreshPath( const QString & path );
	bool removeFile( const QString & path );
public slots:
	void toggledView();
	void toggledView( bool flat );
signals:
	void open( const QString & name );
private:
	PrivateProjectDirectoryDockWidget * d;
	friend class PrivateProjectDirectoryDockWidget;
};

#endif // __PROJECTDIRECTORYDOCKWIDGET_H__
