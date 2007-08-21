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

#ifndef __FILECONTENTDOCKWIDGET_H__
#define __FILECONTENTDOCKWIDGET_H__

#include <QDockWidget>
#include <QString>

class PrivateFileContentDockWidget;
class QAbstractItemModel;

class FileContentDockWidget : public QDockWidget {
	Q_OBJECT
public:
	FileContentDockWidget( const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	FileContentDockWidget( QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	virtual ~FileContentDockWidget();
public slots:
	void updateModel( QAbstractItemModel * model );
signals:
	void open( const QString & name, int line );
private:
	PrivateFileContentDockWidget * d;
	friend class PrivateFileContentDockWidget;
};

#endif // __FILECONTENTDOCKWIDGET_H__
