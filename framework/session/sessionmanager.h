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
#ifndef XINXSESSIONSESSIONMANAGER_H
#define XINXSESSIONSESSIONMANAGER_H

// Xinx header
#include <core/lib-config.h>
#include <core/xinxsingleton.h>
#include <utils/actiongroup.h>
#include <session/session.h>

namespace XinxProject
{
class Project;
}

namespace XinxSession
{

class PrivateSessionManager;

class LIBEXPORT SessionManager : public XinxLibSingleton<SessionManager>
{
	Q_OBJECT
public:
	~SessionManager();

	Session * currentSession() const;

	QAction * newSessionAction() const;
	QAction * removeSessionAction() const;
	QAction * renameSessionAction() const;
	ActionGroup * sessionActions() const;

	bool isApplicationStopping() const;

public slots:
	void setApplicationStopping(bool value);

	void restoreSession(const QString & sessionName = DEFAULT_SESSION);
	void renameSession(const QString & sessionName);
	void deleteSession();

	void createRecoverSession();
	void deleteRecoverSession();
signals:
	void changed();
protected:
	virtual void initialisation();
private:
	SessionManager();

	QScopedPointer<PrivateSessionManager> d;
	friend class XinxLibSingleton<SessionManager>;
};

} // namespace XinxSession

#endif // XINXSESSIONSESSIONMANAGER_H
