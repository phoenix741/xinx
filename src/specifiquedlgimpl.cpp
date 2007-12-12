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
#include "globals.h"
#include "exceptions.h"
#include "xslproject.h"

// Qt header
#include <QFileInfo>

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
	if( global.m_config->config().files.count( suffix ) > 0 )
		result = global.m_config->config().files[ suffix ];
	return result;
}

bool SpecifiqueDialogImpl::isSpecifique( const QString & filename ) {
	return 	global.m_project && 
			QFileInfo( filename ).fileName().startsWith( global.m_project->specifiquePrefix(), Qt::CaseInsensitive );
}

bool SpecifiqueDialogImpl::canBeSaveAsSpecifique( const QString & filename ) {
	return 	global.m_project &&
			global.m_project->options().testFlag( XSLProject::hasSpecifique ) &&
			extentionOfFileName( filename ).canBeSpecifique &&
			global.m_config->config().project.alertWhenSavingStandardFile;
}

bool SpecifiqueDialogImpl::canBeAddedToRepository( const QString & filename ) {
	Q_UNUSED( filename );
	
	return 	global.m_project &&
			( global.m_project->projectRCS() != XSLProject::NORCS );
}

void SpecifiqueDialogImpl::setFileName( const QString & filename ) {
	XINX_ASSERT( global.m_project );

	m_filename = filename;
	m_specifiqueCheckBox->setEnabled( (!isSpecifique( filename ) ) && canBeSaveAsSpecifique( filename ) );
	m_repositoryCheckBox->setEnabled( canBeAddedToRepository( filename ) );
}

QString SpecifiqueDialogImpl::path() const {
	if( m_specifiqueCheckBox->enabled() && m_specifiqueCheckBox->checked() ) { 
		QString fileSuffix = suffix.isEmpty() ? QFileInfo( m_filename ).completeSuffix() : suffix;
		struct_extentions customFile = extentionOfFileName( fileSuffix );
		return QDir( global.m_project->processedSpecifiquePath() ).absoluteFilePath( customFile.customPath );
	} else
	 	return m_lastPlace;
}

QString SpecifiqueDialogImpl::filename() const {
	QString newFileName = QString();
	
	newFileName = QFileInfo( fileName ).fileName();
	if( m_specifiqueCheckBox->enabled() && m_specifiqueCheckBox->checked() ) 
		newFileName = global.m_project->specifiquePrefix().toLower() + "_" + newFileName;
	
	return newFileName;
}

QString SpecifiqueDialogImpl::saveFileAs( const QString & filename, const QString & suffix, QStringList & filesForRepository ) {
	QString newFilename = filename;
	
	SpecifiqueDialogImpl dlg;
	dlg.setFileName( filename );
	dlg.m_suffix = suffix;
	if( ( canBeAddedToRepository( filename ) || ( (!isSpecifique( filename ) ) && canBeSaveAsSpecifique( filename ) ) ) && dlg.exec() ) 
		if( (!filename.isEmpty()) && dlg.m_repositoryCheckBox->isChecked() && dlg.m_repositoryCheckBox->isEnabled() ) 
			filesForRepository << filename;

	if( filename != newFilename )
		filesForRepository << newFilename;
	return newFilename;
}

QString SpecifiqueDialogImpl::saveFileAsIfStandard( const QString & filename, QStringList & filesForRepository ) {
	if( (!isSpecifique( filename )) && canBeSaveAsSpecifique( filename ) )
		return saveFileAs( filename, QString(), filesForRepository );
	return filename;
}

