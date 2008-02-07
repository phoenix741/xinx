/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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
 
// Xinx header
#include "threadedconfigurationfile.h"
#include "private/p_threadedconfigurationfile.h"

// Qt header
#include <QMetaType>

/* PrivateThreadedConfigurationFile */

PrivateThreadedConfigurationFile::PrivateThreadedConfigurationFile( ThreadedConfigurationFile * parent ) : m_parent( parent ) {
	
}

PrivateThreadedConfigurationFile::~PrivateThreadedConfigurationFile() {
	
}

void PrivateThreadedConfigurationFile::threadFinished() {
	if( m_state == GETVERSION ) {
		emit m_parent->versionFinded( m_configuration );
	}
}


/* ThreadedConfigurationFile */

ThreadedConfigurationFile::ThreadedConfigurationFile() {
	d = new PrivateThreadedConfigurationFile( this );
	connect( this, SIGNAL(finished()), d, SLOT(threadFinished()) );
}

ThreadedConfigurationFile::~ThreadedConfigurationFile() {
	disconnect();
	terminate();
	wait();
	delete d;
}

ThreadedConfigurationFile * ThreadedConfigurationFile::simpleConfigurationFile( const QString & pathname ) {
	qRegisterMetaType<SimpleConfigurationFile>("SimpleConfigurationFile");
	
	ThreadedConfigurationFile * instance = new ThreadedConfigurationFile();
	instance->d->m_state = PrivateThreadedConfigurationFile::GETVERSION;
	instance->d->m_pathname = pathname;

	return instance;
}

void ThreadedConfigurationFile::threadrun() {
	if( d->m_state == PrivateThreadedConfigurationFile::GETVERSION ) {
		d->m_configuration = MetaConfigurationFile::simpleConfigurationFile( d->m_pathname );
	}
}
