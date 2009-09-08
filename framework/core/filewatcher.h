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

#ifndef _FILEWATCHER_H_
#define _FILEWATCHER_H_
#pragma once

// Qt header
#include <qobject.h>

class PrivateWatcher;

/*!
 * Class used to watch the modification of a file. The file is periodically watched
 * and if the date of the file is modified, the user is alerted by a signal.
 *
 * Behind the FileWatcher, a thread watch modifications of all files referenced by a
 * FileWatcher. The watcher can be desactivate and reactivate without delete the object.
 * When the object is reactivated, the file date, in memory, is updated.
 */
class FileWatcher : public QObject {
	Q_OBJECT
public:
	/*!
	 * Create a FileWatcher with a file name
	 * \param filename The file name to watch.
	 */
	FileWatcher( const QString & filename );
	/*! Destroy the FileWatcher */
	virtual ~FileWatcher();

	/*!
	 * Desactivate the watcher. You must called the \e activate method to reactivate
	 * the watcher.
	 */
	void desactivate();
	/*!
	 * Activate the watcher. The watcher is created activated. Use \e desactivate to
	 * desactivate.
	 */
	void activate();
signals:
	/*! The signal is emited when the watched file is modified.*/
	void fileChanged();
protected:
	PrivateWatcher * d;
	friend class PrivateWatcher;
};

#endif // _FILEWATCHER_H_
