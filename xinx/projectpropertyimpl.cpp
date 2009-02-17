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
#include "projectpropertyimpl.h"
#include <core/xinxproject.h>
#include <core/xinxconfig.h>
#include <core/exceptions.h>
#include <plugins/xinxpluginsloader.h>

// Qt header
#include <QDir>
#include <QInputDialog>
#include <QPushButton>

ProjectPropertyImpl::ProjectPropertyImpl( QWidget * parent, Qt::WFlags f) : QDialog(parent, f), m_versionInstance( NULL ) {
	setupUi(this);

	m_buttonBox->button( QDialogButtonBox::Ok )->setDisabled( true );

	m_pathListLabel->setBuddy( m_searchPathList );
	m_prefixLabel->setBuddy( m_prefixList );
	m_specifiqueProjectPathLineEdit->setValidator( new QRegExpValidator( QRegExp( "[\\w]*" ), m_specifiqueProjectPathLineEdit ) );

	m_projectRCSComboBox->addItem( tr("<No Revision Control System>") );
	QPair<QString,QString> revisionControl;
	foreach( revisionControl, XinxPluginsLoader::self()->revisionsControls() )
		m_projectRCSComboBox->addItem( revisionControl.second, revisionControl.first );

	foreach( XinxPluginElement * element, XinxPluginsLoader::self()->plugins() ) {
		if( element->isActivated() && qobject_cast<IXinxPluginProjectConfiguration*>( element->plugin() ) ) {
			IXinxPluginProjectConfiguration* interface = qobject_cast<IXinxPluginProjectConfiguration*>( element->plugin() );
			QWidget* widget = interface->createProjectSettingsPage();
			if( widget ) {
				m_pluginPages.append( qMakePair( interface, widget ) );
				m_tabWidget->addTab( widget, widget->windowTitle() );
			}
		}
	}
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
			connect( m_versionInstance, SIGNAL(versionFinded(ConfigurationFile)), this, SLOT(versionFinded(ConfigurationFile)) );
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

void ProjectPropertyImpl::versionFinded( ConfigurationFile configuration ) {
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

void ProjectPropertyImpl::loadFromProject( XinxProject * project ) {
	m_nameLineEdit->setText( project->projectName() );
	m_projectLineEdit->setText( QDir::toNativeSeparators( project->projectPath() ) );
	if( m_langComboBox->findText( project->defaultLang() ) < 0 ) m_langComboBox->addItem( project->defaultLang() );
	m_langComboBox->setCurrentIndex( m_langComboBox->findText( project->defaultLang() ) );
	if( m_navigatorComboBox->findText( project->defaultNav() ) < 0 ) m_navigatorComboBox->addItem( project->defaultNav() );
	m_navigatorComboBox->setCurrentIndex( m_navigatorComboBox->findText( project->defaultNav() ) );
	m_specifiqueProjectPathLineEdit->setText( project->specifiquePathName() );

	m_prefixList->setDefaultValue( project->specifiquePrefix() );
	m_prefixList->setValues( project->specifiquePrefixes() );

	m_specifiqueGroupBox->setChecked( project->options().testFlag( XinxProject::hasSpecifique ) );
	m_logLineEdit->setText( QDir::toNativeSeparators( project->logProjectDirectory() ) );

	int index = m_projectRCSComboBox->findData( project->projectRCS() );
	if( index < 0 ) index = 0;
	m_projectRCSComboBox->setCurrentIndex( index );

	QString defSearchPath;
	int indexOfSpecifquePath = project->indexOfSpecifiquePath();
	if( ( indexOfSpecifquePath >= 0 ) && ( indexOfSpecifquePath < project->searchPathList().size() ) )
		defSearchPath = project->searchPathList().at( indexOfSpecifquePath );

	m_searchPathList->setDefaultValue( defSearchPath );
	m_searchPathList->setValues( project->searchPathList() ); // fromNativeSeparators

	QPair<IXinxPluginProjectConfiguration*,QWidget*> page;
	foreach( page, m_pluginPages ) {
		if( ! page.first->loadProjectSettingsPage( page.second ) ) qWarning( qPrintable( tr("Can't load \"%1\" page").arg( page.second->windowTitle() ) ) );
	}

	updateOkButton();
}

void ProjectPropertyImpl::saveToProject( XinxProject * project ) {
	project->blockSignals( true );

	project->setProjectName( m_nameLineEdit->text() );
	project->setProjectPath( QDir::fromNativeSeparators( m_projectLineEdit->text() ) );
	project->setDefaultLang( m_langComboBox->currentText() );
	project->setDefaultNav( m_navigatorComboBox->currentText() );
	project->setSpecifiquePathName( m_specifiqueProjectPathLineEdit->text() );

	project->searchPathList() = m_searchPathList->values(); // toNativeSeparators
	project->setIndexOfSpecifiquePath( project->searchPathList().indexOf( m_searchPathList->defaultValue() ) );

	project->setSpecifiquePrefix( m_prefixList->defaultValue() );
	project->specifiquePrefixes() = m_prefixList->values();

	project->setProjectRCS( m_projectRCSComboBox->itemData( m_projectRCSComboBox->currentIndex() ).toString() );
	project->setLogProjectDirectory( QDir::fromNativeSeparators( m_logLineEdit->text() ) );

	XinxProject::ProjectOptions options;
	if( m_specifiqueGroupBox->isChecked() )
		options |= XinxProject::hasSpecifique;
	project->setOptions( options );

	QPair<IXinxPluginProjectConfiguration*,QWidget*> page;
	foreach( page, m_pluginPages ) {
		if( ! page.first->saveProjectSettingsPage( page.second ) ) qWarning( qPrintable( tr("Can't save \"%1\" page").arg( page.second->windowTitle() ) ) );
	}

	project->blockSignals( false );
}

void ProjectPropertyImpl::updateOkButton() {
	bool projectLineOk = ! ( m_projectLineEdit->text().isEmpty() || !QDir( m_projectLineEdit->text() ).exists() ),
	     hasSpecif     = m_specifiqueGroupBox->isChecked(),
	     specifLineOk  = true,//! ( m_specifiquePathLineEdit->text().isEmpty() || !QDir( m_specifiquePathLineEdit->text() ).exists() ),
	     prefixLineOk  = ! m_prefixList->defaultValue().isEmpty(),
	     okButtonEnabled = projectLineOk && ( (!hasSpecif) || ( specifLineOk && prefixLineOk ) );

	m_buttonBox->button( QDialogButtonBox::Ok )->setEnabled( okButtonEnabled );
}

void ProjectPropertyImpl::on_m_specifiqueGroupBox_clicked() {
	updateOkButton();
}

void ProjectPropertyImpl::on_m_logButton_clicked() {
	m_logLineEdit->changePath( this, XINXConfig::self()->config().project.defaultPath );
}
