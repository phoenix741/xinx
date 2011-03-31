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
#ifndef XINXSESSIONSESSIONMANAGER_P_H
#define XINXSESSIONSESSIONMANAGER_P_H

#include "sessionmanager.h"
#include <QSignalMapper>

namespace XinxSession
{

//! \private
class PrivateSessionManager : public QObject
{
	Q_OBJECT
public:
	PrivateSessionManager(SessionManager * session_manager);

	void updateSessions();

	SessionManager * _session_manager;
	Session * _current_session;

	QAction * _new_session_action;
	QAction * _remove_session_action;
	QAction * _rename_session_action;
	ActionGroup * _session_actions;
	QSignalMapper * _signal_mapper;

	bool _is_application_closing;
public slots:
	void newSessionTrigger();
	void renameSessionTrigger();
	void removeSessionTrigger();
};

}

#endif // XINXSESSIONSESSIONMANAGER_P_H
