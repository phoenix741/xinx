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
#include "rcs_svn.h"
#include <core/xinxconfig.h>
#include <plugins/xinxpluginsloader.h>

// Qt header
#include <QTextStream>
#include <QRegExp>
#include <QDir>
#include <QApplication>

// Std header
#ifdef Q_WS_WIN
#include <windows.h>
#endif

/* RCS_SVN */

RCS_SVN::RCS_SVN(const QString & basePath) : RCS(basePath), m_process(0), m_content(0)
{
	qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
	m_tool = XINXConfig::self()->getTools("svn");
}

RCS_SVN::~RCS_SVN()
{

}

RCS::rcsFeatures RCS_SVN::features() const
{
	return RCS::RcsFeatureAdd | RCS::RcsFeatureRemove | RCS::RcsFeatureUpdateAndCommit;
}

RCS::struct_rcs_infos RCS_SVN::info(const QString & path)
{
	if (m_infos.contains(QDir::fromNativeSeparators(path)))
		return m_infos.value(QDir::fromNativeSeparators(path));

	infos(QFileInfo(path).absolutePath());

	if (m_infos.contains(QDir::fromNativeSeparators(path)))
		return m_infos.value(QDir::fromNativeSeparators(path));
	else
	{
		RCS::struct_rcs_infos rcsInfos = { QDir::fromNativeSeparators(path), RCS::Unknown, "0", QDateTime() };
		rcsInfos.filename = QString();
		rcsInfos.state = RCS::Unknown;
		rcsInfos.version = "0";
		rcsInfos.rcsDate = QDateTime();
		return rcsInfos;
	}
}

QList<RCS::struct_rcs_infos> RCS_SVN::infos(const QString & path)
{
	QList<RCS::struct_rcs_infos> result;

	QFileInfo pathInformation(path);

	QString absolutePath = pathInformation.isDir() ? path : QFileInfo(path).absolutePath();
	QProcess process;
	int index = 0;
	QStringList processResult;
	QString options = "-vuN";
	int boucle = 0;
	do
	{
		process.setWorkingDirectory(absolutePath);
		process.setProcessChannelMode(QProcess::MergedChannels);
		process.start(m_tool, QStringList() << "status" << "--non-interactive" << options);
		process.waitForStarted();
		if (process.error() == QProcess::FailedToStart)
		{
			return result;
		}
		process.waitForFinished();
		processResult = QString(process.readAllStandardOutput()).split("\n");
		if ((processResult.count() > 0) && (processResult.at(0).startsWith("svn:")) && (processResult.at(0).contains("Could not resolve hostname")))
		{
			options = "-vN";
		}
		else break;
		boucle++;
	}
	while (boucle <= 1);

	if (processResult.count() == 0) return result;

	do
	{
		RCS::struct_rcs_infos rcsInfos = { QDir::fromNativeSeparators(path), RCS::Unknown, "0", QDateTime() };

		rcsInfos.state = RCS::Unknown;
		rcsInfos.version = "0";
		rcsInfos.rcsDate = QDateTime();

		QString statutFile = processResult.at(index);
		if (statutFile.length() < 8) return result;

		rcsInfos.state = RCS::Updated;
		switch (statutFile.at(0).toAscii())
		{
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
		if ((rcsInfos.state == RCS::Updated) && (statutFile.at(8) == '*'))
			rcsInfos.state = RCS::NeedsCheckout;
		QStringList eol = statutFile.mid(9).simplified().split(" ");
		if (eol.size() >= 1) rcsInfos.version = eol.at(0);


		if (eol.count() >= 4)
		{
			QString filename = QDir::fromNativeSeparators(QDir(absolutePath).absoluteFilePath(eol.at(3)));

			rcsInfos.filename = filename;
			m_infos.insert(filename, rcsInfos);
			result << rcsInfos;
		}

		index++;
	}
	while (index < (processResult.count() - 1));  // Last line is "At revision XXX."

	return result;
}

RCS::FilesOperation RCS_SVN::operations(const QString & path)
{
	QList<FileOperation> result;
	QProcess process;
	process.setWorkingDirectory(QFileInfo(path).absolutePath());
	process.start(m_tool, QStringList() << "status" << path);
	process.waitForStarted();
	if (process.error() == QProcess::FailedToStart) return result;
	process.waitForFinished();
	QStringList processResult = QString(process.readAllStandardOutput()).split("\n");
	if (processResult.count() == 0) return result;

	foreach(const QString & pr, processResult)
	{
		if (pr.isEmpty()) continue;
		QString filename = QDir(path).absoluteFilePath(pr.mid(7).trimmed());
		bool hasWilcard = false;
		foreach(const QString & wilcard, XinxPluginsLoader::self()->managedFilters())
		{
			QRegExp projectWilcard(wilcard, Qt::CaseInsensitive, QRegExp::Wildcard);
			if (projectWilcard.exactMatch(filename))
			{
				hasWilcard = true;
				break;
			}
		}
		if (!hasWilcard) continue;
		switch (pr.at(0).toAscii())
		{
		case 'A' :
		case 'D' :
		case 'M' :
		case 'R' :
			result.append(RCS::FileOperation(filename, RCS::Commit));
			break;
		case 'X' :
		case '?' :
			result.append(RCS::FileOperation(filename, RCS::AddAndCommit));
			break;
		case '!' :
			result.append(RCS::FileOperation(filename, RCS::RemoveAndCommit));
			break;
		default:
			;
		}
	}
	return result;
}

RCS::FilesOperation RCS_SVN::operations(const QStringList & paths)
{
	QList<FileOperation> result;
	foreach(const QString & p, paths)
	{
		result += operations(p);
	}
	return result;
}

void RCS_SVN::logMessages()
{
	QStringList errors = QString::fromLocal8Bit(m_process->readAllStandardError()).split("\n");
	foreach(const QString & error, errors)
	{
		if (! error.trimmed().isEmpty())
			emit alert(RCS::LogError, error.trimmed());
	}
	QStringList infos = QString::fromLocal8Bit(m_process->readAllStandardOutput()).split("\n");
	foreach(const QString & info, infos)
	{
		if (! info.trimmed().isEmpty())
			emit alert(RCS::LogNormal, info.trimmed());
	}
}

void RCS_SVN::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitCode);
	Q_UNUSED(exitStatus);

	emit alert(RCS::LogApplication, tr("Process terminated"));

	foreach(const QString & file,  m_fileChanged)
	{
		m_infos.remove(QDir::fromNativeSeparators(file));
		emit stateChanged(file, info(file));
	}

	delete m_process;
	m_process = 0;
}

