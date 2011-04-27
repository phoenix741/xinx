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
#include "rcs_cvs.h"
#include <core/xinxconfig.h>
#include <plugins/xinxpluginsloader.h>
#include "pluginsettings.h"

// Qt header
#include <QDir>

// Std header
#ifdef Q_WS_WIN
#include <windows.h>
#endif

/* RCS_CVS */

RCS_CVS::RCS_CVS(const QString & base) : RCS(base), m_content(0)
{
	m_entriesList = new EntriesList();
	m_watcher     = new FilesWatcher(this);
	m_cvs         = XINXConfig::self()->getTools("cvs");
	connect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(entriesStateChanged(QString)));
}

RCS_CVS::~RCS_CVS()
{
	delete m_entriesList;
}

RCS::rcsFeatures RCS_CVS::features() const
{
	return RCS::RcsFeatureAdd | RCS::RcsFeatureRemove | RCS::RcsFeatureUpdateAndCommit;
}

void RCS_CVS::setPluginSettings(PluginSettings * settings)
{
	m_settings = settings;
}

PluginSettings * RCS_CVS::pluginSettings() const
{
	return m_settings;
}

RCS::struct_rcs_infos RCS_CVS::info(const QString & path)
{
	RCS::struct_rcs_infos rcsInfos;
	QString localPath = QDir::fromNativeSeparators(path);
	EntriesLine e     = m_entriesList->status(localPath);
	rcsInfos.filename = localPath;
	rcsInfos.state    = e.status(QFileInfo(localPath).absolutePath());
	rcsInfos.rcsDate  = e.date;
	rcsInfos.version  = e.version;
	updateEntries();
	return rcsInfos;
}

QList<RCS::struct_rcs_infos> RCS_CVS::infos(const QString & path)
{
	if (QFileInfo(path).isFile())
	{
		return QList<RCS::struct_rcs_infos>() << info(path);
	}

	QList<RCS::struct_rcs_infos> result;
	QString absolutePath = QFileInfo(path).absoluteFilePath();
	const QList<EntriesLine> entries = m_entriesList->statusOfPath(absolutePath);

	QListIterator<EntriesLine> it(entries);
	while (it.hasNext())
	{
		const EntriesLine value = it.next();

		RCS::struct_rcs_infos rcsInfos;
		rcsInfos.filename = QDir(path).absoluteFilePath(value.filename);
		rcsInfos.state    = value.status(absolutePath);
		rcsInfos.rcsDate  = value.date;
		rcsInfos.version  = value.version;

		result << rcsInfos;
	}

	updateEntries();

	return result;
}

RCS::rcsOperation RCS_CVS::operationOfState(RCS::rcsState state)
{
	RCS::rcsOperation operation;
	switch (state)
	{
	case RCS::LocallyModified:
	case RCS::LocallyAdded:
	case RCS::LocallyRemoved:
		operation = RCS::Commit;
		break;
	case RCS::NeedsCheckout:
		operation = RCS::RemoveAndCommit;
		break;
	case RCS::Unknown:
		operation = RCS::AddAndCommit;
		break;
	case RCS::NeedPatch:
	case RCS::UnresolvedConflict:
	case RCS::FileHadConflictsOnMerge:
	case RCS::Updated:
	default:
		operation = RCS::Nothing;
	}
	return operation;
}

RCS::FilesOperation RCS_CVS::operationOf(const QString & path)
{
	RCS::FilesOperation operations;

	QStringList files = QDir(path).entryList(XinxPluginsLoader::self()->managedFilters(), QDir::Files);
	foreach(const QString & filename, files)
	{
		QString filepath = QDir(path).absoluteFilePath(filename);
		if (! QFileInfo(filepath).isDir())
		{
			RCS::rcsState state = m_entriesList->status(filepath).status(QFileInfo(filepath).absolutePath());
			RCS::rcsOperation operation = operationOfState(state);
			if (operation != RCS::Nothing)
			{
				RCS::FileOperation file(filepath, operation);
				operations.append(file);
			}
		}
	}
	EntriesFile file = m_entriesList->value(QDir(path).absoluteFilePath("CVS/Entries"));
	foreach(const EntriesLine & e, file)
	{
		RCS::rcsState state = e.status(path);
		if ((state != RCS::NeedsCheckout) && (state != RCS::LocallyRemoved)) continue;      // Géré au dessus
		RCS::rcsOperation operation = operationOfState(state);
		if (operation != RCS::Nothing)
		{
			RCS::FileOperation op(QDir(path).absoluteFilePath(e.filename), operation);
			operations.append(op);
		}
	}
	return operations;
}

