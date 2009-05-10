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
 * \file contentviewmodel.h
 * \brief This file contains element to manage content view model.
 */

#ifndef __CONTENTVIEWMODEL_H__
#define __CONTENTVIEWMODEL_H__

// Xinx header
#include <contentview/abstractcontentviewmodel.h>

// Qt header
#include <QAbstractItemModel>
#include <QStack>
#include <QHash>

class ContentViewNode;

/*!
 * \class ContentViewModel
 * \brief This class is used to show the content of the file in a tree view.
 *
 * This class is used as a proxy of the tree view and content view nodes. It's
 * a sub-class of the Qt standard class QAbstractItemModel.
 */
class ContentViewModel : public AbstractContentViewModel {
	Q_OBJECT
public:
	/*!
	 * Construct a file content model.
	 * \param parent Parent of the file content model.
	 */
	ContentViewModel( ContentViewNode * root, QObject *parent = 0 );
	/*! Destroy the model. */
	virtual ~ContentViewModel();

	/*!
	 * This structure give the line and the filename where we can find data that
	 * model represents.
	 * If the filename is empty, the model refere to the current file.
	 */
	struct struct_file_content {
		int line; /*!< The line of the element. */
		QString filename; /*!< The file name of the element. If empty the file is the current file. */
	};

	virtual QVariant data( const QModelIndex &index, int role ) const;
	virtual Qt::ItemFlags flags( const QModelIndex &index ) const;
	virtual QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
	virtual QModelIndex index( ContentViewNode * node ) const;
	virtual QModelIndex parent( const QModelIndex &index ) const;
	virtual bool hasChildren( const QModelIndex & parent = QModelIndex() ) const;
	virtual int rowCount( const QModelIndex &parent = QModelIndex() ) const;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;

protected:
	virtual void nodeChanged( ContentViewNode * node );
	virtual void beginInsertNode( ContentViewNode * node, int first, int last );
	virtual void beginRemoveNode( ContentViewNode * node, int first, int last );
	virtual void endInsertNode();
	virtual void endRemoveNode();

	QModelIndex index( quint32 id ) const;

	QHash< quint32, quint32 > m_parents;
	QHash< quint32, QList< quint32 > > m_childs;

	QHash< quint32, ContentViewNode * > m_nodeOfId;
	QMultiHash< ContentViewNode*, quint32 > m_idOfNode;
private:
	void addChildsOf( quint32 id );
	void removeChildsOf( quint32 id );
	inline quint32 createId();

	struct tupleParentFisrtLast {
		ContentViewNode * parent;
		int first, last;
	};

	QStack<tupleParentFisrtLast> m_stack;

	unsigned long m_lastId;
};

Q_DECLARE_METATYPE( ContentViewModel::struct_file_content )

#endif /* __CONTENTVIEWMODEL_H__ */
