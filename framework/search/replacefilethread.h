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

#ifndef REPLACEFILETHREAD_H
#define REPLACEFILETHREAD_H

#include <jobs/xinxjob.h>
#include <editors/abstracteditor.h>
#include <QHash>

class FindedFile;

class ReplaceFileThread : public XinxJob
{
	Q_OBJECT
public:
	ReplaceFileThread();
	virtual ~ReplaceFileThread();

	virtual QString description() const;

	void replace();

	void setSearchString(const QString & from, const QString & to, const AbstractEditor::SearchOptions & options);
	void addReplacement(const QString & filename, int line, const QString & content, int posStart, int posEnd);
signals:
	void lineModified(const QString & filename, int line);
	void processEnded();
protected:
    virtual void startJob();
private:
	void updateContent(const FindedFile & findedFile);

	QHash<QString,FindedFile> _files;
	QString m_from, m_to;
	AbstractEditor::SearchOptions m_options;
};

#endif // REPLACEFILETHREAD_H