RCS::FilesOperation RCS_CVS::recursiveOperationOf(const QString & path)
{
	RCS::FilesOperation operations;
	if (QFileInfo(path).isDir())
	{
		if (QFileInfo(QDir(path).absoluteFilePath("CVS/Entries")).exists())
		{
			operations += operationOf(path);

			QStringList infolist = QDir(path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			foreach(const QString & fileName, infolist)
			if (fileName != "CVS")
			{
				QString file = QDir(path).absoluteFilePath(fileName);
				operations += recursiveOperationOf(file);
			}
		}
	}
	else
	{
		RCS::rcsState state = m_entriesList->status(path).status(QFileInfo(path).absolutePath());
		RCS::rcsOperation operation = operationOfState(state);
		RCS::FileOperation file(path, operation);
		operations.append(file);
	}
	return operations;
}

RCS::FilesOperation RCS_CVS::operations(const QStringList & path)
{
	RCS::FilesOperation files;
	foreach(const QString & p, path)
	{
		files += recursiveOperationOf(p);
	}
	updateEntries();
	return files;
}

void RCS_CVS::update(const QStringList & path)
{
	Q_ASSERT((! m_process) || (m_process->state() == QProcess::NotRunning));

	try
	{
		QStringList parameters;
		if (! m_settings->config().progressMessages.isEmpty())
			parameters << m_settings->config().progressMessages;
		parameters << QString("-z%1").arg(m_settings->config().compressionLevel) << "update";
		if (m_settings->config().pruneEmptyDirectories)
			parameters << "-P";
		if (m_settings->config().createDirectories)
			parameters << "-d";

		callCVS(path, parameters);

		emit alert(RCS::LogApplication, tr("Update terminated"));
	}
	catch (ToolsNotDefinedException e)
	{
		emit alert(RCS::LogError, e.getMessage());
	}
}

void RCS_CVS::updateToRevision(const QString & path, const QString & revision, QByteArray * content)
{
	Q_ASSERT((! m_process) || (m_process->state() == QProcess::NotRunning));

	try
	{
		QStringList parameters;
		if (! m_settings->config().progressMessages.isEmpty())
			parameters << m_settings->config().progressMessages;
		parameters << QString("-z%1").arg(m_settings->config().compressionLevel) << "update";
		if (content != NULL)
			parameters << "-p";
		parameters << "-r" << revision;

		m_content = content;

		if ((! path.isEmpty()) && QFileInfo(path).exists() && ! QFileInfo(path).isDir())
			callCVS(QStringList() << path, parameters);

		m_content = 0;

		emit alert(RCS::LogApplication, tr("Update to revision %1 terminated").arg(revision));
	}
	catch (ToolsNotDefinedException e)
	{
		emit alert(RCS::LogError, e.getMessage());
	}
}

void RCS_CVS::commit(const QStringList & path, const QString & message)
{
	Q_ASSERT((! m_process) || (m_process->state() == QProcess::NotRunning));

	try
	{
		QString newMessage = message;
		newMessage += "\n";
		newMessage += "\n";
		newMessage += tr("Files commited :") + "\n";
		newMessage += "\n";
		foreach(const QString & file, path)
		{
			newMessage += QFileInfo(file).fileName() + "\n";
		}

		QStringList parameters;
		if (! m_settings->config().progressMessages.isEmpty())
			parameters << m_settings->config().progressMessages;
		parameters << QString("-z%1").arg(m_settings->config().compressionLevel) << "commit" << "-m" << newMessage;

		callCVS(path, parameters);

		emit alert(RCS::LogApplication, tr("Commit terminated"));
	}
	catch (ToolsNotDefinedException e)
	{
		emit alert(RCS::LogError, e.getMessage());
	}
}

void RCS_CVS::add(const QStringList & path)
{
	Q_ASSERT((! m_process) || (m_process->state() == QProcess::NotRunning));

	try
	{
		QStringList parameters;
		parameters << "add";

		callCVS(path, parameters);

		emit alert(RCS::LogApplication, tr("Add terminated"));
	}
	catch (ToolsNotDefinedException e)
	{
		emit alert(RCS::LogError, e.getMessage());
	}
}

void RCS_CVS::remove(const QStringList & path)
{
	Q_ASSERT((! m_process) || (m_process->state() == QProcess::NotRunning));

	try
	{
		QStringList parameters;
		parameters << "remove";

		callCVS(path, parameters);

		emit alert(RCS::LogApplication, tr("Remove terminated"));
	}
	catch (ToolsNotDefinedException e)
	{
		emit alert(RCS::LogError, e.getMessage());
	}
}

void RCS_CVS::abort()
{
	if ((! m_process) || (m_process->state() == QProcess::NotRunning)) return ;

	m_process->terminate();
#ifdef Q_WS_WIN
	if (GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, m_process->pid()->dwThreadId) != 0)
		perror("GenerateConsoleCtrlEvent");
	if (GenerateConsoleCtrlEvent(CTRL_C_EVENT, m_process->pid()->dwThreadId) != 0)
		perror("GenerateConsoleCtrlEvent");
	emit alert(RCS::LogError, tr("I'M A PROCESS KILLER"));
	m_process->kill();
#endif
}

