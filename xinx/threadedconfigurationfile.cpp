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

// Qt header
#include <QMetaType>

/* ThreadedConfigurationFile */

ThreadedConfigurationFile::ThreadedConfigurationFile() {
	connect( this, SIGNAL(finished()), this, SLOT(threadFinished()) );
}

ThreadedConfigurationFile::~ThreadedConfigurationFile() {
	disconnect();
	terminate();
	wait();
}

ThreadedConfigurationFile * ThreadedConfigurationFile::simpleConfigurationFile( const QString & pathname ) {
	qRegisterMetaType<ConfigurationFile>("ConfigurationFile");

	ThreadedConfigurationFile * instance = new ThreadedConfigurationFile();
	instance->m_state = ThreadedConfigurationFile::GETVERSION;
	instance->m_pathname = pathname;

	return instance;
}

void ThreadedConfigurationFile::threadrun() {
	if( m_state == ThreadedConfigurationFile::GETVERSION ) {
		m_configuration = MetaConfigurationFile::simpleConfigurationFile( m_pathname );
	}
}

void ThreadedConfigurationFile::threadFinished() {
	if( m_state == GETVERSION ) {
		emit versionFinded( m_configuration );
	}
}

