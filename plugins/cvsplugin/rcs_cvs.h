/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __RCS_CVS_H__
#define __RCS_CVS_H__

// Xinx header
#include <rcs.h>
#include <exceptions.h>
#include "entries.h"

//Qt header
#include <QPointer>

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
private:
	void callUpdate( const QStringList & path );
	void callCommit( const RCS::FilesOperation & path, const QString & message );
	void callAdd( const QStringList & path );
	void callRemove( const QStringList & path );
	void callUpdateToRevision( const QString & path, const QString & revision, QString * content );
	
	RCS::rcsOperation operationOfState( RCS::rcsState state );
	RCS::FilesOperation operationOf( const QString & path );
	RCS::FilesOperation recursiveOperationOf( const QString & path );
	
	QPointer<CVSThread> m_thread;
	EntriesList * m_entriesList;
};

#endif // __RCS_CVS_H__
