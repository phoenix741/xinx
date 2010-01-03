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
#include <core/xinxconfig.h>

// Qt header
#include <QMessageBox>

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

bool RCSManager::setCurrentRCS( const QString & rcs ) {
	if( rcs != m_rcsName ) {
		if( m_rcs ) {
			delete m_rcs;
			m_rcs     = 0;
			m_rcsName = QString();
		}

		if( ! rcs.isEmpty() ) {
			m_rcs     = createRevisionControl( rcs, m_rootPath );
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

void RCSManager::setCurrentRootPath( const QString & rootPath ) {
	if( m_rootPath == rootPath ) {
		if( m_rcs )
			m_rcs->setBasePath( rootPath );
		m_rootPath = rootPath;
	}
}

const QString & RCSManager::currentRootPath() const {
	return m_rootPath;
}

void RCSManager::addFileOperation( rcsAddRemoveOperation op, const QStringList & filename, QWidget * parent ) {
	QStringList filenameList = filename;
	if( ( ! XINXConfig::self()->config().rcs.autoAddFileToVersionningSystem ) && ( op == RCSManager::RCS_ADD ) ) {
		QMutableStringListIterator it( filenameList );
		while( it.hasNext() ) {
			const QString text = it.next();

			if( QMessageBox::question( parent, tr("Add a file"), tr("Do you want to add the file '%1' to the repository").arg( text ), QMessageBox::Yes | QMessageBox::No ) == QMessageBox::No ) {
				it.remove();
			}
		}
	}

	m_operations.append( qMakePair( op, filenameList ) );
}

void RCSManager::validFileOperations() {
	if( m_rcs ) {
		while( m_operations.size() ) {
			QPair<rcsAddRemoveOperation,QStringList> operation = m_operations.dequeue();
			switch( operation.first ) {
			case RCSManager::RCS_ADD:
				m_rcs->add( operation.second );
				break;
			case RCSManager::RCS_REMOVE:
				m_rcs->remove( operation.second );
				break;
			}
		}
	} else {
		m_operations.clear();
	}
}

void RCSManager::rollbackFileOperations() {
	m_operations.clear();
}

void RCSManager::validWorkingCopy( QWidget * parent ) {
}

void RCSManager::updateWorkingCopy() {
}

void RCSManager::loadWorkingCopyStatut( QStringList files ) {
}

void RCSManager::abort( QWidget * parent ) {
	Q_UNUSED( parent );

}


