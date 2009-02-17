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

// Xinx header
#include "contentview/contentviewmodel.h"
#include "contentview/contentviewnode.h"

/* AbstractContentViewModel */

AbstractContentViewModel::AbstractContentViewModel( ContentViewNode * root, QObject *parent ) : QAbstractItemModel( parent ), m_rootNode( root ) {
	m_rootNode->setModel( this, (unsigned long)m_rootNode );
}

AbstractContentViewModel::~AbstractContentViewModel() {
	m_rootNode->setModel( 0, (unsigned long)m_rootNode );
}

ContentViewNode * AbstractContentViewModel::rootNode() const {
	return m_rootNode;
}

void AbstractContentViewModel::nodeChanged( ContentViewNode * node ) {
	emit dataChanged( index( node ), index( node ) );
}

void AbstractContentViewModel::beginInsertNode( ContentViewNode * node, int first, int last ) {
	beginInsertRows( index( node ), first, last );
}

void AbstractContentViewModel::beginRemoveNode( ContentViewNode * node, int first, int last ) {
	beginRemoveRows( index( node ), first, last );
}

void AbstractContentViewModel::endInsertNode() {
	endInsertRows();
}

void AbstractContentViewModel::endRemoveNode() {
	endRemoveRows();
}
