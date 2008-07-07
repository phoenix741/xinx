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
#include "specifiquedlgimpl.h"
#include <exceptions.h>
#include <xslproject.h>
#include <xinxpluginsloader.h>
#include <xinxconfig.h>

// Qt header
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>

/* SpecifiqueDialogImpl */

QString SpecifiqueDialogImpl::m_lastPlace = QDir::currentPath();

SpecifiqueDialogImpl::SpecifiqueDialogImpl( QWidget * parent, Qt::WFlags f ) : QDialog( parent, f ) {
	setupUi( this );

	m_specifiqueCheckBox->setChecked( true );
	m_repositoryCheckBox->setChecked( true );
}

bool SpecifiqueDialogImpl::isSpecifique( const QString & filename ) {
	if( ! XINXProjectManager::self()->project() ) return false;

	foreach( QString prefix, XINXProjectManager::self()->project()->specifiquePrefixes() ) {
		if( QFileInfo( filename ).fileName().startsWith( prefix + "_", Qt::CaseInsensitive ) )
			return true;
	}
	return false;
}

bool SpecifiqueDialogImpl::canBeSaveAsSpecifique( const QString & filename ) {
	return 	XINXProjectManager::self()->project() &&
		XINXProjectManager::self()->project()->options().testFlag( XSLProject::hasSpecifique ) &&
		XINXConfig::self()->matchedFileType( filename ).canBeSpecifique &&
		QFileInfo( filename ).absolutePath().contains( XINXProjectManager::self()->project()->projectPath() );
}

bool SpecifiqueDialogImpl::canBeAddedToRepository( const QString & filename ) {
	Q_UNUSED( filename );

	return XINXProjectManager::self()->project() && ( ! XINXProjectManager::self()->project()->projectRCS().isEmpty() );
}

void SpecifiqueDialogImpl::setLastPlace( const QString & pathname ) {
	m_lastPlace = pathname;
}

QString SpecifiqueDialogImpl::lastPlace() {
	return m_lastPlace;
}

void SpecifiqueDialogImpl::setFileName( const QString & filename ) {
	Q_ASSERT( XINXProjectManager::self()->project() );

	m_filename = filename;
	m_specifiqueCheckBox->setEnabled( (! filename.isEmpty() ) && ( ! isSpecifique( filename ) ) && canBeSaveAsSpecifique( filename ) );
	m_repositoryCheckBox->setEnabled( canBeAddedToRepository( filename ) );
}

QString SpecifiqueDialogImpl::path() const {
	if( m_specifiqueCheckBox->isChecked() ) {
		QString newFilename = m_filename.isEmpty() ? m_defaultFileName : m_filename;
		AppSettings::struct_extentions customFile = XINXConfig::self()->matchedFileType( newFilename );
		return QDir( XINXProjectManager::self()->project()->processedSpecifiquePath() ).absoluteFilePath( customFile.customPath );
	} else if( !m_filename.isEmpty() )
		return QFileInfo( m_filename ).absolutePath();
	else
	 	return m_lastPlace;
}

QString SpecifiqueDialogImpl::filename() const {
	QString newFilename = QFileInfo( m_filename ).fileName();

	if( m_specifiqueCheckBox->isChecked() && (! isSpecifique( m_filename ) ) )
		newFilename = XINXProjectManager::self()->project()->specifiquePrefix() + "_" + newFilename;

	return newFilename;
}

QString SpecifiqueDialogImpl::saveFileAs( const QString & filename, const QString & defaultFileName, QStringList & filesForRepository ) {
	QString newFilename = filename.isEmpty() ? defaultFileName : filename,
			filter 		= XinxPluginsLoader::self()->fileTypeFilter( XinxPluginsLoader::self()->matchedFileType( newFilename ) );

	bool saveToRepository = false;

	SpecifiqueDialogImpl dlg;

	if( ( canBeAddedToRepository( newFilename ) || ( (!filename.isEmpty()) && (!isSpecifique( filename )) && canBeSaveAsSpecifique( filename ) ) ) ) {
		dlg.setFileName( filename );
		dlg.m_defaultFileName = defaultFileName;

		if( dlg.exec() ) {
			newFilename = QDir( dlg.path() ).absoluteFilePath( dlg.filename() );
			saveToRepository = dlg.m_repositoryCheckBox->isChecked() && dlg.m_repositoryCheckBox->isEnabled();
		} else
			return QString();
	} else {
		newFilename = QDir( m_lastPlace ).absoluteFilePath( QFileInfo( filename ).fileName() );
	}

	newFilename = QFileDialog::getSaveFileName( &dlg, tr("Save text file"), newFilename, XinxPluginsLoader::self()->managedFilters().join(";;"), &filter );
	if( newFilename.isEmpty() )
		return QString();

	m_lastPlace = QFileInfo( newFilename ).absolutePath();

	if( saveToRepository ) {
		filesForRepository << QDir( dlg.path() ).absoluteFilePath( QFileInfo( filename ).fileName() );
		if( filename != newFilename )
			filesForRepository << newFilename;
	}

	if( ( filename != newFilename ) && (!isSpecifique( filename )) && isSpecifique( newFilename ) ) {
		QFile::copy( filename, QDir( dlg.path() ).absoluteFilePath( QFileInfo( filename ).fileName() ) );
	}

	return newFilename;
}

QString SpecifiqueDialogImpl::saveFileAsIfStandard( const QString & filename, QStringList & filesForRepository ) {
	Q_ASSERT( ! filename.isEmpty() );

	if( (!isSpecifique( filename )) && canBeSaveAsSpecifique( filename ) ) {
		SpecifiqueDialogImpl dlg;
		dlg.setFileName( filename );
		if( dlg.exec() ) {
			QString path 	= QDir( dlg.path() ).absoluteFilePath( dlg.filename() ),
					filter	= XinxPluginsLoader::self()->fileTypeFilter( XinxPluginsLoader::self()->matchedFileType( filename ) );

			if( dlg.m_specifiqueCheckBox->isEnabled() && dlg.m_specifiqueCheckBox->isChecked() ) {
				path = QFileDialog::getSaveFileName( &dlg, tr("Save text file"), path, XinxPluginsLoader::self()->managedFilters().join(";;"), &filter );
				if( ! path.isEmpty() )
					m_lastPlace = QFileInfo( path ).absolutePath();
			}

			if( dlg.m_repositoryCheckBox->isChecked() && dlg.m_repositoryCheckBox->isEnabled() ) {
				filesForRepository << QDir( dlg.path() ).absoluteFilePath( QFileInfo( filename ).fileName() );
				if( filename != path )
					filesForRepository << path;
			}

			if( ( filename != path ) && (!isSpecifique( filename )) && isSpecifique( path ) ) {
				QFile::copy( filename, QDir( dlg.path() ).absoluteFilePath( QFileInfo( filename ).fileName() ) );
			}

			return path;
		} else
			return QString(); // Annulation
	} else
		return filename;
}

