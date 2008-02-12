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

/*!
 * This dock contains the content of the current file editor in a tree.
 * This dock permit the user to brownse the content of the file, and go inside import.
 */
class FileContentDockWidget : public QDockWidget {
	Q_OBJECT
public:
	FileContentDockWidget( const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	FileContentDockWidget( QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	virtual ~FileContentDockWidget();
public slots:
	/*!
	 * Update the tree with this model. The model comes from the editor
	 * \param model Model contains the content of the editor. If NULL, the editor is deleted.
	 */
	void updateModel( QAbstractItemModel * model );
signals:
	/*!
	 * Signal emited when the user double-click on the editor. The goal is open the file 
	 * if necessary, and change the line.
	 * \param name Name of the file to open
	 * \param line Line to change.
	 */
	void open( const QString & name, int line );
private:
	PrivateFileContentDockWidget * d;
	friend class PrivateFileContentDockWidget;
};

#endif // __FILECONTENTDOCKWIDGET_H__