void RCS_CVS::entriesStateChanged(const QString & path)
{
	const EntriesFile & e = m_entriesList->value(path);
	foreach(const EntriesLine & l, e)
	{
		emit stateChanged(QDir(e.path).absoluteFilePath(l.filename), info(QDir(e.path).absoluteFilePath(l.filename)));
	}
}

void RCS_CVS::updateEntries()
{
	QMutexLocker locker(&_mutex);

	if (m_watcher->files().size())
		m_watcher->removePaths(m_watcher->files());

	foreach(const EntriesFile & e, *m_entriesList)
	{
		if (e.size() > 0)
			m_watcher->addPath(QDir(e.path).absoluteFilePath("CVS/Entries"));
	}
}

void RCS_CVS::processLine(const QString & line)
{
	QString lline = line.simplified();

	if (lline.startsWith("M "))
		emit alert(RCS::LogLocallyModified, lline);
	else if (lline.startsWith("A "))
		emit alert(RCS::LogLocallyModified, lline);
	else if (lline.startsWith("R "))
		emit alert(RCS::LogLocallyModified, lline);
	else if (lline.startsWith("U "))
		emit alert(RCS::LogRemotlyModified, lline);
	else if (lline.startsWith("P "))
		emit alert(RCS::LogRemotlyModified, lline);
	else if (lline.startsWith("C "))
		emit alert(RCS::LogConflict, lline);
	else if (lline.startsWith("? "))
		emit alert(RCS::LogNotManaged, lline);
	else
		emit alert(RCS::LogNormal, lline);
}

void RCS_CVS::processReadOutput()
{
	if (! m_content)
	{
		m_process->setReadChannel(QProcess::StandardOutput);
		while (m_process->canReadLine())
			processLine(m_process->readLine());

		/* Process error */
		m_process->setReadChannel(QProcess::StandardError);
		while (m_process->canReadLine())
			emit alert(RCS::LogError, m_process->readLine().simplified());
	}
	else
	{
		m_process->setReadChannel(QProcess::StandardOutput);
		while (m_process->canReadLine())
		{
			m_content->append(m_process->readLine());
		}

		/* Process error */
		m_process->setReadChannel(QProcess::StandardError);
		while (m_process->canReadLine())
			emit alert(RCS::LogError, m_process->readLine().simplified());
	}
}

void RCS_CVS::callCVS(QStringList paths, const QStringList & options)
{
	if (paths.size() <= 0) return;
	if (paths.size() > 1)
		paths.sort();

	int i = 0;
	QString path;
	QStringList files;

	do
	{
		files.clear();
		QFileInfo info = QFileInfo(paths.at(i));
		if (info.isDir())
		{
			path = paths.at(i);
		}
		else
		{
			path = info.absolutePath();
			files << info.fileName();
		}
		i++;
		QFileInfo infoNext;
		while ((i < paths.size()) && ((infoNext = QFileInfo(paths.at(i))).absolutePath() == path))
		{
			files << infoNext.fileName();
			i++;
		}

		/* Create process */
		QStringList processOptions = options;
		processOptions << files;

		m_process = new QProcess;

		emit alert(RCS::LogApplication, QString("Working dir : %1").arg(path));
		m_process->setWorkingDirectory(path);

		emit alert(RCS::LogApplication, QString("%1 %2").arg(m_cvs).arg(processOptions.join(" ")).simplified());
		m_process->start(m_cvs, processOptions, QIODevice::ReadWrite | QIODevice::Text);

		while (m_process->state() != QProcess::NotRunning)
		{
			if (m_process->waitForReadyRead(100))
				processReadOutput();
		}
		processReadOutput();

		delete m_process;
	}
	while (i < paths.size());
}
