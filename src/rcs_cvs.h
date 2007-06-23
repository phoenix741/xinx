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

class ProcessExecutedException {	
};

class RCS_CVS : public RCS {
	Q_OBJECT
public:
	RCS_CVS();
	~RCS_CVS();

	rcsState status( const QString & path );
	virtual RCS::FilesOperation operations( const QString & path );
	virtual void update( const QString & path );
	virtual void commit( const QString & path, const QString & message );
	virtual void add( const QString & path );
	virtual void remove( const QString & path );
public slots:
	virtual void abort();
private:
	PrivateRCS_CVS * d;
	friend class PrivateRCS_CVS;
};

#endif // __RCS_CVS_H__
