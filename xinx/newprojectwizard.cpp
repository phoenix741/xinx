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
#include "newprojectwizard.h"
#include <xinxconfig.h>
#include "xslproject.h"
#include "projectpropertyimpl.h"
#include <xinxpluginsloader.h>

// Qt header
#include <QDir>
#include <QRegExpValidator>
#include <QInputDialog>

/* NewProjectWizard */

NewProjectWizard::NewProjectWizard( QWidget * widget, Qt::WFlags f ) : QWizard( widget, f ), m_project( 0 ) {
	setButtonText( QWizard::CustomButton1, tr("&Expert ...") );
	connect( this, SIGNAL(customButtonClicked(int)), this, SLOT(on_customButton1_clicked()) );

	setPage( Page_Projet, new ProjectPageImpl );
    setPage( Page_Specifique, new SpecifiquePageImpl );
    setPage( Page_Versions, m_versions = new VersionsPageImpl );

	int pageid = NewProjectWizard::Page_Versions + 1;
	foreach( XinxPluginElement * element, XinxPluginsLoader::self()->plugins() ) {
		if( element->isActivated() && qobject_cast<IXinxPluginProjectConfiguration*>( element->plugin() ) ) {
			IXinxPluginProjectConfiguration* interface = qobject_cast<IXinxPluginProjectConfiguration*>( element->plugin() );
			QList<QWizardPage*> pages = interface->createNewProjectSettingsPages( pageid );
			foreach( QWizardPage* page, pages ) {
				m_wizardPages.append( qMakePair( interface, page ) );
				setPage( pageid++, page );
			}
		}
	}
	m_lastPage = pageid;
	setPage( m_lastPage, new LastPageImpl );

    setStartId( Page_Projet );

	setPixmap( QWizard::LogoPixmap, QPixmap(":/images/splash.png").scaled( QSize( 48, 48 ) ) );
	setPixmap( QWizard::BannerPixmap, QPixmap(":/images/banner_wizard.png") );

	button( QWizard::CancelButton )->setIcon( QPixmap( ":/images/button_cancel.png" ) );
	button( QWizard::BackButton )->setIcon( QPixmap( ":/images/bookmarkprevious.png" ) );
	button( QWizard::NextButton )->setIcon( QPixmap( ":/images/bookmarknext.png" ) );
	button( QWizard::FinishButton )->setIcon( QPixmap( ":/images/button_apply.png" ) );
	button( QWizard::CommitButton )->setIcon( QPixmap( ":/images/button_ok.png" ) );

	setWindowTitle( tr("New Project Wizard") );
}

XSLProject * NewProjectWizard::createProject() {
	if( ! m_project ) {
		m_project = new XSLProject();

		m_project->setProjectName( field( "project.name" ).toString() );
		m_project->setProjectPath( QDir::fromNativeSeparators( field( "project.path" ).toString() ) );
		m_project->setLogProjectDirectory( QDir::fromNativeSeparators( field( "project.log" ).toString() ) );

		XSLProject::ProjectOptions options;
		if( field( "project.derivated" ).toBool() ) {
			m_project->setSpecifiquePrefix( field( "specifique.prefix" ).toString() );
			m_project->setSpecifiquePathName( field( "specifique.path" ).toString() );
			options |= XSLProject::hasSpecifique;
		}
		m_project->setOptions( options );

		QString rcs = QString();
		if( ! m_versions->m_noRevisionControl->isChecked() ) {
			QPair<QRadioButton*,QString> btn;
			foreach( btn, m_versions->m_revisionBtn ) {
				if( btn.first->isChecked() ) {
					rcs = btn.second;
					break;
				}
			}
		}
		m_project->setProjectRCS( rcs );

		QPair<IXinxPluginProjectConfiguration*,QWizardPage*> page;
		foreach( page, m_wizardPages ) {
			if(! page.first->saveNewProjectSettingsPage( m_project, page.second )) qWarning( qPrintable( tr("Can't save \"%1\" page").arg( page.second->windowTitle() ) ) );
		}
	}
	return m_project;
}

void NewProjectWizard::on_customButton1_clicked() {
	hide();
	m_project = new XSLProject();
	ProjectPropertyImpl property ( this );
	property.loadFromProject( m_project ); // Load an empty project;
	if( ! property.exec() ) {
		delete m_project; m_project = NULL;
		reject();
		return;
	}
	property.saveToProject( m_project );
	accept();
}

/* ProjectPageImpl */

ProjectPageImpl::ProjectPageImpl( QWidget * parent ) : QWizardPage( parent ) {
	setupUi( this );
	setTitle( windowTitle() );

	registerField( "project.derivated", m_derivatedRadio );

	registerField( "project.name*",     m_projectNameEdit );
    registerField( "project.as",        m_ASPathEdit );
    registerField( "project.log*",      m_logPathEdit );
    registerField( "project.path*",     m_projectPathEdit );
}

