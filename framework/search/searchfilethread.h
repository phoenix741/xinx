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
#include <core/xinxthread.h>
#include <editors/abstracteditor.h>

class SearchFileThread : public XinxThread
{
	Q_OBJECT
public:
	SearchFileThread(QObject * parent = 0);
	virtual ~SearchFileThread();

	void setPath(const QString & path);
	void setSearchString(const QString & from, const QString & to, const AbstractEditor::SearchOptions & options);

	void search();
signals:
	void find(const QString & filename, const QString & lineText, int lineNumber);
	void test(const QString & filename);
	void end();
protected:
	virtual void threadrun();
private:
	void searchRecursive(const QString & path);
	void testFile(const QString & path);

	QString m_from, m_to, m_path;
	AbstractEditor::SearchOptions m_options;
};

#endif // _SEARCHFILETHREAD_H_
