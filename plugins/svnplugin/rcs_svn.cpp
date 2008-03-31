/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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
#include "rcs_svn.h"
#include <xinxconfig.h>

// Qt header
#include <QProcess>
#include <QTextStream>
#include <QDebug>

/* RCS_SVN */

RCS_SVN::RCS_SVN( const QString & basePath ) : RCS( basePath ) {
	
}

RCS_SVN::~RCS_SVN() {
	
}
	
RCS::struct_rcs_infos RCS_SVN::infos( const QString & path ) { 
	RCS::struct_rcs_infos rcsInfos = { RCS::Unknown, "0.0", QDateTime() };
	try {
		QProcess process;
		process.start( XINXConfig::self()->getTools( "svn", false ), QStringList() << "status" << "-vuN" << path );
		process.waitForStarted();
		if( process.error() == QProcess::FailedToStart ) {
			return rcsInfos;
		}
		process.waitForFinished();
		QStringList processResult = QString(process.readAllStandardOutput()).split( "\n" );
		if( processResult.count() == 0 ) return rcsInfos;
		QString statutFile = processResult.at(0);
		if( statutFile.length() < 8 ) return rcsInfos;
		
		rcsInfos.state = RCS::Updated;
		switch( statutFile.at( 0 ).toAscii() ) {
		case 'A' :
			rcsInfos.state = RCS::LocallyAdded;
			break;
		case 'D' :
			rcsInfos.state = RCS::LocallyRemoved;
			break;
		case 'C' :
			rcsInfos.state = RCS::UnresolvedConflict;
			break;
		case 'M' :
		case 'R' : 
			rcsInfos.state = RCS::LocallyModified;
			break;
		case 'X' :
		case '?' :
		case '~' :
		case '!' :
			rcsInfos.state = RCS::Unknown;
			break;
		default:
			;
		}
		if( ( rcsInfos.state == RCS::Updated ) && ( statutFile.at( 7 ) == '*' ) )
			rcsInfos.state = RCS::NeedsCheckout;
		QStringList eol = statutFile.mid( 8 ).simplified().split( " " );
		if( eol.size() > 1 ) rcsInfos.version = eol.at( 0 );
		
	} catch( ToolsNotDefinedException e ) {
	}
	return rcsInfos;
}

RCS::FilesOperation RCS_SVN::operations( const QStringList & path ) {
	return QList<FileOperation>();
}

void RCS_SVN::update( const QStringList & path ) {
	
} 

void RCS_SVN::commit( const RCS::FilesOperation & path, const QString & message ) {
	
}

void RCS_SVN::add( const QStringList & path ) {
	
}

void RCS_SVN::remove( const QStringList & path ) {
	
}

void RCS_SVN::updateToRevision( const QString & path, const QString & revision, QString * content ) {
	
}
	
void RCS_SVN::abort() {
	
}
