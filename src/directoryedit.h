/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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

#ifndef __DIRECTORYEDIT_H__
#define __DIRECTORYEDIT_H__

// Qt header
#include <QLineEdit>
#include <QDirModel>

/*!
 * Completer model class based on a QDirModel which permit to propose a completion on
 * the path. 
 */
class CompleterDirModel : public QDirModel {
public:
	/*!
	 * Construct a CompleterDirModel. The QDir model convert the slash on native separtor.
	 * If the path end with a separator, the separator is deleted.
	 * \param parent The parent of the object.
	 */
	CompleterDirModel(QObject *parent = 0);
	/*!
	 * Return the data to change in the completion dir model. This model show the good
	 * separator (with the \e QDir::toNativeSepartors() function).
	 * \param index The model index, represents a path in the tree.
	 * \param role Only the role Qt::DisplayRole is modified in this object.
	 */
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

/*!
 * A text edit using the QCompleter \e CompleterDirModel to simplify the editing of 
 * the path.
 * If the path is wrong, he's writing in red.
 */
class DirectoryEdit : public QLineEdit {
	Q_OBJECT
public:
	/*!
	 * Construct a DirectoryEdit object.
	 * \param parent The parent widget of the object.
	 */
	DirectoryEdit( QWidget * parent = 0 );
	/*!
	 * Construct a DirectoryEdit object and change the content of the text at the creation.
	 * \param contents Content of the Line edit.
	 * \param parent The parent widget of the object.
	 */
	DirectoryEdit( const QString & contents, QWidget * parent = 0 );
protected slots:
	/*!
	 * Slot called when the text changed. This method change the color of the text to 
	 * show user quickly if the directory exists or not.
	 */
	void slotTextChanged( QString text );
};

#endif // __DIRECTORYEDIT_H__
