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
#pragma once

// Xinx header
#include <core/exceptions.h>
#include <rcs/rcs.h>
#include "entries.h"

// Qt header
#include <QPointer>
#include <QFileSystemWatcher>
#include <QProcess>

class PluginSettings;

class RCS_CVS : public RCS {
	Q_OBJECT
public:
	RCS_CVS( const QString & base );
	virtual ~RCS_CVS();

	virtual void update( const QStringList & path );
	virtual void updateToRevision( const QString & path, const QString & revision, QByteArray * content = 0 );
	virtual void commit( const QStringList & path, const QString & message );

	virtual void add( const QStringList & path );
	virtual void remove( const QStringList & path );
	virtual void searchFileToAddOrRemove( const QStringList & path, QStringList & toAdd, QStringList & toRemove );
	virtual struct_rcs_infos infos( const QString & path );
public slots:
	virtual void abort();

private slots:
	void entriesStateChanged( const QString & path );
private:
	void processLine( const QString & line );
	void processReadOutput();

	void callCVS( QStringList path, const QStringList & options );

	void updateEntries();

	RCS::rcsOperation operationOfState( RCS::rcsState state );
	RCS::FilesOperation operationOf( const QString & path );
	RCS::FilesOperation recursiveOperationOf( const QString & path );


	QPointer<QByteArray> m_content;
	QPointer<QProcess> m_process;

	EntriesList * m_entriesList;
	QFileSystemWatcher * m_watcher;

	QString m_cvs;
	PluginSettings * m_settings;
};

#endif // __RCS_CVS_H__
