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

#include "rcs.h"

class PrivateRCS_CVS;
class CVSThread;
class CVSUpdateThread;
class CVSAddThread;
class CVSRemoveThread;
class CVSCommitThread;

class ProcessExecutedException {	
};

class RCS_CVS : public RCS {
	Q_OBJECT
public:
	RCS_CVS();
	virtual ~RCS_CVS();

	rcsState status( const QString & path );
	virtual RCS::FilesOperation operations( const QStringList & path );
	virtual void update( const QStringList & path );
	virtual void commit( const FilesOperation & path, const QString & message );
	virtual void add( const QStringList & path );
	virtual void remove( const QStringList & path );
public slots:
	virtual void abort();
private:
	PrivateRCS_CVS * d;
	friend class PrivateRCS_CVS;
	friend class CVSThread;
	friend class CVSUpdateThread;
	friend class CVSAddThread;
	friend class CVSRemoveThread;
	friend class CVSCommitThread;
};

#endif // __RCS_CVS_H__