void RCS_SVN::update(const QStringList & path)
{
	Q_ASSERT(!m_process);

	QString tool = m_tool;
	QStringList args = QStringList() << "update" << "--non-interactive" << path;
	m_fileChanged = path;
	m_process = new QProcess;
	connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(logMessages()));
	connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(logMessages()));
	connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
	emit alert(RCS::LogApplication, tool + " " + args.join(" "));
	m_process->start(tool, args);
	m_process->waitForStarted();
	if (m_process->error() == QProcess::FailedToStart)
		emit alert(RCS::LogError, tr("Can't start svn program."));
	else
		m_process->waitForFinished();
}

void RCS_SVN::commit(const QStringList & path, const QString & message)
{
	Q_ASSERT(!m_process);

	QStringList args = QStringList() << "commit";
	if (! message.isEmpty())
		args << "-m" << message;
	args << path;
	m_fileChanged = path;
	m_process = new QProcess;
	connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(logMessages()));
	connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(logMessages()));
	connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
	emit alert(RCS::LogApplication, m_tool + " " + args.join(" "));
	m_process->start(m_tool, args);
	m_process->waitForStarted();
	if (m_process->error() == QProcess::FailedToStart)
		emit alert(RCS::LogError, tr("Can't start svn program."));
	else
		m_process->waitForFinished();
}

void RCS_SVN::add(const QStringList & path)
{
	Q_ASSERT(!m_process);

	QStringList args = QStringList() << "add" << path;
	m_fileChanged = path;
	m_process = new QProcess;
	connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(logMessages()));
	connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(logMessages()));
	connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
	emit alert(RCS::LogApplication, m_tool + " " + args.join(" "));
	m_process->start(m_tool, args);
	m_process->waitForStarted();
	if (m_process->error() == QProcess::FailedToStart)
		emit alert(RCS::LogError, tr("Can't start svn program."));
	else
		m_process->waitForFinished();
}

void RCS_SVN::remove(const QStringList & path)
{
	Q_ASSERT(!m_process);

	QStringList args = QStringList() << "remove" << "--non-interactive" << path;
	m_fileChanged = path;
	m_process = new QProcess;
	connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(logMessages()));
	connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(logMessages()));
	connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
	emit alert(RCS::LogApplication, m_tool + " " + args.join(" "));
	m_process->start(m_tool, args);
	m_process->waitForStarted();
	if (m_process->error() == QProcess::FailedToStart)
		emit alert(RCS::LogError, tr("Can't start svn program."));
	else
		m_process->waitForFinished();
}

void RCS_SVN::updateToRevisionFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitCode);
	Q_UNUSED(exitStatus);

	QFile temporaryFile(m_tmpfilename);
	if (temporaryFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		*m_content = temporaryFile.readAll();
	}
	temporaryFile.remove();

	emit alert(RCS::LogApplication, tr("Process terminated"));

	delete m_process;
	m_process = 0;
}

void RCS_SVN::updateToRevision(const QString & path, const QString & revision, QByteArray * content)
{
	// svn export -r rev path temporyfile
	m_content     = content;
	m_tmpfilename =  QDir(QDir::tempPath()).absoluteFilePath(QFileInfo(path).fileName() + "." + revision);

	QStringList args = QStringList() << "export" << "--non-interactive" << "-r" << revision << path << m_tmpfilename;
	m_process = new QProcess;
	connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(logMessages()));
	connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(logMessages()));
	connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(updateToRevisionFinished(int,QProcess::ExitStatus)));
	emit alert(RCS::LogApplication, m_tool + " " + args.join(" "));

	m_process->setWorkingDirectory(QFileInfo(path).absolutePath());
	m_process->start(m_tool, args);
	m_process->waitForStarted();
	if (m_process->error() == QProcess::FailedToStart)
		emit alert(RCS::LogError, tr("Can't start svn program."));
	else
		m_process->waitForFinished();
}

void RCS_SVN::abort()
{
	if ((! m_process) || (m_process->state() == QProcess::NotRunning)) return ;
	m_process->terminate();
#ifdef Q_WS_WIN
	if (GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, m_process->pid()->dwThreadId) != 0)
		perror("GenerateConsoleCtrlEvent");
	if (GenerateConsoleCtrlEvent(CTRL_C_EVENT, m_process->pid()->dwThreadId) != 0)
		perror("GenerateConsoleCtrlEvent");
	emit alert(RCS::LogError, tr("Can't stop the process on windows."));
#endif
}
