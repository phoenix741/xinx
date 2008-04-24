/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke   *
 *   ulrich.vdh@gmail.com   *
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
#include "xinxcore.h"
#include "scriptmanager.h"

// Qt header
#include <QApplication>
#include <QMutex>
#include <QMutexLocker>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>

/* Static member */

ScriptManager * ScriptManager::s_self = 0;

/* ScriptManager */

ScriptManager::~ScriptManager() {
	
}

void ScriptManager::loadScripts() {
	QDir scriptDir = QApplication::applicationDirPath();
	scriptDir.cdUp();
	scriptDir.cd( "scripts" );
	
	QStringList filenames = scriptDir.entryList( QStringList("*.js"), QDir::Files );
	
	foreach( QString filename, filenames ) {
		loadScript( scriptDir.absoluteFilePath( filename ) );
	}
}

void ScriptManager::loadScript( const QString & filename ) {
	QFile qsFile( filename );
	if( ! qsFile.open( QIODevice::ReadOnly ) ) {
		qWarning( qPrintable( tr("Cannot read file : %1: %2").arg( QFileInfo( filename ).fileName() ).arg( qsFile.errorString() ) ) );
		return;
	}
	
	QTextStream in( &qsFile );
	in.setCodec( "UTF-8" );
	
	QString script = in.readAll();
	qsFile.close();
	
	QScriptValue qsScript = m_engine.evaluate( script );
	if( m_engine.hasUncaughtException() ) {
		qWarning( qPrintable( tr( "An error occurred while executing the script %1 : %2" ).arg( QFileInfo( filename ).fileName() ).arg( m_engine.uncaughtException().toString() ) ) );
		return;
	}
	
	m_objects.append( qsScript );
	
	m_filenames << filename;
}

const QList<QScriptValue> & ScriptManager::objects() const {
	return m_objects;
}

QScriptEngine & ScriptManager::engine() {
	return m_engine;
}

ScriptManager * ScriptManager::self() {
	static QMutex selfMutex;
	
	QMutexLocker locker( &selfMutex );
	if( s_self == 0 ) {
		s_self = new ScriptManager();
		XINXStaticDeleter::self()->addObject( s_self );
	}
	return s_self;
}

ScriptManager::ScriptManager() {
	loadScripts();
}

