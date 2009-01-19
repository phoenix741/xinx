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

#ifndef __SCRIPTMANAGER_H__
#define __SCRIPTMANAGER_H__

// Qt header
#include <QObject>
#include <QStringList>
#include <QScriptEngine>

class AbstractEditor;

class ScriptManager : public QObject {
	Q_OBJECT
public:
	~ScriptManager();
	
	const QList<QScriptValue> & objects() const; 
	QScriptEngine & engine();
	
	void setCurrentEditeur( AbstractEditor * editor );
	
	static ScriptManager * self();
	
public slots:
	void loadScripts();
signals:
	void changed();
private slots:
	void projectChange();
private:
	ScriptManager();
	void loadScript( const QString & filename );
	
	QStringList m_filenames;
	QScriptEngine m_engine;
	QList<QScriptValue> m_objects;
	
	static ScriptManager * s_self;
};

Q_DECLARE_METATYPE( QScriptValue )

#endif /*__SCRIPTMANAGER_H__*/
