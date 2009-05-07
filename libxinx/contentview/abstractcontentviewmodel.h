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

/*!
 * \file abstractcontentviewmodel.h
 * \brief This file contains element to manage content view node for content view or for completion.
 */

#ifndef __ABSTRACTCONTENTVIEWMODEL_H__
#define __ABSTRACTCONTENTVIEWMODEL_H__

// Qt header
#include <QAbstractItemModel>
#include <QMutex>

class ContentViewNode;

/*!
 * \class AbstractContentViewModel
 * \brief This class is used to show the content of the file in a tree view or in completion.
 */
class AbstractContentViewModel : public QAbstractItemModel {
	Q_OBJECT
public:
	/*!
	 * Construct a file node model.
	 * \param parent Parent of the file node model.
	 */
	AbstractContentViewModel( ContentViewNode * root, QObject *parent = 0 );
	/*! Destroy the model. */
	virtual ~AbstractContentViewModel();

	virtual QModelIndex index( ContentViewNode * node ) const = 0;
protected:
	virtual void nodeChanged( ContentViewNode * node );
	virtual void beginInsertNode( ContentViewNode * node, int first, int last );
	virtual void beginRemoveNode( ContentViewNode * node, int first, int last );
	virtual void endInsertNode();
	virtual void endRemoveNode();

	ContentViewNode * rootNode() const;
	QMutex & mutex();
private:
	ContentViewNode * m_rootNode;
	QMutex m_updateMutex;

	friend class ContentViewNode;
};

#endif /* __ABSTRACTCONTENTVIEWMODEL_H__ */
