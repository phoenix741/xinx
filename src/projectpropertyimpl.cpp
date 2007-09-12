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

#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <assert.h>

#include "projectpropertyimpl.h"
#include "xslproject.h"
#include "xinxconfig.h"
#include "globals.h"

ProjectPropertyImpl::ProjectPropertyImpl( QWidget * parent, Qt::WFlags f) : QDialog(parent, f), m_versionInstance( NULL ) {
	setupUi(this);
}

ProjectPropertyImpl::~ProjectPropertyImpl() {
	if( m_versionInstance ) 
		delete m_versionInstance;
}

void ProjectPropertyImpl::on_m_projectButton_clicked() {
	QString value = m_projectLineEdit->text();
	if( value.isEmpty() ) 
		value = global.m_config->config().project.defaultPath;
		
	value = QFileDialog::getExistingDirectory( this, tr("Project path"), value );
	if( ! value.isEmpty() ) {
		m_projectLineEdit->setText( value );		
	}
}

void ProjectPropertyImpl::on_m_prefixLineEdit_textChanged( QString text ) {
	Q_UNUSED( text );
	
	m_prefixLineEdit->setText( text.toUpper() );
	updateOkButton();
}

void ProjectPropertyImpl::on_m_projectLineEdit_textChanged( QString text ) {
	QDir dir (text);
	QPalette paletteVerion( m_configurationVersionLabel->palette() );
	
	if( m_versionInstance ) {
		delete m_versionInstance;
		m_versionInstance = NULL;
	}
	
	if( dir.exists() ) {
		paletteVerion.setColor( QPalette::WindowText, Qt::red );

		if( ConfigurationFile::exists( text ) ) {
			m_configurationVersionLabel->setText( tr("Search version of file ...") );
			m_versionInstance = ThreadedConfigurationFile::version( text );
			connect( m_versionInstance, SIGNAL(versionFinded(ConfigurationVersion)), this, SLOT(versionFinded(ConfigurationVersion)) );
			m_versionInstance->start();
		} else
			m_configurationVersionLabel->setText( tr("warning: The configuration file is not in this directory.") );
	} else {
		paletteVerion.setColor( QPalette::WindowText, QColor() );
		m_configurationVersionLabel->setText( QString() );
	}
	m_configurationVersionLabel->setPalette( paletteVerion );

	updateOkButton();
	updateSpecifiquePath();
}

void ProjectPropertyImpl::versionFinded( ConfigurationVersion version ) {
	QPalette paletteVerion( m_configurationVersionLabel->palette() );
	if( version.isValid() ) {
		paletteVerion.setColor( QPalette::WindowText, QColor() );
		m_configurationVersionLabel->setText( version.toString() );
	} else {
		paletteVerion.setColor( QPalette::WindowText, Qt::red );
		m_configurationVersionLabel->setText( tr("warning: A valid version cannot be found in the configuration file.") );
	}
	m_configurationVersionLabel->setPalette( paletteVerion );
}

void ProjectPropertyImpl::loadFromProject( XSLProject * project ) {
	m_nameLineEdit->setText( project->projectName() );
	m_projectLineEdit->setText( project->projectPath() );
	m_langComboBox->setCurrentIndex( m_langComboBox->findText( project->defaultLang() ) );
	m_navigatorComboBox->setCurrentIndex( m_navigatorComboBox->findText( project->defaultNav() ) );
	m_specifiqueProjectPathLineEdit->setText( project->specifiqueProjectPathName() );
	//m_specifiquePathLineEdit->setText( project->specifPath() );
	m_prefixLineEdit->setText( project->specifPrefix() );
	m_standardProjectCheckBox->setChecked( ! project->options().testFlag( XSLProject::hasSpecifique ) );
	m_webServicesCheckBox->setChecked( project->options().testFlag( XSLProject::hasWebServices ) );
	
	switch( project->projectRCS() ) {
	case XSLProject::NORCS :
		m_projectRCSComboBox->setCurrentIndex( 0 );
		break;
	case XSLProject::CVS :
		m_projectRCSComboBox->setCurrentIndex( 1 );
		break;
	case XSLProject::SUBVERSION :
		m_projectRCSComboBox->setCurrentIndex( 2 );
		break;
	}
	
	m_webServiceList->clear();
	foreach( QString link, project->serveurWeb() ) {
		m_webServiceList->addItem( link );
	}
	m_webServiceBtnDel->setEnabled( m_webServiceList->count() > 0 );

	m_searchPathList->clear();
	foreach( QString link, project->searchPathList() ) {
		m_searchPathList->addItem( link );
	}
	m_searchPathBtnDef->setEnabled( m_searchPathList->count() > 0 );
	m_searchPathBtnDel->setEnabled( m_searchPathList->count() > 0 );
}

