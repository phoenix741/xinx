/*
 XINX
 Copyright (C) 2007-2010 by Ulrich Van Den Hekke
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

#ifndef DICTIONARYMODEL_H
#define DICTIONARYMODEL_H

#include <QStandardItemModel>

namespace XinxProject
{
	class Project;
	typedef QSharedPointer<Project> ProjectPtr;
	typedef QWeakPointer<Project> ProjectPtrWeak;
}

class DictionaryModel : public QStandardItemModel
{
	Q_OBJECT
public:
    DictionaryModel(QObject* parent = 0);
    virtual ~DictionaryModel();

	void loadDictionaries(const QString & prefix);
signals:
	void changed();
private slots:
	void textEditorChanged(int index);
private:
	XinxProject::ProjectPtrWeak  _project;
	QString _prefix;
};

#endif // DICTIONARYMODEL_H
