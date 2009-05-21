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
 * \file contentviewnode.h
 * \brief Contains the element for manage a node used in content view.
 */

#ifndef __CONTENTVIEWNODE_H__
#define __CONTENTVIEWNODE_H__

// Qt header
#include <QList>
#include <QHash>
#include <QAtomicInt>

class AbstractContentViewModel;

class ContentViewNode;

/*!
 * \class ContentViewNodeList
 * \brief List of ContentViewNode
 * List of node.
 * Instead of compare pointer, we compare the node.
 */
class ContentViewNodeList : public QList<ContentViewNode*> {
public:
	int indexOfObject( ContentViewNode* t, int from = 0 ) const;
	int indexOf( ContentViewNode* t, int from = 0 ) const;
	int lastIndexOf( ContentViewNode* t, int from = -1 ) const;
	bool contains( ContentViewNode* value ) const;
	int count( ContentViewNode* value ) const;
	int count() const;
};

/*!
 * \class ContentViewNode
 * \brief This class represent a node element for the content view
 *
 * This class is a node element for the content view. It can be added to another
 * content view element, and detached.
 *
 * This class isn't a QObject (so no signal neither slot) to down memory. To permit
 * add and remove, we define the used model.
 *
 * The object (if in the cache) can have many tuple parent-model. So, in some
 * case, we must call method with an id to know which tuple use. The id will be
 * the root element of the structure when needed.
 * All modification (set) must be warn to the model (layoutChanged)
 *
 * ContentViewNode is inspered from QCodeNode of Edyuk but is rewriting from null to
 * considere multiple modele and multiple parents.
 */
class ContentViewNode {
public:
	/*!
	 * To simplify, the content view node will be not subclassed. Else data will
	 * be stored in this structure as role. Other value (as mode, ...) can be stored
	 * as user value, if needed.
	 */
	enum RoleIndex {
			NODE_DISPLAY_NAME = 0,   //!< The waiting content is a QString.
			NODE_ICON         = 1,   //!< The waiting content is a QIcon.
			NODE_NAME         = 2,   //!< The waiting content is a QString.
			NODE_DISPLAY_TIPS = 3,   //!< The waiting content is a QString.
			NODE_TYPE         = 33,  //!< The waiting content is a QString.
			NODE_USER_VALUE   = 34,  //!< No waiting content.
	};

	/*!
	 * Create a content view node and initialize the node with a \e name and a \e line
	 * where the element can be find.
	 * If \e cachedNode is true, the parent is different for each id. Else the parent is
	 * only set on the id 0
	 */
	ContentViewNode( const QString & name, int line, bool cachedNode = false );


	/*!
	 * Used to delete this object. Call it to prevent to delete this object if member
	 * \e canBeDeleted is set to false.
	 * \see canBeDeleted(), setCanBeDeleted()
	 */
	void deleteInstance( unsigned long id = 0 );

	/*!
	 * Attach this node and all it's child to the parent node below. Use the parent model
	 * to know which model use.
	 * \param parent The parent node where this node is attached.
	 * \param id The id to use to known the context.
	 * \see detach()
	 */
	bool attach( ContentViewNode * parent, unsigned long id = 0 );

	/*!
	 * Detach the node from the parent, and from the model of the parent.
	 * \see detach(), attach()
	 */
	void detach( ContentViewNode * parent, unsigned long id = 0 );

	/*!
	 * Set the content view model for a given id. We can't set the model
	 * for a null id.
	 */
	static void addModel( AbstractContentViewModel * model, unsigned long id );

	//! Get the model for the given id \e id.
	static QList<AbstractContentViewModel*> models( unsigned long id );

	//! Remove the model from the attached list
	static void removeModel( AbstractContentViewModel * model, unsigned long id );

	//! Remove all models from the node (and it's child) for the given id.
	static void clearModels( unsigned long id );

	/*!
	 * Remove all elements of the list but not delete them.
	 * \see clear()
	 */
	void removeAll( unsigned long id = 0 );

	/*!
	 * Remove all elements of the list.
	 * \see removeAll()
	 */
	inline void clear() { removeAll(); }

	//! Return the register line for the node
	int line() const;
	//! Change the line of the node
	void setLine( int value );

	//! Return the current file name of the node. If not set, this is the current file name.
	const QString & fileName() const;
	//! Set the file name of the node with \e value.
	void setFileName( const QString & value );

	//! Return if the item can be deleted when reference counting down to 0
	bool isAutoDelete();
	//! Set to true if you want to delete this objet when reference counting down to 0
	void setAutoDelete( bool value );

	//! Return the data stored in the node for the given \e index
	QVariant data( int index = ContentViewNode::NODE_NAME ) const;
	//! Set the data for the givent \e index
	void setData( const QVariant & value, int index = ContentViewNode::NODE_NAME );

	/*!
	 * List of childs node of this node. Childs node are attach with method. If i call
	 * contains, the operator == must be call. (I hope)
	 * \e attach()
	 * \see attach(), detach()
	 */
	const ContentViewNodeList & childs() const;

	/*!
	 * Test if an element is equals to another ContentViewNode. If the \e node is equals
	 * the method return \e true.
	 *
	 * To know if two elements are equals, the method compare the datas content.
	 */
	bool operator==( const ContentViewNode & node ) const;

	/*!
	 * Copy the contents of an ContentViewNode \e node to self.
	 * Datas copied are the \e line, \e filename, and all elements
	 * in \e datas.
	 */
	ContentViewNode & operator=( const ContentViewNode & node );
private:
	/*!
	 * Only the object itself can auto destroy
	 */
	~ContentViewNode();

	static QList<AbstractContentViewModel*> callModelsLock( unsigned long id );
	static void callModelsUnlock( QList<AbstractContentViewModel*> models );

	void callModelsDataChanged( QList<AbstractContentViewModel*> models );

	static void callModelBeginInsertRows( ContentViewNode * node, int line, QList<AbstractContentViewModel*> models );
	static void callModelEndInsertRows( QList<AbstractContentViewModel*> models );

	static void callModelBeginRemoveRows( ContentViewNode * node, int firstLine, int lastLine, QList<AbstractContentViewModel*> models );
	static void callModelEndRemoveRows( QList<AbstractContentViewModel*> models );

	/* Variables */
	QAtomicInt m_referenceCounter;

	bool m_autoDelete, m_cachedNode;
	int m_line;
	QString m_filename;
	QHash<int,QVariant> m_datas;

	ContentViewNodeList m_childs;

	/* On partage les models pour une mise �  jours plus rapide */
	static QMultiHash<unsigned long, AbstractContentViewModel* > s_models;
};

extern bool ContentViewNodeListSort( ContentViewNode * d1, ContentViewNode * d2 );

#endif /* __CONTENTVIEWNODE_H__ */
