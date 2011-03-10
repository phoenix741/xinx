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

#pragma once
#ifndef __SCRIPTMANAGER_H__
#define __SCRIPTMANAGER_H__

// Xinx header
#include <core/lib-config.h>
#include <core/xinxsingleton.h>
#include <scripts/scriptvalue.h>

// Qt header
#include <QObject>
#include <QStringList>
#include <QScriptEngine>
#include <QPointer>

class AbstractEditor;

namespace XinxProject
{
class Parameters;
};

class LIBEXPORT ScriptManager : public XinxLibSingleton<ScriptManager>
{
	Q_OBJECT
public:
	~ScriptManager();

	const QList<ScriptValue> & objects() const;
	QScriptEngine & engine();

	void setCurrentEditeur(AbstractEditor * editor);

	void callScriptsBeforeSave(AbstractEditor * editor);
	void callScriptsAfterSave(AbstractEditor * editor);
	void callScriptsBeforeLoad(AbstractEditor * editor);
	void callScriptsAfterLoad(AbstractEditor * editor);
public slots:
	void loadScripts();
signals:
	void changed();
private:
	ScriptManager();
	void loadScript(const QString & filename);

	QStringList m_filenames;
	QScriptEngine m_engine;
	QList<ScriptValue> m_objects;
	QPointer<XinxProject::Parameters> m_project;
	AbstractEditor * m_editor;
	friend class XinxLibSingleton<ScriptManager>;
};

#endif /*__SCRIPTMANAGER_H__*/
