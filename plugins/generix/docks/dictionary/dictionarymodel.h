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
#include <QPointer>

namespace XinxProject
{
class Project;
typedef QSharedPointer<Project> ProjectPtr;
typedef QWeakPointer<Project> ProjectPtrWeak;
}
class ConfigurationManager;

class DictionaryModel : public QStandardItemModel
{
	Q_OBJECT
public:
	enum
	{
		ITEM_CODE  = Qt::UserRole,
		ITEM_LANG  = Qt::UserRole + 1,
		ITEM_CTX   = Qt::UserRole + 2,
		ITEM_DICO  = Qt::UserRole + 3
	};
	DictionaryModel(QObject* parent = 0);
	virtual ~DictionaryModel();

	void loadDictionaries(const QString & prefix);
	const QStringList & dictionaries() const;

	XinxProject::ProjectPtr getProject() const;

	bool changeLabel(const QString & dictionary, const QString & label, const QString & lang, const QString & context, const QString & content);
signals:
	void changed();
private slots:
	void textEditorChanged(int index);
	void reload();
private:
	void setConfigurationManager(ConfigurationManager* m);

	XinxProject::ProjectPtrWeak  _project;
	QPointer<ConfigurationManager> _currentConfigurationManager;
	QString _prefix;
	QStringList _dictionaries;
};

#endif // DICTIONARYMODEL_H
