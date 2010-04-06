/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

/*!
 * \defgroup RCS Revision Control System
 */

/* Constante */

RCSManager * RCSManager::s_self = 0;

/* RCSManager */

/*!
 * \ingroup RCS
 * \class RCSManager
 * \brief The RCS Manager is the new interface to use to interact with RCS plugin.
 *
 * On validation :
 *   The class must search file to add and file to remove, and show a dialog to
 *   the user. After the validation of the dialog, the class must add, and remove files
 *   and commit the class.
 */

/*!
 * \enum RCSManager::rcsAddRemoveOperation
 * \brief Operations on file that XINX can made.
 */

/*!
 * \var RCSManager::rcsAddRemoveOperation RCSManager::RCS_ADD
 * \brief Add a file to RCS
 */

/*!
 * \var RCSManager::rcsAddRemoveOperation RCSManager::RCS_REMOVE
 * \brief Remove a file to the RCS
 */


RCSManager::RCSManager() : m_rcs(0)
{
	// Global Update
	m_updateAll = new QAction(QIcon(":/images/vcs_update.png"), tr("Update project"), this);
	m_updateAll->setWhatsThis(tr("Call the update fonction of your <i>revision control system</i> for all the project directory."));
	m_updateAll->setShortcut(QKeySequence::Refresh);
	connect(m_updateAll, SIGNAL(triggered()), this, SLOT(updateWorkingCopy()));

	// Global Commit
	m_commitAll = new QAction(QIcon(":/images/vcs_commit.png"), tr("Commit project"), this);
	m_commitAll->setWhatsThis(QApplication::translate("MainForm", "<p>Call the commit method of your <i>revision control sytem</i> for all the project directory. An optional message can be added.</p>\n"
	                          "<p><i>Only <b>XINX</b> managed files are commited to the repository.</i></p>", 0, QApplication::UnicodeUTF8));
	m_commitAll->setShortcut(QKeySequence("F6"));
	connect(m_commitAll, SIGNAL(triggered()), this, SLOT(validWorkingCopy()));

	// Cancel
	m_abort = new QAction(QIcon(":/images/button_cancel.png"), tr("Cancel RCS operation"), this);
	m_abort->setShortcut(QKeySequence("Escape"));
	connect(m_abort, SIGNAL(triggered()), this, SLOT(abort()));

	connect(&m_rcsWatcher, SIGNAL(started()), this, SIGNAL(operationStarted()));
	connect(&m_rcsWatcher, SIGNAL(finished()), this, SIGNAL(operationTerminated()));

	connect(&m_rcsWatcher, SIGNAL(started()), this, SLOT(updateActions()));
	connect(&m_rcsWatcher, SIGNAL(finished()), this, SLOT(updateActions()));

	updateActions();
}

//! Destroy the RCS Manager
RCSManager::~RCSManager()
{
	if (s_self == this)
		s_self = 0;
}

//! Return the single instance of the RCSManager
RCSManager * RCSManager::self()
{
	if (! s_self)
	{
		s_self = new RCSManager();
		XINXStaticDeleter::self()->add(s_self);
	}
	return s_self;
}

//! Return the action that can be used to update the repository
QAction * RCSManager::updateAllAction() const
{
	return m_updateAll;
}

//! Return the action that can be used to commit the repository
QAction * RCSManager::commitAllAction() const
{
	return m_commitAll;
}

//! Call the action than can be used to cancel the execution of the operation
QAction * RCSManager::abortAction() const
{
	return m_abort;
}

/*!
 * List all the revision control that can be used. The Result is a list of
 * QPair. The first element is the key and the second element is the description.
 */
QMap<QString,QString> RCSManager::revisionControlIds() const
{
	QMap<QString,QString> result;
	foreach(XinxPluginElement * element, XinxPluginsLoader::self()->plugins())
	{
		IRCSPlugin * interface = qobject_cast<IRCSPlugin*>(element->plugin());
		if (element->isActivated() && interface)
		{
			foreach(const QString & rcsKey, interface->rcs())
			{
				result.insert(rcsKey, interface->descriptionOfRCS(rcsKey));
			}
		}
	}
	return result;
}

