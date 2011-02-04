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
#ifndef XINXSESSIONSESSION_H
#define XINXSESSIONSESSION_H

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>
#include <core/xinxsingleton.h>
#include <utils/actiongroup.h>

// Qt header
#include <QString>
#include <QStringList>
#include <QPair>
#include <QVariant>
#include <QPointer>
#include <QApplication>

#define DEFAULT_SESSION QString("default")
#define RECOVER_SESSION QString("recover")

namespace XinxProject
{
	class Project;
}

namespace XinxSession
{

class SessionManager;
class SessionEditor;
class PrivateSession;

class LIBEXPORT Session : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QStringList lastOpenedFile READ lastOpenedFile)
public:
	Session(SessionManager * manager, const QString & sessionName = DEFAULT_SESSION);
	~Session();

	const QStringList & lastOpenedFile() const;
	void addOpenedFile(const QString & filename);

	void clearSerializedEditors();
	const QList<SessionEditor*> & serializedEditors() const;
	void addSerializedEditor(SessionEditor * editor);
	void removeSerializedEditor(SessionEditor * editor);

	const QStringList & openedProject() const;
	void addOpenedProject(const QString & projectPath);
	void deleteOpenedProject(const QString & projectPath);

	static QStringList sessionsNames();
	const QString & sessionName() const;
	void loadSession(const QString & sessionName = QString());
	void saveSession(const QString & sessionName = QString());
	void removeSession();

	void setSessionUpdatable(bool value);
	bool isSessionUpdatable() const;
signals:
	void changed();
private:
	QScopedPointer<PrivateSession> d;
	friend class XinxLibSingleton<Session>;
};

} // namespace XinxSession

Q_DECLARE_METATYPE(XinxSession::Session*)

#endif // XINXSESSIONSESSION_H
