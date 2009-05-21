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
 * \file completionnodemodel.h
 * \brief Contains object manage ContentViewNode for completion
 */

#ifndef _COMPLETIONNODEMODEL_H_
#define _COMPLETIONNODEMODEL_H_

// Xinx header
#include <contentview/abstractcontentviewmodel.h>

// Qt header
#include <QPair>
#include <QHash>

class ContentViewNode;

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
	enum CompletionNodeRole { CompletionNodeName = 0x77 };

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
	virtual QModelIndex parent( const QModelIndex &index ) const;
	virtual bool hasChildren( const QModelIndex & parent = QModelIndex() ) const;
	virtual int rowCount( const QModelIndex &parent = QModelIndex() ) const;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;
protected:
	//! Method to used to know if an element must be showed
	virtual bool mustElementBeShowed( ContentViewNode * n );

	//! List of all nodes in the model
	const QList<ContentViewNode*> & nodes() const;
	//! List of showed nodes in the model
	const QList<ContentViewNode*> & showedNodes() const;

	//! Show a node in the screen
	void showNode( ContentViewNode * n );
	//! Hide a node of the screen
	void hideNode( ContentViewNode * n );
	//! Return wether the node is showed or not
	bool isElementShowed( ContentViewNode * n );

	//! Add the node \e node to hidden nodes (and show it if \e mustElementBeShowed() )
	void addAllNodes( ContentViewNode * parent, ContentViewNode * node );

	//! This methode is called when a node is add.
	virtual void addNode( ContentViewNode * parent, ContentViewNode * n );

	//! Remove the node \e node and all it's child
	void removeAllNodes( ContentViewNode * parent, ContentViewNode * n );

	//! Remove a node from the model
	virtual void removeNode( ContentViewNode * parent, ContentViewNode * n );

	//! Return the possible parent of a nodes
	ContentViewNode * parent( ContentViewNode * node ) const;

	virtual void nodeChanged( ContentViewNode * node );
	virtual void beginInsertNode( ContentViewNode * n, int first, int last );
	virtual void beginRemoveNode( ContentViewNode * n, int first, int last );
	virtual void endInsertNode();
	virtual void endRemoveNode();
	//! Recalculate the element show with the list of showed eleemnt.
	virtual void reset();
private:
	typedef QPair<ContentViewNode*,ContentViewNode*> ContentViewNodePair;
	QHash< ContentViewNode*, ContentViewNode* > m_parents;

	ContentViewNode * m_parent;
	int m_first, m_last;

	QList<ContentViewNode*> m_nodes, m_showedNodes;
};

#endif /* _COMPLETIONNODEMODEL_H_ */
