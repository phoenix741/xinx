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
#ifndef __DIRECTORYEDIT_P_H__
#define __DIRECTORYEDIT_P_H__

#include "directoryedit.h"
#include <QString>
#include <QObject>

//! \private
class DirectoryEditPrivate : public QObject
{
	Q_OBJECT
public:
	DirectoryEditPrivate(DirectoryEdit* parent);

	DirectoryEdit * _parent;
	bool m_error, m_directory, m_fileMustExist, m_ignoreCursorPositionChanged;
	QString m_filter, m_prefix;
public slots:
	void slotTextChanged(QString text);
	void slotEditorCursorPositionChanged(int, int);
};

#endif // __DIRECTORYEDIT_P_H__

