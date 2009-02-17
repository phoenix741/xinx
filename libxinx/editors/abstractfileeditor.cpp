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
#include "editors/abstractfileeditor.h"
#include "core/xinxconfig.h"
#include "core/filewatcher.h"
#include "core/xinxproject.h"

// Qt header
#include <QFile>
#include <QApplication>
#include <QMessageBox>
#include <QFileInfo>

/* AbstractFileEditor */

AbstractFileEditor::AbstractFileEditor( QWidget * parent ) : AbstractEditor( parent ), m_isSaving( false ), m_modified( false ) {
}

AbstractFileEditor::~AbstractFileEditor() {
	desactivateWatcher();
	delete m_watcher;
}


QString AbstractFileEditor::getTitle() const {
	if( ! m_lastFileName.isEmpty() )
		return QFileInfo( m_lastFileName ).fileName();
	else
		return defaultFileName();
}

QString AbstractFileEditor::getLongTitle() const {
	if( ! m_lastFileName.isEmpty() )
		return m_lastFileName;
	else
		return defaultFileName();
}

void AbstractFileEditor::loadFromFile( const QString & fileName ) {
	if( ! fileName.isEmpty() ) setWatcher( fileName );

	QFile file( m_lastFileName );
	if( ! file.open( QIODevice::ReadOnly ) ) {
		qCritical( qPrintable(tr("Can't open file for reading %1 : %2").arg( m_lastFileName ).arg( file.errorString() )) );
		return;
	}
	loadFromDevice( file );

	setModified( false );
}

void AbstractFileEditor::saveToFile( const QString & fileName ) {
	/* Make a backup of the file */
	if( ( fileName.isEmpty() || ( fileName == m_lastFileName ) )
	    && XINXConfig::self()->config().editor.createBackupFile ) {

		    if( QFile::exists( fileName + ".bak" ) ) 	QFile::remove( fileName + ".bak" );
		    QFile::copy( fileName, fileName + ".bak" );
	    }

	/* Change the file name */
	if( ! fileName.isEmpty() ) setWatcher( fileName );

	m_isSaving = true;
	desactivateWatcher();
	qApp->processEvents();

	/* Open the file for writting an save */
	QFile file( m_lastFileName );
	if( ! file.open( QIODevice::WriteOnly ) ) {
		qCritical( qPrintable(tr("Can't open file for writing %1 : %2").arg( m_lastFileName ).arg( file.errorString() )) );
		m_isSaving = false;
		activateWatcher();
		return;
	}
	saveToDevice( file );
	file.close();

	m_isSaving = false;
	activateWatcher();
	setModified( false );
}

const QString & AbstractFileEditor::lastFileName() const {
	return m_lastFileName;
}

void AbstractFileEditor::setModified( bool isModified ) {
	if( m_modified != isModified ) {
		m_modified = isModified;
		emit modificationChanged( isModified );
	}
}

bool AbstractFileEditor::isModified() {
	return m_modified;
}

void AbstractFileEditor::fileChanged() {
	if( m_isSaving ) return;
	if( ! XINXConfig::self()->config().editor.popupWhenFileModified ) return ;

	m_watcher->desactivate();
	if( QFile( m_lastFileName ).exists() && QMessageBox::question( qApp->activeWindow(), tr("Reload page"), tr("The file %1 was modified. Reload the page ?").arg( QFileInfo( m_lastFileName ).fileName() ), QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
		loadFromFile();
	else
		setModified( true );

	if( ! QFile( m_lastFileName ).exists() ) {
		QMessageBox::warning( qApp->activeWindow(), tr("Reload page"), tr("The file %1 was removed.").arg( QFileInfo( m_lastFileName ).fileName() ) );
		setModified( true );
	}
	m_watcher->activate();
}

void AbstractFileEditor::desactivateWatcher() {
	if( m_watcher )
		m_watcher->desactivate();
}

void AbstractFileEditor::activateWatcher() {
	if( ! m_watcher ) {
		m_watcher = new FileWatcher( m_lastFileName );
		connect( m_watcher, SIGNAL(fileChanged()), this, SLOT(fileChanged()) );
	} else
		m_watcher->activate();
}

void AbstractFileEditor::setWatcher( const QString & path ) {
	if( m_lastFileName != path ) {
		delete m_watcher;
		m_lastFileName = path;
		activateWatcher();
	}
}

void AbstractFileEditor::serialize( XinxProjectSessionEditor * data, bool content ) {
	AbstractEditor::serialize( data, content );

	data->writeProperty( "FileName", QVariant( m_lastFileName ) );
	data->writeProperty( "Modified", QVariant( m_modified ) );
}

void AbstractFileEditor::deserialize( XinxProjectSessionEditor * data ) {
	m_lastFileName = data->readProperty( "FileName" ).toString();
	m_modified = data->readProperty( "Modified" ).toBool();

	activateWatcher();

	AbstractEditor::deserialize( data );
}
