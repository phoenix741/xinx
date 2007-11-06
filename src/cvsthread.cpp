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
#include "cvsthread.h"
#include "globals.h"
#include "xinxconfig.h"

// Qt header
#include <QFileInfo>

// Std header
#ifdef Q_WS_WIN
	#include <windows.h>
#endif

/* CVSThread */

CVSThread::CVSThread( QStringList paths, bool terminate ) : XinxThread() {
	m_process = NULL;
	m_paths = paths;
	m_terminate = terminate;
}

CVSThread::~CVSThread() {
	terminate();
	wait();
	delete m_process;
}

void CVSThread::processLine( const QString & line ) {
	QString lline = line.simplified();
	
	if( lline.startsWith( "M " ) )
		emit log( RCS::LogLocallyModified, lline );
	else if( lline.startsWith( "A " ) )
		emit log( RCS::LogLocallyModified, lline );
	else if( lline.startsWith( "R " ) )
		emit log( RCS::LogLocallyModified, lline );
	else if( lline.startsWith( "U " ) )
		emit log( RCS::LogRemotlyModified, lline );
	else if( lline.startsWith( "P " ) )
		emit log( RCS::LogRemotlyModified, lline );
	else if( lline.startsWith( "C " ) )
		emit log( RCS::LogConflict, lline );
	else if( lline.startsWith( "? " ) )
		emit log( RCS::LogNotManaged, lline );
	else		
		emit log( RCS::LogNormal, lline );
}

void CVSThread::processReadOutput() {
	m_process->setReadChannel( QProcess::StandardOutput );
	while( m_process->canReadLine() ) 
		processLine( m_process->readLine() );
		
	/* Process error */
	m_process->setReadChannel( QProcess::StandardError );
	while( m_process->canReadLine() )
		emit log( RCS::LogError, m_process->readLine().simplified() );
}

void CVSThread::callCVS( const QString & path, const QStringList & options ) {
	/* Create process */
	m_process = new QProcess( this );

	emit log( RCS::LogApplication, QString("Working dir : %1").arg( path ) );
	m_process->setWorkingDirectory( path );
	emit log( RCS::LogApplication, QString("%1 %2").arg( global.m_config->config().tools["cvs"] ).arg( options.join( " " ) ).simplified() );
	m_process->start( global.m_config->config().tools["cvs"], options, QIODevice::ReadWrite | QIODevice::Text );

	while( m_process->state() != QProcess::NotRunning ) {
		if( m_process->waitForReadyRead( 100 ) )
			processReadOutput();
	}
	processReadOutput();

	delete m_process;
	m_process = NULL;
}

void CVSThread::abort() {
	if( ( ! m_process ) || ( m_process->state() == QProcess::NotRunning ) ) return ;
	m_process->terminate();
#ifdef Q_WS_WIN
	if( GenerateConsoleCtrlEvent( CTRL_BREAK_EVENT, m_process->pid()->dwThreadId ) != 0 )
		perror( "GenerateConsoleCtrlEvent" );
	if( GenerateConsoleCtrlEvent( CTRL_C_EVENT, m_process->pid()->dwThreadId ) != 0 )
		perror( "GenerateConsoleCtrlEvent" );
	emit log( RCS::LogError, tr("I'M A PROCESS KILLER") );
	m_process->kill();
#endif
}

void CVSThread::threadrun() {
	if( m_paths.size() <= 0 ) return;
	if( m_paths.size() > 1 )
		m_paths.sort();
	
	int i = 0;
	QString path;
	QStringList files;

	do {
		files.clear();
		QFileInfo info = QFileInfo( m_paths.at( i ) );
		if( info.isDir() ) {
			path = m_paths.at( i );
		} else {
			path = info.absolutePath();
			files << info.fileName();
		}
		i++;
		QFileInfo infoNext; 
		while( ( i < m_paths.size() ) && ( ( infoNext = QFileInfo( m_paths.at( i ) ) ).absolutePath() == path ) ) {
			files << infoNext.fileName();
			i++;
		}
		callCVS( path, files );
	} while( i < m_paths.size() );
}

/* CVSUpdateThread */

CVSUpdateThread::CVSUpdateThread( QStringList paths, bool terminate ) : CVSThread( paths, terminate ) {
}

void CVSUpdateThread::callCVS( const QString & path, const QStringList & files ) {
	QStringList parameters;
	if( ! global.m_config->config().cvs.progressMessages.isEmpty() )
		parameters << global.m_config->config().cvs.progressMessages;
	parameters << QString("-z%1").arg( global.m_config->config().cvs.compressionLevel ) << "update";
	if( global.m_config->config().cvs.pruneEmptyDirectories )
		parameters << "-P";
	if( global.m_config->config().cvs.createDirectories )
		parameters << "-d";

	parameters << files;

	CVSThread::callCVS( path, parameters );
}