void ProjectPropertyImpl::saveToProject( XSLProject * project ) {
	project->setProjectName( m_nameLineEdit->text() );
	project->setProjectPath( m_projectLineEdit->text() );
	project->setDefaultLang( m_langComboBox->currentText() );
	project->setDefaultNav( m_navigatorComboBox->currentText() );
	project->setSpecifiqueProjectPathName( m_specifiqueProjectPathLineEdit->text() );
	//project->setSpecifPath( m_specifiquePathLineEdit->text() );
	project->setSpecifPrefix( m_prefixLineEdit->text() );
	project->setProjectRCS( (XSLProject::enumProjectRCS)m_projectRCSComboBox->currentIndex() );
	XSLProject::ProjectOptions options;
	if( ! m_standardProjectCheckBox->isChecked() )
		options |= XSLProject::hasSpecifique;
	if( m_webServicesCheckBox->isChecked() )
		options |= XSLProject::hasWebServices;
	project->setOptions( options );

	project->serveurWeb().clear();
	for( int i = 0; i < m_webServiceList->count(); i++ ) {
		project->serveurWeb().append( m_webServiceList->item( i )->text() );
	}
	project->searchPathList().clear();
	for( int i = 0; i < m_searchPathList->count(); i++ ) {
		project->searchPathList().append( m_searchPathList->item( i )->text() );
	}
}

void ProjectPropertyImpl::updateSpecifiquePath() {
	QString path = QString("%1/langue/%2/nav/%3").arg( m_projectLineEdit->text() ).arg( m_langComboBox->currentText().toLower() ).arg( global.m_config->config().project.defaultProjectPathName );
	
	m_specifiquePathLabel->setText( QDir::cleanPath( path ) );
}

void ProjectPropertyImpl::updateOkButton() {
	bool projectLineOk = ! ( m_projectLineEdit->text().isEmpty() || !QDir( m_projectLineEdit->text() ).exists() ),
	     hasSpecif     = ! m_standardProjectCheckBox->isChecked(),
	     specifLineOk  = true,//! ( m_specifiquePathLineEdit->text().isEmpty() || !QDir( m_specifiquePathLineEdit->text() ).exists() ),
	     prefixLineOk  = ! m_prefixLineEdit->text().isEmpty(),
	     okButtonEnabled = projectLineOk && ( (!hasSpecif) || ( specifLineOk && prefixLineOk ) );

	okButton->setEnabled( okButtonEnabled );
}

void ProjectPropertyImpl::on_m_langComboBox_currentIndexChanged( QString str ) {
	Q_UNUSED( str );
	
	updateSpecifiquePath();
}


void ProjectPropertyImpl::on_m_webServiceBtnDel_clicked() {
	assert( m_webServiceList->currentRow() >= 0 );
	
	delete m_webServiceList->currentItem();
	
	m_webServiceBtnDel->setEnabled( m_webServiceList->count() > 0 );
}

void ProjectPropertyImpl::on_m_webServiceBtnAdd_clicked() {
	QString text = QInputDialog::getText( this, tr("Add WebService"), tr("URL of the WebServices"), QLineEdit::Normal, "http://localhost:8888/gce120/services/" );
	if( ! text.isEmpty() )
		m_webServiceList->addItem( text );

	m_webServiceBtnDel->setEnabled( m_webServiceList->count() > 0 );
}


void ProjectPropertyImpl::on_m_searchPathBtnAdd_clicked() {
	QString text = QInputDialog::getText( this, tr("Add Search Path"), tr("Search Path"), QLineEdit::Normal, "langues/<lang>/nav/<project>" );
	if( ! text.isEmpty() )
		m_searchPathList->addItem( text );

	m_searchPathBtnDel->setEnabled( m_searchPathList->count() > 0 );
	m_searchPathBtnDef->setEnabled( m_searchPathList->count() > 0 );
}

void ProjectPropertyImpl::on_m_searchPathBtnDel_clicked() {
	assert( m_searchPathList->currentRow() >= 0 );
	
	delete m_searchPathList->currentItem();
	
	m_searchPathBtnDel->setEnabled( m_searchPathList->count() > 0 );
	m_searchPathBtnDef->setEnabled( m_searchPathList->count() > 0 );
}


void ProjectPropertyImpl::on_m_standardProjectCheckBox_clicked() {
	updateOkButton();
}

void ProjectPropertyImpl::on_m_relativeLineEditPath_textChanged( QString text ) {
	QListWidgetItem * item = m_searchPathList->currentItem();
	if( item ) 
		item->setText( text );
}

void ProjectPropertyImpl::on_m_searchPathBtnDef_clicked() {
	QListWidgetItem * item = m_searchPathList->currentItem();
	if( item ) {
		for( int i = 0; i < m_searchPathList->count() ; i++ ) {
			QListWidgetItem * item = m_searchPathList->item( i );
			QFont font = item->font();
			font.setBold( false );
			item->setFont( font );
		}
		defaultSearchPath = item->text();
		QFont font = item->font();
		font.setBold( true );
		item->setFont( font );
	}
}
