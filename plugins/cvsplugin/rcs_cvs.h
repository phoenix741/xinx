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

#ifndef __RCS_CVS_H__
#define __RCS_CVS_H__

// Xinx header
#include <core/exceptions.h>
#include <rcs/rcs.h>
#include "entries.h"

// Qt header
#include <QPointer>
#include <QFileSystemWatcher>

class PluginSettings;
class CVSThread;

class RCS_CVS : public RCS {
	Q_OBJECT
public:
	RCS_CVS( const QString & base );
	virtual ~RCS_CVS();

	virtual struct_rcs_infos infos( const QString & path );
	virtual RCS::FilesOperation operations( const QStringList & path );
	virtual void update( const QStringList & path );
	virtual void commit( const FilesOperation & path, const QString & message );
	virtual void add( const QStringList & path );
	virtual void remove( const QStringList & path );
	virtual void updateToRevision( const QString & path, const QString & revision, QString * content = 0 );
public slots:
	virtual void abort();
private slots:
	void entriesStateChanged( const QString & path );
private:
	void updateEntries();

	RCS::rcsOperation operationOfState( RCS::rcsState state );
	RCS::FilesOperation operationOf( const QString & path );
	RCS::FilesOperation recursiveOperationOf( const QString & path );

	QPointer<CVSThread> m_thread;
	EntriesList * m_entriesList;
	QFileSystemWatcher * m_watcher;
};

#endif // __RCS_CVS_H__
