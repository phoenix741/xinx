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
#include <rcs/commitmessagedialogimpl.h>
#include <project/xinxproject.h>

// Qt header
#include <QMessageBox>
#include <QtConcurrentRun>

/* Constante */

RCSManager * RCSManager::s_self = 0;

/* RCSManager */

RCSManager::RCSManager() : m_rcs( 0 ) {
	// Global Update
	m_updateAll = new QAction( QIcon(":/images/vcs_update.png"), tr( "Update project"), this );
	m_updateAll->setWhatsThis(tr( "Call the update fonction of your <i>revision control system</i> for all the project directory."));
	m_updateAll->setShortcut( QKeySequence::Refresh );
	connect( m_updateAll, SIGNAL(triggered()), this, SLOT(updateWorkingCopy()) );

	// Global Commit
	m_commitAll = new QAction( QIcon(":/images/vcs_commit.png"), tr( "Commit project"), this );
	m_commitAll->setWhatsThis(QApplication::translate("MainForm", "<p>Call the commit method of your <i>revision control sytem</i> for all the project directory. An optional message can be added.</p>\n"
		"<p><i>Only <b>XINX</b> managed files are commited to the repository.</i></p>", 0, QApplication::UnicodeUTF8));
	m_commitAll->setShortcut( QKeySequence( "F6" ) );
	connect( m_commitAll, SIGNAL(triggered()), this, SLOT(validWorkingCopy()) );

	// Cancel
	m_abort = new QAction( QIcon(":/images/button_cancel.png"), tr( "Cancel RCS operation"), this );
	m_abort->setShortcut( QKeySequence( "Escape" ) );
	connect( m_abort, SIGNAL(triggered()), this, SLOT(abort()) );

	connect( &m_rcsWatcher, SIGNAL(finished()), this, SIGNAL(operationTerminated()) );
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

QAction * RCSManager::updateAllAction() const {
	return m_updateAll;
}

QAction * RCSManager::commitAllAction() const {
	return m_commitAll;
}

QAction * RCSManager::abortAction() const {
	return m_abort;
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
			if( rcs )
				break;
		}
	}
	return rcs;
}

bool RCSManager::setCurrentRCS( const QString & rcs ) {
	if( rcs != m_rcsName ) {
		if( m_rcs ) {
			m_rcsWatcher.waitForFinished();
			delete m_rcs;
			m_rcs     = 0;
			m_rcsName = QString();
		}

		if( ! rcs.isEmpty() ) {
			m_rcs     = createRevisionControl( rcs, m_rootPath );
			m_rcsName = m_rcs ? rcs : QString();
			if( ! m_rcs )
				return false;
			connect( m_rcs, SIGNAL(log(RCS::rcsLog,QString)), this, SIGNAL(log(RCS::rcsLog,QString)) );
			connect( m_rcs, SIGNAL(stateChanged(QString,RCS::struct_rcs_infos)), this, SIGNAL(statusChange(QString,RCS::struct_rcs_infos)) );
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
			m_rcs->setWorkingDirectory( rootPath );
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

static void callRCSFileOperations( RCS * rcs, QStringList toAdd, QStringList toRemove ) {
	rcs->add( toAdd );
	rcs->remove( toRemove );
}

void RCSManager::validFileOperations() {
	m_rcsWatcher.waitForFinished();

	if( m_rcs ) {
		QStringList fileToAdd, fileToRemove;
		while( m_operations.size() ) {
			QPair<rcsAddRemoveOperation,QStringList> operation = m_operations.dequeue();

			switch( operation.first ) {
			case RCSManager::RCS_ADD:
				fileToAdd << operation.second;
				break;
			case RCSManager::RCS_REMOVE:
				fileToRemove << operation.second;
				break;
			}
		}

		emit operationStarted();
		m_rcsWatcher.setFuture( QtConcurrent::run( callRCSFileOperations, m_rcs, fileToAdd, fileToRemove ) );

	} else {
		emit operationStarted();
		m_operations.clear();
		emit operationTerminated();
	}
}

void RCSManager::rollbackFileOperations() {
	m_operations.clear();
}

static void callRCSValideWorkingCopy( RCS * rcs, RCS::FilesOperation operations, QString messages ) {
	QStringList toAdd, toRemove, toCommit;

	foreach( RCS::FileOperation operation, operations ) {
		if( operation.second == RCS::AddAndCommit ) {
			toAdd << operation.first;
		}
		if( operation.second == RCS::RemoveAndCommit ) {
			toRemove << operation.first;
		}
		toCommit << operation.first;
	}

	rcs->add( toAdd );
	rcs->remove( toRemove );
	rcs->commit( toCommit, messages );
}

void RCSManager::validWorkingCopy( QStringList files, QWidget * parent ) {
	Q_ASSERT( m_rcs );

	m_rcsWatcher.waitForFinished();

	emit operationStarted();

	QString changeLog;
	CommitMessageDialogImpl dlg( parent );
	RCS::FilesOperation operations;

	if( files.count() == 0 ) {
		if( XINXProjectManager::self()->project() )
			files << XINXProjectManager::self()->project()->projectPath();

	}

	if( files.count() > 0 ) {
		operations = m_rcs->operations( files );

		if( XINXConfig::self()->config().rcs.createChangelog ) {
			changeLog = QDir( XINXProjectManager::self()->project()->projectPath() ).absoluteFilePath( "changelog" );
			if( QFile::exists( changeLog ) )
				operations << qMakePair( changeLog, RCS::Commit );
			else
				operations << qMakePair( changeLog, RCS::AddAndCommit );
		}
	}

	dlg.setFilesOperation( operations );

	if( dlg.exec() ) {
		QString message = dlg.messages();

		if( XINXConfig::self()->config().rcs.createChangelog ) {
			QFile changeLogFile( changeLog );
			if( changeLogFile.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) ) {
				QTextStream stream( &changeLogFile );
				stream << QDate::currentDate().toString( Qt::ISODate ) << " " << QTime::currentTime().toString( Qt::ISODate ) << " : ";
				if( message.isEmpty() )
					stream << tr( "<Commit with no text>" );
				else
					stream << message;
				stream << endl;
			}
		}

		m_rcsWatcher.setFuture( QtConcurrent::run( callRCSValideWorkingCopy, m_rcs, dlg.filesOperation(), message ) );
	} else {
		emit log( RCS::LogApplication, tr("Operation cancelled") );
		emit operationTerminated();
	}
}

static void callRCSUpdateWorkingCopy( RCS * rcs, QStringList list ) {
	rcs->update( list );
}

void RCSManager::updateWorkingCopy( QStringList list ) {
	Q_ASSERT( m_rcs );

	m_rcsWatcher.waitForFinished();

	emit operationStarted();

	if( list.count() == 0 )
		list << XINXProjectManager::self()->project()->projectPath();

	m_rcsWatcher.setFuture( QtConcurrent::run( callRCSUpdateWorkingCopy, m_rcs, list ) );
}

void RCSManager::loadWorkingCopyStatut( QStringList files ) {
}

void RCSManager::abort() {
	Q_ASSERT( m_rcs );

	if( m_rcsWatcher.isRunning() )
		m_rcs->abort();
}


