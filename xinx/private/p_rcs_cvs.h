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

#ifndef __P_RCS_CVS_H__
#define __P_RCS_CVS_H__

// Xinx header
#include "../rcs_cvs.h"
#include "../xinxconfig.h"
#include "../cvsfiles.h"
#include "../xinxthread.h"

// Qt header
#include <QStringList>
#include <QThread>

/* PrivateRCS_CVS */

class PrivateRCS_CVS : public QObject {
	Q_OBJECT
public:
	PrivateRCS_CVS( RCS_CVS * parent );
	virtual ~PrivateRCS_CVS();

	void callUpdate( const QStringList & path );
	void callCommit( const RCS::FilesOperation & path, const QString & message );
	void callAdd( const QStringList & path );
	void callRemove( const QStringList & path );
	void callUpdateToRevision( const QString & path, const QString & revision, QString * content );
	
	RCS::rcsOperation operationOfState( RCS::rcsState state );
	RCS::FilesOperation operationOf( const QString & path );
	RCS::FilesOperation recursiveOperationOf( const QString & path );
	
	XinxThread * m_thread;
	CVSFileEntryList * m_entries;
private:
	RCS_CVS * m_parent;
};

#endif // __P_RCS_CVS_H__
