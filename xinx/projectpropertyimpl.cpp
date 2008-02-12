/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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
#include "projectpropertyimpl.h"
#include "xslproject.h"
#include "xinxconfig.h"
#include "exceptions.h"

// Qt header
#include <QDir>
#include <QInputDialog>

ProjectPropertyImpl::ProjectPropertyImpl( QWidget * parent, Qt::WFlags f) : QDialog(parent, f), m_versionInstance( NULL ) {
	setupUi(this);

	m_pathListLabel->setBuddy( m_searchPathList );
	m_prefixLabel->setBuddy( m_prefixList );
	m_specifiqueProjectPathLineEdit->setValidator( new QRegExpValidator( QRegExp( "[\\w]*" ), m_specifiqueProjectPathLineEdit ) );
	m_servicesList->setDefaultVisible( false );
}

ProjectPropertyImpl::~ProjectPropertyImpl() {
	if( m_versionInstance ) {
		m_versionInstance->wait();
		delete m_versionInstance;
	}
}

void ProjectPropertyImpl::on_m_projectButton_clicked() {
	m_projectLineEdit->changePath( this, XINXConfig::self()->config().project.defaultPath );
}

void ProjectPropertyImpl::on_m_prefixList_defaultValueChanged( QString text ) {
	Q_UNUSED( text );
	
	updateOkButton();
}

void ProjectPropertyImpl::on_m_projectLineEdit_textChanged( QString text ) {
	QDir dir (text);
	QPalette paletteVerion( m_configurationVersionLabel->palette() );
	
	if( m_versionInstance ) {
		m_versionInstance->wait();
		delete m_versionInstance;
		m_versionInstance = NULL;
	}
	
	if( dir.exists() ) {
		paletteVerion.setColor( QPalette::WindowText, Qt::red );

		if( ConfigurationFile::exists( text ) || MetaConfigurationFile::exists( text ) ) {
			m_configurationVersionLabel->setText( tr("Search version of file ...") );
			m_presentationLabel->setText( "" );
			m_versionInstance = ThreadedConfigurationFile::simpleConfigurationFile( text );
			connect( m_versionInstance, SIGNAL(versionFinded(SimpleConfigurationFile)), this, SLOT(versionFinded(SimpleConfigurationFile)) );
			m_versionInstance->start();
		} else {
			m_configurationVersionLabel->setText( tr("warning: The configuration file is not in this directory.") );
			m_presentationLabel->setText( "" );
		}
	} else {
		paletteVerion.setColor( QPalette::WindowText, QColor() );
		m_configurationVersionLabel->setText( QString() );
	}
	m_configurationVersionLabel->setPalette( paletteVerion );

	updateOkButton();
}

void ProjectPropertyImpl::versionFinded( SimpleConfigurationFile configuration ) {
	QPalette paletteVerion( m_configurationVersionLabel->palette() );
	if( configuration.version().isValid() ) {
		paletteVerion.setColor( QPalette::WindowText, QColor() );
		m_configurationVersionLabel->setText( configuration.version().toString() );
		m_presentationLabel->setText( configuration.xmlPresentationFile() );
	} else {
		paletteVerion.setColor( QPalette::WindowText, Qt::red );
		m_configurationVersionLabel->setText( tr("warning: A valid version cannot be found in the configuration file (may be 4xx).") );
		m_presentationLabel->setText( configuration.xmlPresentationFile() );
	}
	m_configurationVersionLabel->setPalette( paletteVerion );
}

void ProjectPropertyImpl::loadFromProject( XSLProject * project ) {
	m_nameLineEdit->setText( project->projectName() );
	m_projectLineEdit->setText( QDir::toNativeSeparators( project->projectPath() ) );
	m_langComboBox->setCurrentIndex( m_langComboBox->findText( project->defaultLang() ) );
	m_navigatorComboBox->setCurrentIndex( m_navigatorComboBox->findText( project->defaultNav() ) );
	m_specifiqueProjectPathLineEdit->setText( project->specifiquePathName() );
	
	m_prefixList->setDefaultValue( project->specifiquePrefix() );
	m_prefixList->setValues( project->specifiquePrefixes() );

	m_specifiqueGroupBox->setChecked( project->options().testFlag( XSLProject::hasSpecifique ) );
	m_webServiceGroupBox->setChecked( project->options().testFlag( XSLProject::hasWebServices ) );
	m_logLineEdit->setText( QDir::toNativeSeparators( project->logProjectDirectory() ) );
	
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
	
	m_servicesList->setValues( project->serveurWeb() );

	QString defSearchPath = project->searchPathList().at( project->indexOfSpecifiquePath() );

	m_searchPathList->setDefaultValue( defSearchPath );
	m_searchPathList->setValues( project->searchPathList() ); // fromNativeSeparators

	updateOkButton();
}

void ProjectPropertyImpl::saveToProject( XSLProject * project ) {
	project->setProjectName( m_nameLineEdit->text() );
	project->setProjectPath( QDir::fromNativeSeparators( m_projectLineEdit->text() ) );
	project->setDefaultLang( m_langComboBox->currentText() );
	project->setDefaultNav( m_navigatorComboBox->currentText() );
	project->setSpecifiquePathName( m_specifiqueProjectPathLineEdit->text() );

	project->searchPathList() = m_searchPathList->values(); // toNativeSeparators
	project->setIndexOfSpecifiquePath( project->searchPathList().indexOf( m_searchPathList->defaultValue() ) );

	project->setSpecifiquePrefix( m_prefixList->defaultValue() );
	project->specifiquePrefixes() = m_prefixList->values();
	
	project->setProjectRCS( (XSLProject::enumProjectRCS)m_projectRCSComboBox->currentIndex() );
	project->setLogProjectDirectory( QDir::fromNativeSeparators( m_logLineEdit->text() ) );

	XSLProject::ProjectOptions options;
	if( m_specifiqueGroupBox->isChecked() )
		options |= XSLProject::hasSpecifique;
	if( m_webServiceGroupBox->isChecked() )
		options |= XSLProject::hasWebServices;
	project->setOptions( options );

	project->serveurWeb() = m_servicesList->values();
}

void ProjectPropertyImpl::updateOkButton() {
	bool projectLineOk = ! ( m_projectLineEdit->text().isEmpty() || !QDir( m_projectLineEdit->text() ).exists() ),
	     hasSpecif     = m_specifiqueGroupBox->isChecked(),
	     specifLineOk  = true,//! ( m_specifiquePathLineEdit->text().isEmpty() || !QDir( m_specifiquePathLineEdit->text() ).exists() ),
	     prefixLineOk  = ! m_prefixList->defaultValue().isEmpty(),
	     okButtonEnabled = projectLineOk && ( (!hasSpecif) || ( specifLineOk && prefixLineOk ) );

	okButton->setEnabled( okButtonEnabled );
}

void ProjectPropertyImpl::on_m_specifiqueGroupBox_clicked() {
	updateOkButton();
}

void ProjectPropertyImpl::on_m_logButton_clicked() {
	m_logLineEdit->changePath( this, XINXConfig::self()->config().project.defaultPath );
}