RCS * RCSManager::createRevisionControl(QString revision, QString basePath) const
{
	RCS * rcs = NULL;
	foreach(XinxPluginElement * element, XinxPluginsLoader::self()->plugins())
	{
		IRCSPlugin * interface = qobject_cast<IRCSPlugin*>(element->plugin());
		if (element->isActivated() && interface)
		{
			rcs = interface->createRCS(revision, basePath);
			if (rcs)
				break;
		}
	}
	return rcs;
}

/*!
 * Method called by XINX to set a Revision control system.
 * \param rcs The new Revision Controle System to use
 */
bool RCSManager::setCurrentRCS(const QString & rcs)
{
	if (rcs != m_rcsName)
	{
		if (m_rcs)
		{
			m_rcsWatcher.waitForFinished();
			delete m_rcs;
			m_rcs     = 0;
			m_rcsName = QString();
		}

		if (! rcs.isEmpty())
		{
			try
			{
				m_rcs     = createRevisionControl(rcs, m_rootPath);
				m_rcsName = m_rcs ? rcs : QString();
				if (! m_rcs)
				{
					updateActions();
					return false;
				}
				connect(m_rcs, SIGNAL(log(RCS::rcsLog,QString)), this, SIGNAL(log(RCS::rcsLog,QString)));
				connect(m_rcs, SIGNAL(stateChanged(QString,RCS::struct_rcs_infos)), this, SIGNAL(stateChange(QString,RCS::struct_rcs_infos)));
			}
			catch (ToolsNotDefinedException e)
			{
				m_rcs     = 0;
				m_rcsName = QString();
				emit log(RCS::LogApplication, tr("No tools defined"));
				updateActions();
				return false;
			}
		}
	}
	updateActions();
	return true;
}

//! Return the name of the current RCS
QString RCSManager::currentRCS() const
{
	return m_rcsName;
}

//! Return the description of the current RCS
QString RCSManager::description() const
{
	return revisionControlIds().value(m_rcsName);
}

//! RCS Object
RCS * RCSManager::currentRCSInterface() const
{
	return m_rcs;
}

//! Set the current root path
void RCSManager::setCurrentRootPath(const QString & rootPath)
{
	if (m_rootPath == rootPath)
	{
		if (m_rcs)
			m_rcs->setWorkingDirectory(rootPath);
		m_rootPath = rootPath;
	}
}

//! Get the current root path
const QString & RCSManager::currentRootPath() const
{
	return m_rootPath;
}

//! Return true if an operation is executed
bool RCSManager::isExecuting() const
{
	return m_rcsWatcher.isRunning();
}

