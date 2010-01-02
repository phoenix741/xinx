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

// Xinx header
#include "threadedconfigurationfile.h"
#include <project/xinxproject.h>

// Qt header
#include <QMetaType>
#include <QDir>

/* ThreadedConfigurationFile */

ThreadedConfigurationFile::ThreadedConfigurationFile() {
	qRegisterMetaType<ConfigurationFile>("ConfigurationFile");

	connect( this, SIGNAL(finished()), this, SLOT(threadFinished()) );
}

ThreadedConfigurationFile::~ThreadedConfigurationFile() {
	disconnect();
	terminate();
	wait();
}

ThreadedConfigurationFile * ThreadedConfigurationFile::simpleConfigurationFile( const QString & pathname ) {
	ThreadedConfigurationFile * instance = new ThreadedConfigurationFile();
	instance->m_state = ThreadedConfigurationFile::GETVERSION;
	instance->m_pathname = pathname;

	return instance;
}

ThreadedConfigurationFile * ThreadedConfigurationFile::businessViewOfFile( const QString & filename ) {
	ThreadedConfigurationFile * instance = new ThreadedConfigurationFile();
	instance->m_state = ThreadedConfigurationFile::GETBUSINESSVIEW;
	instance->m_filename = filename;
	instance->m_businessView.clear();

	return instance;
}

void ThreadedConfigurationFile::threadrun() {
	if( m_state == ThreadedConfigurationFile::GETVERSION ) {
		m_configuration = MetaConfigurationFile::simpleConfigurationFile( m_pathname );
		m_configuration.version();
		m_configuration.xmlPresentationFile();
	} else if( m_state == ThreadedConfigurationFile::GETBUSINESSVIEW ) {
		if( XINXProjectManager::self()->project() ) {
			QString path = XINXProjectManager::self()->project()->projectPath();

			MetaConfigurationFile metaconf( QDir( path ).absoluteFilePath( "configurationdef.xml" ) );
			foreach( ConfigurationFile * conf, metaconf.configurations() ) {
				QStringList bvs = conf->businessViewPerFiles( QDir( path ).relativeFilePath( m_filename ) );
				foreach( const QString & bv, bvs ) {
					m_businessView << (bv + " (" + QFileInfo( conf->filename() ).fileName() + ")");
				}
			}
		}
	}
}

void ThreadedConfigurationFile::threadFinished() {
	if( m_state == GETVERSION ) {
		emit versionFinded( m_configuration );
	} else if( m_state == GETBUSINESSVIEW ) {
		emit businessViewFinded( m_businessView );
	}
}