void ProjectPageImpl::initializePage() {
	m_ASPathEdit->setText( QDir::toNativeSeparators( XINXConfig::self()->config().project.defaultPath ) );
}

int ProjectPageImpl::nextId() const {
	if( m_derivatedRadio->isChecked() )
		return NewProjectWizard::Page_Specifique;
	else
		return NewProjectWizard::Page_Versions;
}

void ProjectPageImpl::setVisible( bool visible ) {
	QWizardPage::setVisible( visible );

	if( visible ) {
		wizard()->setOption( QWizard::HaveCustomButton1, true );
	} else
		wizard()->setOption( QWizard::HaveCustomButton1, false );
}

bool ProjectPageImpl::isComplete () const {
	if( ! QWizardPage::isComplete() ) return false;
	if( ! QDir( QDir::fromNativeSeparators( m_projectPathEdit->text() ) ).exists() ) return false;
	return true;
}

void ProjectPageImpl::on_m_ASPathBtn_clicked() {
	m_ASPathEdit->changePath( this, XINXConfig::self()->config().project.defaultPath );
}

void ProjectPageImpl::on_m_logPathBtn_clicked() {
	m_logPathEdit->changePath( this, XINXConfig::self()->config().project.defaultPath );
}

void ProjectPageImpl::on_m_projectPathBtn_clicked() {
	m_projectPathEdit->changePath( this, XINXConfig::self()->config().project.defaultPath );
}

void ProjectPageImpl::on_m_projectNameEdit_textChanged( const QString & text ) {
	m_projectPathEdit->setText( QDir::toNativeSeparators(
			QDir( QDir::fromNativeSeparators( m_ASPathEdit->text() ) ).absoluteFilePath(
					QString( "j2ee/home/applications/%1" ).arg( text )
			) )
	);
}

void ProjectPageImpl::on_m_ASPathEdit_textChanged( const QString & text ) {
	m_projectPathEdit->setText( QDir::toNativeSeparators(
			QDir( QDir::fromNativeSeparators( text ) ).absoluteFilePath(
					QString( "j2ee/home/applications/%1" ).arg( m_projectNameEdit->text() )
			) )
	);
	m_logPathEdit->setText( QDir::toNativeSeparators(
			QDir( QDir::fromNativeSeparators( text ) ).absoluteFilePath( "j2ee/home/log" )
	) );
}

/* SpecifiquePageImpl */

SpecifiquePageImpl::SpecifiquePageImpl( QWidget * parent ) : QWizardPage( parent ) {
	setupUi( this );
	setTitle( windowTitle() );
	setSubTitle( tr("Define if the project is specifique or not. A specifique (derivated) project "
					"is used when original's file can't be modified.") );

	m_specifiquePathNameEdit->setValidator( new QRegExpValidator( QRegExp( "[\\w]*" ), m_specifiquePathNameEdit ) );

    registerField( "specifique.prefix*",    m_prefixEdit );
    registerField( "specifique.path*",      m_specifiquePathNameEdit );
}

void SpecifiquePageImpl::initializePage() {
	m_prefixEdit->setText( QString("P%1").arg( field("project.name").toString().left( 2 ) ).toUpper() );
	m_specifiquePathNameEdit->setText( XINXConfig::self()->config().project.defaultProjectPathName );
}

int SpecifiquePageImpl::nextId() const {
	return NewProjectWizard::Page_Versions;
}

/* VersionsPageImpl */

VersionsPageImpl::VersionsPageImpl( QWidget * parent ) : QWizardPage( parent ) {
	setTitle( tr("Define Revision Control") );
	setSubTitle( tr("Define the revision control used for the project if you want to used one.") );

	m_noRevisionControl = new QRadioButton( tr("No revision control system."), this );
	m_noRevisionControl->setChecked( true );

	QPair<QString,QString> revisionControl;
	foreach( revisionControl, XinxPluginsLoader::self()->revisionsControls() ) {
		m_revisionBtn.append( qMakePair( new QRadioButton( revisionControl.second, this ), revisionControl.first) );
	}

	QVBoxLayout * layout = new QVBoxLayout;
	layout->addWidget( m_noRevisionControl );
	QPair<QRadioButton*,QString> button;
	foreach( button, m_revisionBtn ) {
		layout->addWidget( button.first );
	}
	setLayout( layout );

}

int VersionsPageImpl::nextId() const {
	return NewProjectWizard::Page_Versions + 1;
}

/* LastPageImpl */

LastPageImpl::LastPageImpl( QWidget * parent ) : QWizardPage( parent ) {
	setTitle( tr("Project created") );
	setSubTitle( tr("The project is now created.") );

	QLabel * label = new QLabel( tr("The project is now created, please enjoy."), this );

	QVBoxLayout * layout = new QVBoxLayout;
	layout->addWidget( label );
	setLayout( layout );

}

int LastPageImpl::nextId() const {
	return -1;
}