//! Add an operation to the the RCS
void RCSManager::addFileOperation(rcsAddRemoveOperation op, const QStringList & filename, QWidget * parent, bool confirm)
{
	QStringList filenameList = filename;
	if ((! XINXConfig::self()->config().rcs.autoAddFileToVersionningSystem) && confirm && (op == RCSManager::RCS_ADD))
	{
		QMutableStringListIterator it(filenameList);
		while (it.hasNext())
		{
			const QString text = it.next();

			if (QMessageBox::question(parent, tr("Add a file"), tr("Do you want to add the file '%1' to the repository").arg(text), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
			{
				it.remove();
			}
		}
	}

	m_operations.append(qMakePair(op, filenameList));
}

void RCSManager::callRCSFileOperations(RCS * rcs, QStringList toAdd, QStringList toRemove)
{
	if (! toAdd.isEmpty())
		rcs->add(toAdd);
	if (! toRemove.isEmpty())
		rcs->remove(toRemove);
}

//! Validate all operation to the RCS (made in a separate thread).
void RCSManager::validFileOperations()
{
	m_rcsWatcher.waitForFinished();

	if (m_rcs)
	{
		QStringList fileToAdd, fileToRemove;
		while (m_operations.size())
		{
			QPair<rcsAddRemoveOperation,QStringList> operation = m_operations.dequeue();

			switch (operation.first)
			{
			case RCSManager::RCS_ADD:
				fileToAdd << operation.second;
				break;
			case RCSManager::RCS_REMOVE:
				fileToRemove << operation.second;
				break;
			}
		}

		m_rcsWatcher.setFuture(QtConcurrent::run(this, &RCSManager::callRCSFileOperations, m_rcs, fileToAdd, fileToRemove));

	}
	else
	{
		m_operations.clear();
	}
}

//! Rollback all operation to the RCS (return imediatly).
void RCSManager::rollbackFileOperations()
{
	m_operations.clear();
}

void RCSManager::callRCSValideWorkingCopy(RCS * rcs, RCS::FilesOperation operations, QString messages)
{
	QStringList toAdd, toRemove, toCommit;

	foreach(RCS::FileOperation operation, operations)
	{
		if (operation.operation == RCS::AddAndCommit)
		{
			toAdd << operation.filename;
		}
		if (operation.operation == RCS::RemoveAndCommit)
		{
			toRemove << operation.filename;
		}
		toCommit << operation.filename;
	}

	rcs->add(toAdd);
	rcs->remove(toRemove);
	rcs->commit(toCommit, messages);
}

//! Valide the working copy
void RCSManager::validWorkingCopy(QStringList files, QWidget * parent)
{
	Q_ASSERT(m_rcs);

	m_rcsWatcher.waitForFinished();

	QString changeLog;
	CommitMessageDialogImpl dlg(parent);
	RCS::FilesOperation operations;

	if (files.count() == 0)
	{
		if (XINXProjectManager::self()->project())
			files << XINXProjectManager::self()->project()->projectPath();

	}

	if (files.count() > 0)
	{
		operations = m_rcs->operations(files);

		if (XINXConfig::self()->config().rcs.createChangelog)
		{
			changeLog = QDir(XINXProjectManager::self()->project()->projectPath()).absoluteFilePath("changelog");
			if (QFile::exists(changeLog))
				operations << RCS::FileOperation(changeLog, RCS::Commit);
			else
				operations << RCS::FileOperation(changeLog, RCS::AddAndCommit);
		}
	}

	dlg.setFilesOperation(operations);

	if (dlg.exec())
	{
		QString message = dlg.messages();

		if (XINXConfig::self()->config().rcs.createChangelog)
		{
			QFile changeLogFile(changeLog);
			if (changeLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
			{
				QTextStream stream(&changeLogFile);
				stream << QDate::currentDate().toString(Qt::ISODate) << " " << QTime::currentTime().toString(Qt::ISODate) << " : ";
				if (message.isEmpty())
					stream << tr("<Commit with no text>");
				else
					stream << message;
				stream << endl;
			}
		}

		m_rcsWatcher.setFuture(QtConcurrent::run(this, &RCSManager::callRCSValideWorkingCopy, m_rcs, dlg.filesOperation(), message));
	}
	else
	{
		emit log(RCS::LogApplication, tr("Operation cancelled"));
	}
}

void RCSManager::callRCSUpdateWorkingCopy(RCS * rcs, QStringList list)
{
	rcs->update(list);
}

//! Update the working copy
void RCSManager::updateWorkingCopy(QStringList list)
{
	Q_ASSERT(m_rcs);

	m_rcsWatcher.waitForFinished();

	if (list.count() == 0)
		list << XINXProjectManager::self()->project()->projectPath();

	m_rcsWatcher.setFuture(QtConcurrent::run(this, &RCSManager::callRCSUpdateWorkingCopy, m_rcs, list));
}

void RCSManager::callRCSUpdateRevision(RCS * rcs, QString path, QString revision, QByteArray * content)
{
	rcs->updateToRevision(path, revision, content);
}

//! Update one file to the given revision
void RCSManager::updateToRevision(const QString & path, const QString & revision, QByteArray * content)
{
	Q_ASSERT(m_rcs);

	m_rcsWatcher.waitForFinished();

	m_rcsWatcher.setFuture(QtConcurrent::run(this, &RCSManager::callRCSUpdateRevision, m_rcs, path, revision, content));
}

//! Abort all the opreration
void RCSManager::abort()
{
	Q_ASSERT(m_rcs);

	if (m_rcsWatcher.isRunning())
		m_rcs->abort();
}

void RCSManager::updateActions()
{
	bool hasRCS     = (m_rcs != NULL);
	bool isExecute  = hasRCS && RCSManager::self()->isExecuting();

	m_updateAll->setEnabled(hasRCS && (!isExecute));
	m_commitAll->setEnabled(hasRCS && (!isExecute));
	m_abort->setEnabled(hasRCS && (!isExecute));
}

