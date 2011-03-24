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
#ifndef XINXSESSIONSESSION_P_H
#define XINXSESSIONSESSION_P_H

#include "session.h"

namespace XinxSession
{

class PrivateSession
{
public:
	PrivateSession(Session * session);
	void save(const QString & saveName);
	void load();

	Session * _session;
	QString _current_project_path;
	QString _current_session_name;
	QStringList _last_opened_file;
	QStringList _project_path_opened;
	QList<SessionEditor*> _opened_editor;

	bool _is_session_updatable;
};

}

#endif // XINXSESSIONSESSION_P_H
