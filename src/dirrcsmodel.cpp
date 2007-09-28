/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Xinx header
#include "dirrcsmodel.h"
#include "globals.h"
#include "rcs_cvs.h"
#include "xslproject.h"

// Qt header
#include <QBrush>

/* DirRCSModel */

DirRCSModel::DirRCSModel( const QStringList & nameFilters, QDir::Filters filters, QDir::SortFlags sort, QObject * parent ) : QDirModel( nameFilters, filters, sort, parent ) {
	XINX_TRACE( "DirRCSModel", QString( "( %1, ... )" ).arg( nameFilters.join(";") ) );

	if( global.m_project && ( global.m_project->projectRCS() == XSLProject::CVS ) )  {
		m_rcs = new RCS_CVS( global.m_project->projectPath() );
		connect( m_rcs, SIGNAL(stateChanged(QString)), this, SLOT(refresh(QString)) );
	} else
		m_rcs = NULL;
}

DirRCSModel::DirRCSModel(QObject *parent) : QDirModel(parent) {
	XINX_TRACE( "DirRCSModel", QString( "( %1 )" ).arg( (unsigned long)parent, 0, 16 ) );
	
}

DirRCSModel::~DirRCSModel() {
	XINX_TRACE( "~DirRCSModel", "()" );

	delete m_rcs;
}

RCS * DirRCSModel::rcs() { 
	XINX_TRACE( "DirRCSModel::rcs", "()" );

	return m_rcs;
}

QVariant DirRCSModel::data(const QModelIndex &index, int role) const {
	XINX_TRACE( "DirRCSModel::data", QString( "( index, %1 )" ).arg( role ) );

	QString path = filePath(index);
	if( m_rcs ) {
		if ( role == Qt::BackgroundRole && index.column() == 0 ) {
			RCS::rcsState state = m_rcs->status( path );
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
			RCS::rcsState state = m_rcs->status( path );
			QString filedate = m_rcs->infos( path, RCS::rcsFileDate ).toString(),
					date     = m_rcs->infos( path, RCS::rcsDate ).toString(),
					version  = m_rcs->infos( path, RCS::rcsVersions ).toString(),
					status;
   			if( state == RCS::Unknown )
				status = tr("Unknown");
			else if( state == RCS::LocallyModified )
				status = tr("Locally modified");
   			else if( state == RCS::LocallyAdded )
				status = tr("Locally added");
			else if( ( state == RCS::UnresolvedConflict ) || ( state == RCS::FileHadConflictsOnMerge ) )
				status = tr("Has conflict");
			else 
				status = tr("No modified");
		
			QString tips = tr("Status : %1\n"
		                  	  "Date of file : %2\n"
		                  	  "Date in CVS : %3\n"
		                  	  "Version : %4").arg( status ).arg( filedate ).arg( date ).arg( version );
			return tips;
		}
	}
	return QDirModel::data(index, role);
}

void DirRCSModel::refresh( const QString & path ) {
	XINX_TRACE( "DirRCSModel::refresh", QString( "( %1 )" ).arg( path ) );

	QDirModel::refresh( index( path ) );
}
