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
#include "dirrcsmodel.h"
#include "xslproject.h"
#include <xinxpluginsloader.h>

// Qt header
#include <QBrush>
#include <QDateTime>

/* RCSCachedElement */

class RCSCachedElement {
public:
	QString fileName, version;
	RCS::rcsState state;
	QDateTime filedate, rcsdate;
	
	QString stringState() const;
};

QString RCSCachedElement::stringState() const {
	if( state == RCS::Unknown )
		return DirRCSModel::tr("Unknown");
	if( state == RCS::LocallyModified )
		return DirRCSModel::tr("Locally modified");
	if( state == RCS::LocallyAdded )
		return DirRCSModel::tr("Locally added");
	if( ( state == RCS::UnresolvedConflict ) || ( state == RCS::FileHadConflictsOnMerge ) )
		return DirRCSModel::tr("Has conflict");
	return DirRCSModel::tr("No modified");
}

/* DirRCSModel */

DirRCSModel::DirRCSModel( const QStringList & nameFilters, QDir::Filters filters, QDir::SortFlags sort, QObject * parent ) : QDirModel( nameFilters, filters, sort, parent ), m_cache( 500 ) {
	if( XINXProjectManager::self()->project() && ( !XINXProjectManager::self()->project()->projectRCS().isEmpty() ) )  {
		QString rcsKey = XINXProjectManager::self()->project()->projectRCS();
		m_rcs = XinxPluginsLoader::self()->createRevisionControl( rcsKey, XINXProjectManager::self()->project()->projectPath() );
		connect( m_rcs, SIGNAL(stateChanged(QString)), this, SLOT(refresh(QString)) );
	} else
		m_rcs = NULL;
}

DirRCSModel::DirRCSModel(QObject *parent) : QDirModel(parent) {
	
}

DirRCSModel::~DirRCSModel() {
	delete m_rcs;
}

RCS * DirRCSModel::rcs() {
	return m_rcs;
}

RCSCachedElement DirRCSModel::cachedValue( const QString & key ) const {
	QString path = QFileInfo( key ).absoluteFilePath();
	RCSCachedElement * value = m_cache.object( path );
	if( value ) {
		if( value->filedate == QFileInfo( path ).lastModified() )
			return *value;
		else
			m_cache.remove( path );
	}
	
	value = new RCSCachedElement;
	RCS::struct_rcs_infos infos = m_rcs->infos( path );

	value->fileName = path;
	value->state    = infos.state;
	value->filedate = QFileInfo( path ).lastModified();
	value->rcsdate  = infos.rcsDate.toLocalTime();
	value->version  = infos.version;
	m_cache.insert( path, value );
	return *value;
}

QVariant DirRCSModel::data(const QModelIndex &index, int role) const {
	if( ! index.isValid() ) return QVariant();
	
	QString path = filePath(index);
	if( m_rcs ) {
		if ( role == Qt::BackgroundRole && index.column() == 0 ) {
			RCSCachedElement element = cachedValue( path );
			RCS::rcsState state = element.state;
			if( state == RCS::Unknown )
				return QBrush( Qt::gray );
			if( state == RCS::LocallyModified )
				return QBrush( Qt::yellow );
			if( state == RCS::LocallyAdded )
				return QBrush( Qt::green );
			if( ( state == RCS::UnresolvedConflict ) || ( state == RCS::FileHadConflictsOnMerge ) )
				return QBrush( Qt::red );
			
			return QDirModel::data(index, role);
		} else
		if( role == Qt::ToolTipRole && index.column() == 0 ) {
			RCSCachedElement element = cachedValue( path );
			QString filedate = element.filedate.toString(),
					date     = element.rcsdate.toString(),
					version  = element.version,
					status   = element.stringState();
		
			QString tips = tr("Filename : %1\n"
							  "Status : %2\n"
		                  	  "Date of file : %3\n"
		                  	  "Date in CVS : %4\n"
		                  	  "Version : %5\n\n"
		                  	  "Cache size : %6/%7").arg( QFileInfo( path ).fileName() ).arg( status ).arg( filedate ).arg( date ).arg( version ).arg( m_cache.totalCost() ).arg( m_cache.maxCost() );
			return tips;
		}
	}
	return QDirModel::data(index, role);
}

void DirRCSModel::refresh( const QString & path ) {
	m_cache.remove( QFileInfo( path ).absoluteFilePath() );
	QModelIndex idx = index( path );
	if( idx.isValid() )
		QDirModel::refresh( idx );
}
