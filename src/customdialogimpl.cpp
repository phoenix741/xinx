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

#include "customdialogimpl.h"

/* CustomDialogImpl */

//
CustomDialogImpl::CustomDialogImpl( QWidget * parent, Qt::WFlags f)  : QDialog(parent, f) {
	setupUi(this);
}
//

void CustomDialogImpl::loadFromConfig( XINXConfig * config ) {
	myConfig = *config;
	
	m_alertStandardCheckBox->setChecked( myConfig.isAlertWhenStdFile() );
	m_createBakCheckBox->setChecked( myConfig.isCreateBackupFile() );
	int index = m_langComboBox->findText( QString("(%1)").arg( myConfig.lang() ) );
	m_langComboBox->setCurrentIndex( index );
	
	m_projectPathLineEdit->setText( myConfig.xinxProjectPath() );
	m_objectDescriptionPathLineEdit->setText( myConfig.objectDescriptionPath() );
	
	m_fileTypeComboBox->clear();
	foreach( struct XINXConfig::managedFile file, myConfig.managedFile() ) {
		m_fileTypeComboBox->addItem( QString("%1 (%2)").arg( file.name ).arg( file.extentions ) );
	}

	m_syntaxFileTypeComboBox->clear();
	foreach( QString cle, myConfig.managedStructure().keys() ) {
		m_syntaxFileTypeComboBox->addItem( cle );
	}
}

void CustomDialogImpl::saveToConfig( XINXConfig * config ) {
//	*config = myConfig;
}
