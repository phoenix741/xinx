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
#include "rcsmanager.h"
#include <core/xinxcore.h>
#include <plugins/xinxpluginsloader.h>

/* Constante */

RCSManager * RCSManager::s_self = 0;

/* RCSManager */

RCSManager::RCSManager() {
}

RCSManager::~RCSManager() {
	if( s_self == this )
		s_self = 0;
}

RCSManager * RCSManager::self() {
	if( ! s_self ) {
		s_self = new RCSManager();
		XINXStaticDeleter::self()->add( s_self );
	}
	return s_self;
}

QMap<QString,QString> RCSManager::revisionControlIds() const {
	QMap<QString,QString> result;
	foreach( XinxPluginElement * element, XinxPluginsLoader::self()->plugins() ) {
		IRCSPlugin * interface = qobject_cast<IRCSPlugin*>( element->plugin() );
		if( element->isActivated() && interface ) {
			foreach( const QString & rcsKey, interface->rcs() ) {
				result.insert( rcsKey, interface->descriptionOfRCS( rcsKey ) );
			}
		}
	}
	return result;
}

RCS * RCSManager::createRevisionControl( QString revision, QString basePath ) const {
	RCS * rcs = NULL;
	foreach( XinxPluginElement * element, XinxPluginsLoader::self()->plugins() ) {
		IRCSPlugin * interface = qobject_cast<IRCSPlugin*>( element->plugin() );
		if( element->isActivated() && interface ) {
			rcs = interface->createRCS( revision, basePath );
			if( rcs ) break;
		}
	}
	return rcs;
}

bool RCSManager::setCurrentRCS( const QString & rcs, const QString & rootPath ) {
	if( rcs != m_rcsName ) {
		if( m_rcs ) {
			delete m_rcs;
			m_rcs     = 0;
			m_rcsName = QString();
		}

		if( ! rcs.isEmpty() ) {
			m_rcs     = createRevisionControl( rcs, rootPath );
			m_rcsName = m_rcs ? rcs : QString();
			if( ! m_rcs )
				return false;
		} 
	}
	return true;
}

QString RCSManager::currentRCS() const {
	return m_rcsName;
}

QString RCSManager::description() const {
	return revisionControlIds().value( m_rcsName );
}

void RCSManager::addOperation( rcsManagerOperation op, const QStringList & filename ) {
	m_operations.append( qMakePair( op, filename ) );
}

void RCSManager::validOperations() {
	if( m_rcs ) {
		while( m_operations.size() ) {
			QPair<rcsManagerOperation,QStringList> operation = m_operations.dequeue();
			switch( operation.first ) {
			case RCSManager::RCS_ADD:
				m_rcs->add( operation.second );
				break;
			case RCSManager::RCS_REMOVE:
				m_rcs->remove( operation.second );
				break;
			case RCSManager::RCS_UPDATE:
				m_rcs->update( operation.second );
				break;
			}
		}
	} else {
		m_operations.clear();
	}
}

void RCSManager::abort() {
}

void RCSManager::rollbackOperations() {
	m_operations.clear();
}


