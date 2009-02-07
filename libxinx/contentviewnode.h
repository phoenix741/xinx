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

#ifndef __CONTENTVIEWCLASS_H__
#define __CONTENTVIEWCLASS_H__

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
 * case, we must call method with the model to use.
 */
class ContentViewNode {
public:

	/*!
	 * Attach this node and all it's child to the parent node below. If the model
	 * is'nt in the list, we add it (with setModel).
	 * \param parent The parent node where this node is attached.
	 * \param model The model to use. If not defined, the model of the parent \
	 * object is use. <i>If the model is different from the parent model this can \
	 * cause problem.</i>
	 */
	void attach( ContentViewNode * parent, ContentViewModel * model = 0 );

	//! Detach the node from the parent, and from the model of the parent.
	void detach( ContentViewNode * parent );

	//! Return the parent of this node for the given model.
	ContentViewNode * parent( ContentViewModel * model ) const;
private:
	QHash<ContentViewModel*, ContentViewNode*> m_parent;

	int m_line;
	QString m_name, m_filename;

	QList<ContentViewNode*> m_childs;
};

#endif /* __CONTENTVIEWCLASS_H__ */