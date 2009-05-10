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

// Qt header
#include <QMutexLocker>

/* AbstractContentViewModel */

AbstractContentViewModel::AbstractContentViewModel( ContentViewNode * root, QObject *parent ) : QAbstractItemModel( parent ), m_rootNode( root ) {
	QMutexLocker locker( &mutex() );
	m_rootNode->addModel( this, (unsigned long)m_rootNode );
}

AbstractContentViewModel::~AbstractContentViewModel() {
	QMutexLocker locker( &m_updateMutex );
	m_rootNode->removeModel( this, (unsigned long)m_rootNode );
}

ContentViewNode * AbstractContentViewModel::rootNode() const {
	return m_rootNode;
}

QMutex & AbstractContentViewModel::mutex() {
	return m_updateMutex;
}

