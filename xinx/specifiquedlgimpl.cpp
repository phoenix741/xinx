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
#include "exceptions.h"
#include "xslproject.h"
#include "xinxpluginsloader.h"

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

struct_extentions SpecifiqueDialogImpl::extentionOfFileName( const QString & name ) {
	XINX_TRACE( "SpecifiqueDialogImpl::extentionOfFileName", QString( "( %1 )" ).arg( name ) );

	struct_extentions result;
	int dotPosition = name.lastIndexOf( "." );
	QString suffix = name.toLower();
	if( dotPosition >= 0 )
		suffix = suffix.mid( dotPosition + 1 );
	if( XINXConfig::self()->config().files.count( suffix ) > 0 )
		result = XINXConfig::self()->config().files[ suffix ];
	return result;
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
			extentionOfFileName( filename ).canBeSpecifique &&
			XINXConfig::self()->config().project.alertWhenSavingStandardFile &&
			QFileInfo( filename ).absolutePath().contains( XINXProjectManager::self()->project()->projectPath() );
}

bool SpecifiqueDialogImpl::canBeAddedToRepository( const QString & filename ) {
	Q_UNUSED( filename );
	
	return 	XINXProjectManager::self()->project() &&
		  ( XINXProjectManager::self()->project()->projectRCS() != XSLProject::NORCS );
}

void SpecifiqueDialogImpl::setLastPlace( const QString & pathname ) {
	m_lastPlace = pathname;
}

QString SpecifiqueDialogImpl::lastPlace() {
	return m_lastPlace;
}

void SpecifiqueDialogImpl::setFileName( const QString & filename ) {
	XINX_ASSERT( XINXProjectManager::self()->project() );

	m_filename = filename;
	m_specifiqueCheckBox->setEnabled( (!filename.isEmpty()) && (!isSpecifique( filename )) && canBeSaveAsSpecifique( filename ) );
	m_repositoryCheckBox->setEnabled( canBeAddedToRepository( filename ) );
}

QString SpecifiqueDialogImpl::path() const {
	if( m_specifiqueCheckBox->isChecked() ) { 
		QString fileSuffix = m_suffix.isEmpty() ? QFileInfo( m_filename ).completeSuffix() : m_suffix;
		struct_extentions customFile = extentionOfFileName( fileSuffix );
		return QDir( XINXProjectManager::self()->project()->processedSpecifiquePath() ).absoluteFilePath( customFile.customPath );
	} else if( !m_filename.isEmpty() )
		return QFileInfo( m_filename ).absolutePath();
	else
	 	return m_lastPlace;
}

QString SpecifiqueDialogImpl::filename() const {
	QString newFileName;
	
	newFileName = QFileInfo( m_filename ).fileName();
	if( m_specifiqueCheckBox->isChecked() && (! isSpecifique( m_filename ) ) ) 
		newFileName = XINXProjectManager::self()->project()->specifiquePrefix() + "_" + newFileName;
	
	return newFileName;
}

QString SpecifiqueDialogImpl::saveFileAs( const QString & filename, const QString & suffix, QStringList & filesForRepository ) {
	QString newFilename = filename,
			usedSuffix  = suffix.isEmpty() ? QFileInfo( filename ).completeSuffix() : suffix,
			filter 		= XinxPluginsLoader::self()->filter( usedSuffix );
			
	bool saveToRepository = false;
	
	SpecifiqueDialogImpl dlg;

	if( ( canBeAddedToRepository( filename ) || ( (!isSpecifique( filename ) ) && canBeSaveAsSpecifique( filename ) ) ) ) {
		dlg.setFileName( filename );
		dlg.m_suffix = suffix;
	
		if( dlg.exec() ) {
			newFilename = QDir( dlg.path() ).absoluteFilePath( dlg.filename() );
			saveToRepository = dlg.m_repositoryCheckBox->isChecked() && dlg.m_repositoryCheckBox->isEnabled();
		} else
			return QString();
	} else {
		newFilename = QDir( m_lastPlace ).absoluteFilePath( QFileInfo( filename ).fileName() );
	}
	
	newFilename = QFileDialog::getSaveFileName( &dlg, tr("Save text file"), newFilename, XinxPluginsLoader::self()->filters().join(";;"), &filter );
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
	XINX_ASSERT( ! filename.isEmpty() );
	if( (!isSpecifique( filename )) && canBeSaveAsSpecifique( filename ) ) {
		SpecifiqueDialogImpl dlg;
		dlg.setFileName( filename );
		if( dlg.exec() ) {
			QString path 	= QDir( dlg.path() ).absoluteFilePath( dlg.filename() ),
					filter	= XinxPluginsLoader::self()->filter( QFileInfo( filename ).completeSuffix() );
			
			if( dlg.m_specifiqueCheckBox->isEnabled() && dlg.m_specifiqueCheckBox->isChecked() ) {
				path = QFileDialog::getSaveFileName( &dlg, tr("Save text file"), path, XinxPluginsLoader::self()->filters().join(";;"), &filter );
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