void CVSUpdateThread::threadrun() {
	CVSThread::threadrun();
		
	emit log( RCS::LogApplication, tr("Update terminated") );
	if( m_terminate )
		emit operationTerminated();
}

/* CVSUpdateRevisionThread */

CVSUpdateRevisionThread::CVSUpdateRevisionThread( const QString & path, const QString & revision, QString * content ) : CVSThread( QStringList() << path, true ), m_content( content ), m_revision( revision )  {
	
}

void CVSUpdateRevisionThread::processReadOutput() {
	if( ! m_content )
		CVSThread::processReadOutput();
	else {
		m_process->setReadChannel( QProcess::StandardOutput );
		while( m_process->canReadLine() ) { 
			m_content->append( m_process->readLine() );
		}
			
		/* Process error */
		m_process->setReadChannel( QProcess::StandardError );
		while( m_process->canReadLine() )
			emit log( RCS::LogError, m_process->readLine().simplified() );
	}
}

void CVSUpdateRevisionThread::callCVS( const QString & path, const QStringList & files ) {
	QStringList parameters;
	if( ! global.m_config->config().cvs.progressMessages.isEmpty() )
		parameters << global.m_config->config().cvs.progressMessages;
	parameters << QString("-z%1").arg( global.m_config->config().cvs.compressionLevel ) << "update";
	if( m_content != NULL )
		parameters << "-p";
	parameters << "-r" << m_revision; 

	parameters << files;

	CVSThread::callCVS( path, parameters );
}

void CVSUpdateRevisionThread::threadrun() {
	if( m_paths.size() == 1 && QFileInfo( m_paths[ 0 ] ).exists() && !QFileInfo( m_paths[ 0 ] ).isDir() ) 
		CVSThread::threadrun();
		
	emit log( RCS::LogApplication, tr("Update to revision %1 terminated").arg( m_revision ) );
	if( m_terminate )
		emit operationTerminated();
}

/* CVSAddThread */

CVSAddThread::CVSAddThread( QStringList paths, bool terminate ) : CVSThread( paths, terminate ) {
}


void CVSAddThread::callCVS( const QString & path, const QStringList & files ) {
	QStringList parameters;
	parameters << "add";
	parameters << files;

	CVSThread::callCVS( path, parameters );
}

void CVSAddThread::threadrun() {
	CVSThread::threadrun();
	
	emit log( RCS::LogApplication, tr("Add terminated") );
	if( m_terminate )
		emit operationTerminated();
}

/* CVSRemoveThread */

CVSRemoveThread::CVSRemoveThread( QStringList paths, bool terminate ) : CVSThread( paths, terminate ) {
}


void CVSRemoveThread::callCVS( const QString & path, const QStringList & files ) {
	QStringList parameters;
	parameters << "remove";
	parameters << files;
	
	CVSThread::callCVS( path, parameters );
}

void CVSRemoveThread::threadrun() {
	CVSThread::threadrun();
	
	emit log( RCS::LogApplication, tr("Remove terminated") );
	if( m_terminate )
		emit operationTerminated();
}

/* CVSCommitThread */

CVSCommitThread::CVSCommitThread( RCS::FilesOperation paths, QString message ) : CVSThread( QStringList(), true ) {
	m_message = message;
	m_message += "\n";
	m_message += "\n";
	m_message += tr("Files commited :") + "\n";
	m_message += "\n";
	foreach( RCS::FileOperation file, paths ) {
		if( file.second != RCS::Nothing ) {
			m_message += QFileInfo( file.first ).fileName() + "\n";
			m_paths  << file.first;
		}
		if( file.second == RCS::RemoveAndCommit ) 
			m_removeList  << file.first;
		if( file.second == RCS::AddAndCommit ) 
			m_addList  << file.first;
	}
}

void CVSCommitThread::callCVS( const QString & path, const QStringList & files ) {
	QStringList parameters;
	if( ! global.m_config->config().cvs.progressMessages.isEmpty() )
		parameters << global.m_config->config().cvs.progressMessages;
	parameters << QString("-z%1").arg( global.m_config->config().cvs.compressionLevel ) << "commit" << "-m" << m_message;

	parameters << files;
	
	CVSThread::callCVS( path, parameters );
}

void CVSCommitThread::threadrun() {
	if( m_addList.size() > 0 ) {
		CVSThread * thread = new CVSAddThread( m_addList, false );
		connect( thread, SIGNAL(log()), this, SLOT(log()) );
		thread->start();
		thread->wait();
		delete thread;
	}
	if( m_removeList.size() > 0 ) {
		CVSThread * thread = new CVSRemoveThread( m_removeList, false );
		connect( thread, SIGNAL(log()), this, SLOT(log()) );
		thread->start();
		thread->wait();
		delete thread;
	}

	CVSThread::threadrun();

	
	emit log( RCS::LogApplication, tr("Commit terminated") );
	if( m_terminate )
		emit operationTerminated();
}

