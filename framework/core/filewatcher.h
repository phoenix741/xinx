/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

#ifndef _FILEWATCHER_H_
#define _FILEWATCHER_H_
#pragma once

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <qobject.h>

class PrivateWatcher;

class LIBEXPORT FileWatcher : public QObject
{
	Q_OBJECT
public:
	FileWatcher(const QString & filename);
	virtual ~FileWatcher();

	void desactivate();
	void activate();
signals:
	void fileChanged();
protected:
	PrivateWatcher * d;
	friend class PrivateWatcher;
};

#endif // _FILEWATCHER_H_
