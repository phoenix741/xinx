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

#include "replacefilethread.h"
#include <jobs/xinxjobmanager.h>
#include "findedline.h"
#include <QFile>
#include "findedfile.h"
#include <core/xinxconfig.h>

ReplaceFileThread::ReplaceFileThread()
{

}

ReplaceFileThread::~ReplaceFileThread()
{

}

void ReplaceFileThread::replace()
{
	XinxJobManager::self ()->addJob(this);
}

QString ReplaceFileThread::description() const
{
	return tr("Replace %1 by %2").arg(m_from).arg (m_to);
}

void ReplaceFileThread::addReplacement(const QString& filename, int line, const QString& content, int posStart, int posEnd)
{
	FindedFile file = _files.value(filename);
	if (! file.isValid())
	{
		file = FindedFile(filename);
		_files.insert(filename, file);
	}

	file.addFindedLine(FindedLine(line, content, posStart, posEnd));
}

void ReplaceFileThread::setSearchString(const QString& from, const QString& to, const AbstractEditor::SearchOptions& options)
{
	m_from    = from;
	m_to      = to;
	m_options = options;
}

void ReplaceFileThread::updateContent(const FindedFile & findedFile)
{
	QList<QByteArray> fileContent;

	/* Reading file */
	{
		QFile originalFile(findedFile.filename());
		if (! originalFile.open(QIODevice::ReadOnly | QIODevice::Text)) return;

		while (! originalFile.atEnd())
		{
			fileContent.append(originalFile.readLine());
		}
	}

	/* Replace string */
	QSet<int> processedLine; /* a line can contains multiple search string, QString.replace replace all occurences */
	foreach(const FindedLine & findedLine, findedFile)
	{
		// We don't process line already processed (this can happen, if we have the same search string on the same line)
		if (processedLine.contains(findedLine.line())) continue;
		processedLine.insert(findedLine.line());

		const int index = findedLine.line() -1;

		const QByteArray & byteLine = fileContent.at(index);
		QString strLine = QString::fromLocal8Bit(byteLine);

		if (strLine != findedLine.content() + "\n")
		{
			emit lineModified(findedFile.filename(), findedLine.line());
			continue;
		}

		Qt::CaseSensitivity cs = Qt::CaseInsensitive;
		if (m_options.testFlag(AbstractEditor::MATCH_CASE))
			cs = Qt::CaseSensitive;

		QString from = m_from;
		if (m_options.testFlag(AbstractEditor::WHOLE_WORDS) && ! m_options.testFlag(AbstractEditor::REGULAR_EXPRESSION))
			from = "\\b" + m_from + "\\b";

		if (m_options.testFlag(AbstractEditor::WHOLE_WORDS) || m_options.testFlag(AbstractEditor::REGULAR_EXPRESSION))
		{
			strLine = strLine.replace(QRegExp(from), m_to);
		}
		else
		{
			strLine = strLine.replace(from, m_to);
		}

		fileContent.replace(index, strLine.toLocal8Bit());
	}

	/* Create backup file */
	if (XINXConfig::self()->config().editor.createBackupFile)
	{
		QFile::copy(findedFile.filename(), findedFile.filename() + ".bak");
	}

	/* Writing the new file */
	{
		QFile destinationFile(findedFile.filename());
		if (! destinationFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return;

		for(int i = 0; i < fileContent.size(); i++)
		{
			const QByteArray & line = fileContent.at(i);
			destinationFile.write(line);
		}
	}
}

void ReplaceFileThread::startJob()
{
	foreach(const FindedFile& file, _files)
	{
		updateContent(file);
	}
	emit processEnded();
}

