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

#ifndef _SEARCHFILETHREAD_H_
#define _SEARCHFILETHREAD_H_
#pragma once

// Xinx header
#include <jobs/xinxjob.h>
#include <editors/abstracteditor.h>
#include <QQueue>

class SearchFileThread : public XinxJob
{
	Q_OBJECT
public:
	SearchFileThread();
	virtual ~SearchFileThread();

	virtual QString description() const;

	virtual bool canBeCanceled() const;

	void setPath(const QStringList & pathList);
	void setSearchString(const QString & from, const QString & to, const AbstractEditor::SearchOptions & options);

	void search();
signals:
	void find(const QString & filename, const QString & lineText, int lineNumber);
	void test(const QString & filename);
	void end(bool abort);
public slots:
	void abort();
protected:
	virtual void startJob();
private:
	void searchRecursive(const QString & path);
	void testFile(const QString & path);

	QQueue<QString> _path;
	bool _abort;
	QString m_from, m_to, m_path;
	AbstractEditor::SearchOptions m_options;
};

#endif // _SEARCHFILETHREAD_H_
