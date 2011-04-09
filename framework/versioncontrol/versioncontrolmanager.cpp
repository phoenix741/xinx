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
#include <plugins/interfaces/versioncontrol.h>
#include <plugins/xinxpluginelement.h>
#include <plugins/xinxpluginsloader.h>
#include <versioncontrol/versioncontrolmanager.h>
#include <project/xinxprojectmanager.h>
#include <project/xinxprojectproject.h>
#include <versioncontrol/rcsproxy.h>

// Qt header
#include <QAction>
#include <QApplication>
#include <QtConcurrentRun>

namespace VersionControl
{

Manager::Manager() : _interface(0), _current_rcs(0)
{
}

void Manager::initialisation()
{
	// Global Update
	m_updateAll = new QAction(QIcon(":/images/vcs_update.png"), tr("Update project"), this);
	m_updateAll->setWhatsThis(tr("Update all files of the project directory (from repository)."));
	m_updateAll->setShortcut(QKeySequence::Refresh);
	m_updateAll->setEnabled(false);
	connect(m_updateAll, SIGNAL(triggered()), this, SLOT(updateWorkingCopy()));

	// Global Commit
	m_commitAll = new QAction(QIcon(":/images/vcs_commit.png"), tr("Commit project"), this);
	m_commitAll->setWhatsThis(tr("<p>Commit all files of the project directory (to repository). An optional message can be added.</p>\n"
								 "<p><i>Only <b>XINX</b> managed files are commited to the repository.</i></p>"));
	m_commitAll->setShortcut(QKeySequence("F6"));
	m_commitAll->setEnabled(false);
	connect(m_commitAll, SIGNAL(triggered()), this, SLOT(validWorkingCopy()));

	// Cancel
	m_abort = new QAction(QIcon(":/images/button_cancel.png"), tr("Cancel RCS operation"), this);
	m_abort->setShortcut(QKeySequence("Escape"));
	m_abort->setEnabled(false);
	connect(m_abort, SIGNAL(triggered()), this, SLOT(abort()));

	connect(XinxProject::Manager::self(), SIGNAL(selectionChanged(XinxProject::ProjectPtr)), this, SLOT(changed(XinxProject::ProjectPtr)));

	connect(&m_rcsWatcher, SIGNAL(started()), this, SLOT(startOperations()));
	connect(&m_rcsWatcher, SIGNAL(started()), this, SLOT(updateActions()));
	connect(&m_rcsWatcher, SIGNAL(finished()), this, SLOT(stopOperations()));
	connect(&m_rcsWatcher, SIGNAL(finished()), this, SLOT(updateActions()));
}

//! Return the action that can be used to update the repository
QAction * Manager::updateAllAction() const
{
	return m_updateAll;
}

//! Return the action that can be used to commit the repository
QAction * Manager::commitAllAction() const
{
	return m_commitAll;
}

//! Call the action than can be used to cancel the execution of the operation
QAction * Manager::abortAction() const
{
	return m_abort;
}

/*!
 * List all the revision control that can be used. The Result is a list of
 * QPair. The first element is the key and the second element is the description.
 */
QMap<QString,QString> Manager::revisionControlIds() const
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

RCS * Manager::createRevisionControl(QString revision, QString basePath) const
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

	// We will automatically log all data from RCS
	if (rcs)
	{
		connect(rcs, SIGNAL(alert(RCS::rcsLog,QString)), this, SLOT(logOperation(RCS::rcsLog,QString)));
	}
	return rcs;
}

void Manager::setUserInterface(IManagerUI* interface)
{
	_interface = interface;
}

IManagerUI* Manager::userInterface() const
{
	return _interface;
}

void Manager::waitForFinished()
{
	m_rcsWatcher.waitForFinished();
}

//! Return true if an operation is executed
bool Manager::isExecuting() const
{
	return m_rcsWatcher.isRunning();
}

