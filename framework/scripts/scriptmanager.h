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

// Qt header
#include <QObject>
#include <QStringList>
#include <QScriptEngine>
#include <QPointer>

/*!
 * \defgroup XinxScript
 * \brief Define class used to make script with XINX
 *
 * FIXME: Fix the documentation
 *
 * Note : to access to the current project, we must use the project editor.
 *
 * FIXME: Provide a way to access to the manager (defaultProject, and project or the project of a file)
 */

class AbstractEditor;

namespace XinxProject
{
class Parameters;
};

class LIBEXPORT ScriptValue
{
public:
	ScriptValue();
	ScriptValue(QScriptValue value);

	const QScriptValue & value() const;

	QString text() const;

	bool isCallBeforeSave() const;
	void callScriptBeforeSave();

	bool isCallBeforeLoad() const;
	void callScriptBeforeLoad();

	bool isCallAfterSave() const;
	void callScriptAfterSave();

	bool isCallAfterLoad() const;
	void callScriptAfterLoad();

	void callScript();
private:
	QScriptValue m_value;
};


class LIBEXPORT ScriptManager : public QObject
{
	Q_OBJECT
public:
	~ScriptManager();

	const QList<ScriptValue> & objects() const;
	QScriptEngine & engine();

	void setCurrentEditeur(AbstractEditor * editor);

	static ScriptManager * self();

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

	static ScriptManager * s_self;
};

Q_DECLARE_METATYPE(QScriptValue)
Q_DECLARE_METATYPE(ScriptValue)

#endif /*__SCRIPTMANAGER_H__*/
