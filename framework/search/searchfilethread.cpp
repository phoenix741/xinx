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
#include "searchfilethread.h"
#include <plugins/xinxpluginsloader.h>
#include <jobs/xinxjobmanager.h>

// Qt header
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QApplication>

/* SearchFileThread */

SearchFileThread::SearchFileThread()
{
}

SearchFileThread::~SearchFileThread()
{
}

QString SearchFileThread::description() const
{
	return tr("Search %1 in %2").arg(m_from).arg (m_path);
}

void SearchFileThread::abort()
{
	_abort = true;
}

void SearchFileThread::search()
{
	if (! m_to.isNull())
		QMessageBox::information(qApp->activeWindow(), tr("Not supported"), tr("Replacement on multiple file is not yet supported"));

	XinxJobManager::self ()->addJob(this);
}

void SearchFileThread::testFile(const QString & path)
{
	emit test(path);

	QFile file(path);
	if (! file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
	int line = 0;
	QString text;
	while (!file.atEnd())
	{
		text = file.readLine();
		line ++;

		Qt::CaseSensitivity cs = Qt::CaseInsensitive;
		if (m_options.testFlag(AbstractEditor::MATCH_CASE))
			cs = Qt::CaseSensitive;

		QString from = m_from;
		if (m_options.testFlag(AbstractEditor::WHOLE_WORDS) && ! m_options.testFlag(AbstractEditor::REGULAR_EXPRESSION))
			from = "\\b" + m_from + "\\b";

		bool contains;
		if (m_options.testFlag(AbstractEditor::WHOLE_WORDS) || m_options.testFlag(AbstractEditor::REGULAR_EXPRESSION))
			contains = text.contains(QRegExp(from));
		else
			contains = text.contains(from, cs);

		if (contains)
			emit find(path, text.trimmed(), line);

		if (_abort) return;
	}
}

void SearchFileThread::searchRecursive(const QString & path)
{
	if (_abort) return;

	QDir pathObject(path);
	QStringList directory = pathObject.entryList(QDir::Dirs);

	foreach(const QString & pathName, directory)
	{
		if ((pathName != ".") && (pathName != ".."))
			searchRecursive(pathObject.absoluteFilePath(pathName));
	}

	QStringList files = pathObject.entryList(XinxPluginsLoader::self()->managedFilters(), QDir::Files);

	foreach(const QString & fileName, files)
	{
		if (_abort) return;

		testFile(pathObject.absoluteFilePath(fileName));
	}
}

void SearchFileThread::startJob()
{
	_abort = false;
	searchRecursive(m_path);
	emit end(_abort);
}

void SearchFileThread::setPath(const QString & path)
{
	m_path = path;
}

void SearchFileThread::setSearchString(const QString & from, const QString & to, const AbstractEditor::SearchOptions & options)
{
	m_from    = from;
	m_to      = to;
	m_options = options;
}