void Manager::addRemoveFiles(RCS * rcs, QStringList toAdd, QStringList toRemove)
{
	m_rcsWatcher.waitForFinished();
	m_rcsWatcher.setFuture(QtConcurrent::run(this, &Manager::callRCSFileOperations, rcs, toAdd, toRemove));
}

void Manager::validWorkingCopy(RCS * rcs, RCS::FilesOperation operations, QString messages)
{
	m_rcsWatcher.waitForFinished();
	m_rcsWatcher.setFuture(QtConcurrent::run(this, &Manager::callRCSValideWorkingCopy, rcs, operations, messages));
}

void Manager::updateWorkingCopy(RCS * rcs, QStringList list)
{
	m_rcsWatcher.waitForFinished();
	m_rcsWatcher.setFuture(QtConcurrent::run(this, &Manager::callRCSUpdateWorkingCopy, rcs, list));
}

void Manager::updateRevision(RCS * rcs, QString path, QString revision, QByteArray * content)
{
	m_rcsWatcher.waitForFinished();
	m_rcsWatcher.setFuture(QtConcurrent::run(this, &Manager::callRCSUpdateRevision, rcs, path, revision, content));
}

void Manager::changed(XinxProject::ProjectPtr project)
{
	bool rcs_enabled = project && project->rcsProxy() && ! project->projectRCS().isEmpty();
	RCS::rcsFeatures rcs_features = rcs_enabled ? project->rcsProxy()->currentRCSInterface()->features() : RCS::rcsFeatures();

	m_updateAll->setEnabled(rcs_enabled && ! isExecuting() && rcs_features.testFlag(RCS::RcsFeatureUpdateAndCommit));
	m_commitAll->setEnabled(rcs_enabled && ! isExecuting() && rcs_features.testFlag(RCS::RcsFeatureUpdateAndCommit));
	m_abort->setEnabled(rcs_enabled && isExecuting());
}

void Manager::updateActions()
{
	changed(XinxProject::Manager::self()->selectedProject().toStrongRef());
}

void Manager::updateWorkingCopy()
{
	XinxProject::ProjectPtr project = XinxProject::Manager::self()->selectedProject().toStrongRef();
	if (!project || project->projectRCS().isEmpty() || !project->rcsProxy()) return;

	project->rcsProxy()->updateWorkingCopy();
}

void Manager::validWorkingCopy()
{
	XinxProject::ProjectPtr project = XinxProject::Manager::self()->selectedProject().toStrongRef();
	if (!project || project->projectRCS().isEmpty() || !project->rcsProxy()) return;

	project->rcsProxy()->validWorkingCopy(QStringList(), qApp->activeWindow());
}

void Manager::abort()
{
	if (!_current_rcs) return;

	_current_rcs->abort();
}

void Manager::startOperations()
{
	if (userInterface())
	{
		userInterface()->startVersionControlOperations();
	}
}

void Manager::logOperation(RCS::rcsLog niveau, const QString& info)
{
	if (userInterface())
	{
		userInterface()->logVersionControlOperation(niveau, info);
	}
}

void Manager::stopOperations()
{
	if (userInterface())
	{
		userInterface()->stopVersionControlOperations();
	}
	_current_rcs = 0;

	emit finished();
}

/* Method executed by thread */

void Manager::callRCSUpdateWorkingCopy(RCS * rcs, QStringList list)
{
	rcs->update(list);
}

void Manager::callRCSFileOperations(RCS * rcs, QStringList toAdd, QStringList toRemove)
{
	if (! toAdd.isEmpty())
		rcs->add(toAdd);
	if (! toRemove.isEmpty())
		rcs->remove(toRemove);
}

void Manager::callRCSUpdateRevision(RCS * rcs, QString path, QString revision, QByteArray * content)
{
	rcs->updateToRevision(path, revision, content);
}

void Manager::callRCSValideWorkingCopy(RCS * rcs, RCS::FilesOperation operations, QString messages)
{
	QStringList toAdd, toRemove, toCommit;

	foreach(RCS::FileOperation operation, operations)
	{
		if (operation.operation == RCS::Nothing)
		{
			continue;
		}

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

};

