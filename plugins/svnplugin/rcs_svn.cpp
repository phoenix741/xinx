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
#include <xinxpluginsloader.h>

// Qt header
#include <QTextStream>
#include <QRegExp>
#include <QDir>

// Std header
#ifdef Q_WS_WIN
	#include <windows.h>
#endif

/* RCS_SVN */

RCS_SVN::RCS_SVN( const QString & basePath ) : RCS( basePath ), m_content(0) {
	
}

RCS_SVN::~RCS_SVN() {
	
}
	
RCS::struct_rcs_infos RCS_SVN::infos( const QString & path ) { 
	RCS::struct_rcs_infos rcsInfos = { RCS::Unknown, "0.0", QDateTime() };
	try {
		QProcess process;
		process.setWorkingDirectory( QFileInfo( path ).absolutePath() ); 
		process.start( XINXConfig::self()->getTools( "svn", false ), QStringList() << "status" << "--non-interactive" << "-vuN" << path );
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
		QStringList eol = statutFile.mid( 7 ).simplified().split( " " );
		if( eol.size() > 1 ) rcsInfos.version = eol.at( 0 );
		
	} catch( ToolsNotDefinedException e ) {
	}
	return rcsInfos;
}

RCS::FilesOperation RCS_SVN::operations( const QString & path ) {
	QList<FileOperation> result;
	QProcess process;
	process.setWorkingDirectory( QFileInfo( path ).absolutePath() ); 
	process.start( XINXConfig::self()->getTools( "svn", false ), QStringList() << "status" << path );
	process.waitForStarted();
	if( process.error() == QProcess::FailedToStart ) return result;
	process.waitForFinished();
	QStringList processResult = QString(process.readAllStandardOutput()).split( "\n" );
	if( processResult.count() == 0 ) return result;
	
	foreach( QString pr, processResult ) {
		if( pr.isEmpty() ) continue;
		QString filename = QDir( path ).absoluteFilePath ( pr.mid(7).trimmed() );
		bool hasWilcard = false; 
		foreach( QString wilcard, XinxPluginsLoader::self()->defaultProjectFilter() ) {
			QRegExp projectWilcard( wilcard, Qt::CaseInsensitive, QRegExp::Wildcard );
			if( projectWilcard.exactMatch( filename ) ) { hasWilcard = true; break; }
		}
		if( !hasWilcard ) continue;
		switch( pr.at( 0 ).toAscii() ) {
		case 'A' :
		case 'D' :
		case 'M' :
		case 'R' : 
			result.append( qMakePair( filename, RCS::Commit ) );
			break;
		case 'X' :
		case '?' :
			result.append( qMakePair( filename, RCS::AddAndCommit ) );
			break;
		case '!' :
			result.append( qMakePair( filename, RCS::RemoveAndCommit ) );
			break;
		default:
			;
		}
	}
	return result;
}

RCS::FilesOperation RCS_SVN::operations( const QStringList & paths ) {
	QList<FileOperation> result;
	try {
		foreach( QString p, paths ) {
			result += operations( p );
		}
	} catch( ToolsNotDefinedException e ) {
	}
	return result;
}

void RCS_SVN::logMessages() {
	QStringList errors = QString( m_process->readAllStandardError() ).split( "\n" );
	foreach( QString error, errors ) {
		if( ! error.trimmed().isEmpty() )
			emit log( RCS::LogError, error.trimmed() );
	}		
	QStringList infos = QString( m_process->readAllStandardOutput() ).split( "\n" );
	foreach( QString info, infos ) {
		if( ! info.trimmed().isEmpty() )
			emit log( RCS::LogNormal, info.trimmed() );
	}
}

void RCS_SVN::finished( int exitCode, QProcess::ExitStatus exitStatus ) {
	emit log( RCS::LogApplication, tr("Process terminated") );
	emit operationTerminated();
	m_process->deleteLater();
}

void RCS_SVN::update( const QStringList & path ) {
	XINX_ASSERT( !m_process );
	
	try {
		QString tool = XINXConfig::self()->getTools( "svn", false );
		QStringList args = QStringList() << "update" << "--non-interactive" << path;
		m_process = new QProcess;
		connect( m_process, SIGNAL(readyReadStandardError()), this, SLOT(logMessages()) );
		connect( m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(logMessages()) );
		connect( m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)) );
		emit log( RCS::LogApplication, tool + " " + args.join( " " ) );
		m_process->start( tool, args );
		m_process->waitForStarted();
		if( m_process->error() == QProcess::FailedToStart ) {
			emit log( RCS::LogError, tr("Can't start svn program.") );
			delete m_process;
		}
	} catch( ToolsNotDefinedException e ) {
		emit log( RCS::LogError, tr("Can't find the svn program.") );
	}
} 

void RCS_SVN::commit( const RCS::FilesOperation & path, const QString & message ) {
	
}

void RCS_SVN::add( const QStringList & path ) {
	
}

void RCS_SVN::remove( const QStringList & path ) {
	
}

void RCS_SVN::updateToRevisionFinished( int exitCode, QProcess::ExitStatus exitStatus ) {
	QFile temporaryFile( m_tmpfilename );
	if( temporaryFile.open( QIODevice::ReadOnly ) ) {
		*m_content = QString( temporaryFile.readAll() );
	}
	temporaryFile.remove();

	emit log( RCS::LogApplication, tr("Process terminated") );
	emit operationTerminated();
	
	m_process->deleteLater();
}

void RCS_SVN::updateToRevision( const QString & path, const QString & revision, QString * content ) {
	// svn export -r rev path temporyfile
	m_content     = content;
	m_tmpfilename =  QDir( QDir::tempPath() ).absoluteFilePath( QFileInfo( path ).fileName() + "." + revision );
	try {
		QString tool = XINXConfig::self()->getTools( "svn", false );
		QStringList args = QStringList() << "export" << "--non-interactive" << "-r" << revision << path << m_tmpfilename;
		m_process = new QProcess;
		connect( m_process, SIGNAL(readyReadStandardError()), this, SLOT(logMessages()) );
		connect( m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(logMessages()) );
		connect( m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(updateToRevisionFinished(int,QProcess::ExitStatus)) );
		emit log( RCS::LogApplication, tool + " " + args.join( " " ) );

		m_process->setWorkingDirectory( QFileInfo( path ).absolutePath() ); 
		m_process->start( tool, args );
		m_process->waitForStarted();
		if( m_process->error() == QProcess::FailedToStart ) return;
		m_process->waitForFinished();
	} catch( ToolsNotDefinedException e ) {
	}
}
	
void RCS_SVN::abort() {
	if( ( ! m_process ) || ( m_process->state() == QProcess::NotRunning ) ) return ;
	m_process->terminate();
#ifdef Q_WS_WIN
	if( GenerateConsoleCtrlEvent( CTRL_BREAK_EVENT, m_process->pid()->dwThreadId ) != 0 )
		perror( "GenerateConsoleCtrlEvent" );
	if( GenerateConsoleCtrlEvent( CTRL_C_EVENT, m_process->pid()->dwThreadId ) != 0 )
		perror( "GenerateConsoleCtrlEvent" );
	emit log( RCS::LogError, tr("Can't stop the process on windows.") );
#endif
}