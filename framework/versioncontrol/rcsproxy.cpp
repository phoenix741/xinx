/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Xinx header
#include "rcsproxy.h"
#include <versioncontrol/versioncontrolmanager.h>
#include <core/xinxcore.h>
#include <plugins/xinxpluginsloader.h>
#include <core/xinxconfig.h>
#include <versioncontrol/commitmessagedialogimpl.h>
#include <project/xinxprojectproject.h>

// Qt header
#include <QMessageBox>

/*!
 * \defgroup RCS Revision Control System
 */
#include "logdialogimpl.h"

namespace VersionControl
{

/* RCSManager */

/*!
 * \ingroup RCS
 * \class RCSProxy
 * \brief The RCS Manager is the new interface to use to interact with RCS plugin.
 *
 * On validation :
 *   The class must search file to add and file to remove, and show a dialog to
 *   the user. After the validation of the dialog, the class must add, and remove files
 *   and commit the class.
 */

/*!
 * \enum RCSProxy::rcsAddRemoveOperation
 * \brief Operations on file that XINX can made.
 */

/*!
 * \var RCSProxy::rcsAddRemoveOperation RCSProxy::RCS_ADD
 * \brief Add a file to RCS
 */

/*!
 * \var RCSProxy::rcsAddRemoveOperation RCSProxy::RCS_REMOVE
 * \brief Remove a file to the RCS
 */


/*!
 * \brief Create a RCSProxy with a \e project.
 * \param project The project used to create the project
 *
 */
RCSProxy::RCSProxy(XinxProject::ProjectPtrWeak project) : m_project(project)
{

}

//! Destroy the RCS Manager
RCSProxy::~RCSProxy()
{

}

/*!
 * \brief Method called by XINX to set a Revision control system.
 * \param rcs The new Revision Controle System to use
 */
bool RCSProxy::setCurrentRCS(const QString & rcs)
{
	if (rcs != m_rcsName)
	{
		if (m_rcs)
		{
			VersionControl::Manager::self()->waitForFinished();

			m_rcs.reset();
			m_rcsName = QString();
		}

		if (! rcs.isEmpty())
		{
			try
			{
				m_rcs.reset(Manager::self()->createRevisionControl(rcs, m_rootPath));
				m_rcsName = m_rcs ? rcs : QString();
				if (m_rcs.isNull())
				{
					return false;
				}
				connect(m_rcs.data(), SIGNAL(stateChanged(QString, RCS::struct_rcs_infos)), this, SIGNAL(stateChange(QString, RCS::struct_rcs_infos)));
			}
			catch (ToolsNotDefinedException e)
			{
				m_rcs.reset();
				m_rcsName = QString();
				VersionControl::Manager::self()->logOperation(RCS::LogApplication, tr("No tools defined"));
				return false;
			}
		}
	}
	return true;
}

//! Return the name of the current RCS
QString RCSProxy::currentRCS() const
{
	return m_rcsName;
}

//! Return the description of the current RCS
QString RCSProxy::description() const
{
	return Manager::self()->revisionControlIds().value(m_rcsName);
}

//! RCS Object
RCS * RCSProxy::currentRCSInterface() const
{
	return m_rcs.data();
}

//! Set the current root path
void RCSProxy::setCurrentRootPath(const QString & rootPath)
{
	if (m_rootPath != rootPath)
	{
		if (m_rcs)
		{
			m_rcs->setWorkingDirectory(rootPath);
		}
		m_rootPath = rootPath;
	}
}

//! Get the current root path
const QString & RCSProxy::currentRootPath() const
{
	return m_rootPath;
}

//! Add an operation to the the RCS
void RCSProxy::addFileOperation(rcsAddRemoveOperation op, const QStringList & filename, QWidget * parent, bool confirm)
{
	QStringList filenameList = filename;
	if ((! XINXConfig::self()->config().rcs.autoAddFileToVersionningSystem) && confirm && (op == RCSProxy::RCS_ADD))
	{
		QMutableStringListIterator it(filenameList);
		while (it.hasNext())
		{
			const QString text = it.next();

			if (QMessageBox::question(parent, tr("Add a file"), tr("Do you want to add the file '%1' to the repository ?").arg(text), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
			{
				it.remove();
			}
		}
	}

	m_operations.append(qMakePair(op, filenameList));
}

//! Validate all operation to the RCS (made in a separate thread).
void RCSProxy::validFileOperations()
{
	VersionControl::Manager::self()->waitForFinished();

	if (m_rcs)
	{
		QStringList fileToAdd, fileToRemove;
		while (m_operations.size())
		{
			QPair<rcsAddRemoveOperation,QStringList> operation = m_operations.dequeue();

			switch (operation.first)
			{
			case RCSProxy::RCS_ADD:
				fileToAdd << operation.second;
				break;
			case RCSProxy::RCS_REMOVE:
				fileToRemove << operation.second;
				break;
			}
		}

		VersionControl::Manager::self()->addRemoveFiles(m_rcs.data(), fileToAdd, fileToRemove);

	}
	else
	{
		m_operations.clear();
	}
}

//! Rollback all operation to the RCS (return imediatly).
void RCSProxy::rollbackFileOperations()
{
	m_operations.clear();
}

//! Valide the working copy
void RCSProxy::validWorkingCopy(QStringList files, QWidget * parent)
{
	XinxProject::ProjectPtr project = m_project.toStrongRef();

	Q_ASSERT(m_rcs);
	Q_ASSERT(project);

	VersionControl::Manager::self()->waitForFinished();

	QString changeLog;
	CommitMessageDialogImpl dlg(parent);
	RCS::FilesOperation operations;

	dlg.setXinxProject(project);
	if (files.count() == 0)
	{
		files << project->projectPath();
	}

	if (files.count() > 0)
	{
		operations = m_rcs->operations(files);

		if (XINXConfig::self()->config().rcs.createChangelog)
		{
			changeLog = QDir(project->projectPath()).absoluteFilePath("changelog");
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

		VersionControl::Manager::self()->validWorkingCopy(m_rcs.data(), dlg.filesOperation(), message);
	}
	else
	{
		VersionControl::Manager::self()->logOperation(RCS::LogApplication, tr("Operation cancelled"));
	}
}

//! Update the working copy
void RCSProxy::updateWorkingCopy(QStringList list)
{
	Q_ASSERT(m_rcs);

	VersionControl::Manager::self()->waitForFinished();

	if (list.count() == 0)
		list << m_project.toStrongRef()->projectPath();

	VersionControl::Manager::self()->updateWorkingCopy(m_rcs.data(), list);
}

//! Update one file to the given revision
void RCSProxy::updateToRevision(const QString & path, const QString & revision, QByteArray * content)
{
	Q_ASSERT(m_rcs);

	VersionControl::Manager::self()->waitForFinished();
	VersionControl::Manager::self()->updateRevision(m_rcs.data(), path, revision, content);
}

//! Get the log of a path
void RCSProxy::log(const QString& path, QWidget * parent)
{
	Q_ASSERT(m_rcs);

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	LogDialogImpl dlg(parent);
	dlg.setLogEntries(m_rcs->log(path));

	QApplication::restoreOverrideCursor();

	dlg.exec();
}

//! Abort all the opreration
void RCSProxy::abort()
{
	VersionControl::Manager::self()->abort();
}

}
