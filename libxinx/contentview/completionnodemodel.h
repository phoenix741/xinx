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

#ifndef _COMPLETIONNODEMODEL_H_
#define _COMPLETIONNODEMODEL_H_

// Xinx header
#include <contentview/abstractcontentviewmodel.h>

/*!
 * \class CompletionNodeModel
 * \brief The completion node model is used to complete editor.
 *
 * The goal of CompletionNodeModel is to manage a list of elements for the
 * completion. This completion model is used by an object like \e QCompleter.
 */
class CompletionNodeModel : public AbstractContentViewModel {
	Q_OBJECT
public:
	/*!
	 * Create a completion node model.
	 * \param root The root element that contains all node used for \
	 * completion
	 * \param parent The parent of the completion node model.
	 */
	CompletionNodeModel( ContentViewNode * root, QObject *parent = 0 );
	//! Destroy the completion node model.
	virtual ~CompletionNodeModel();

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
	//! Method to used to know if an element must be showed
	virtual bool mustElementBeShowed( ContentViewNode * node );

	//! List of all nodes in the model
	QList<ContentViewNode*> & nodes();
	//! List of showed nodes in the model
	QList<ContentViewNode*> & showedNodes();

	//! Show a node in the screen
	void showNode( ContentViewNode * node );
	//! Hide a node of the screen
	void hideNode( ContentViewNode * node );
	//! Return wether the node is showed or not
	bool isElementShowed( ContentViewNode * node );

	virtual void beginInsertNode( ContentViewNode * node, int first, int last );
	virtual void beginRemoveNode( ContentViewNode * node, int first, int last );
	virtual void endInsertNode();
	virtual void endRemoveNode();
private:
	ContentViewNode * m_parent;
	int m_first, m_last;

	QList<ContentViewNode*> m_nodes, m_showedNodes;
};

#endif /* _COMPLETIONNODEMODEL_H_ */