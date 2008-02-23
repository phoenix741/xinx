/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke   *
 *   ulrich.vdh@free.fr   *
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
#include "projectwizard.h"
#include "directoryedit.h"
#include "projectconverter.h"

// Qt header
#include <QApplication>
#include <QAbstractButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>

/* ProjectWizard */

ProjectWizard::ProjectWizard( QWidget * parent ) : QWizard( parent ), m_converter( 0 ) {
	addPage( new FileWizardPage );
	addPage( new VersionWizardPage );
	addPage( new ProgressWizardPage );
	addPage( new ConclusionWizardPage );
	
	setPixmap( QWizard::LogoPixmap, QPixmap(":/images/splash.png").scaled( QSize( 48, 48 ) ) );
	setPixmap( QWizard::BannerPixmap, QPixmap(":/images/banner.png") );
	 
	button( QWizard::CancelButton )->setIcon( QPixmap( ":/images/button_cancel.png" ) );
	button( QWizard::BackButton )->setIcon( QPixmap( ":/images/bookmarkprevious.png" ) );
	button( QWizard::NextButton )->setIcon( QPixmap( ":/images/bookmarknext.png" ) );
	button( QWizard::FinishButton )->setIcon( QPixmap( ":/images/button_apply.png" ) );
	button( QWizard::CommitButton )->setIcon( QPixmap( ":/images/button_ok.png" ) );

	setWindowTitle( tr( "Project wizard" ) );
}

ProjectConverter * ProjectWizard::converter() const {
	return m_converter;
}

void ProjectWizard::setConverter( ProjectConverter * c ) {
	m_converter  = c;
}

/* FileWizardPage */

FileWizardPage::FileWizardPage( QWidget * parent ) : QWizardPage( parent ) {
	setTitle( tr("Project file selection") );
	setSubTitle( tr("This wizard will help you to migrate your project file to "
					"the current version of XINX. Please fill all fields") );
	
	QLabel * directoryLabel;
	QVBoxLayout * layout = new QVBoxLayout( this );
	layout->addWidget( directoryLabel = new QLabel( tr("&Project file : "), this ) );
	layout->addWidget( m_projectEdit  = new DirectoryEditWidget( false, this ) );
	directoryLabel->setBuddy( m_projectEdit->lineEdit() );
	
	registerField( "project.name*", m_projectEdit->lineEdit() );
}

void FileWizardPage::initializePage() {
	if( qApp->arguments().count() > 1 )
		m_projectEdit->lineEdit()->setText( qApp->arguments().at( 1 ) );
}

bool FileWizardPage::validatePage() {
	try {
		ProjectConverter * converter = dynamic_cast<ProjectWizard*>( wizard() )->converter();
		delete converter;
		converter = new ProjectConverter( field( "project.name" ).toString() );
		dynamic_cast<ProjectWizard*>( wizard() )->setConverter( converter );
	} catch( XinxException & e ) {
		QMessageBox::critical( this, tr("Project Wizard"), e.getMessage() );
		return false;
	}
	return true;
}

/* VersionWizardPage */

VersionWizardPage::VersionWizardPage( QWidget * parent ) : QWizardPage( parent ) {
	setTitle( tr("Version informations") );
	setSubTitle( tr("This page show you some informations about the selected project file") );
	setCommitPage( true );

	QVBoxLayout * layout = new QVBoxLayout( this );
	m_resume = new QLabel( this );
	
	layout->addWidget( m_resume );
}

void VersionWizardPage::initializePage() {
	if( dynamic_cast<ProjectWizard*>( wizard() )->converter() ) {
		m_resume->setText( 
				tr("The selected project to convert is a %1 at the version %2.\nThis wizard will convert the project to the version %3. There is %4 opened file to convert")
					.arg( dynamic_cast<ProjectWizard*>( wizard() )->converter()->type() )
					.arg( dynamic_cast<ProjectWizard*>( wizard() )->converter()->version() )
					.arg( XINX_PROJECT_VERSION )
					.arg( dynamic_cast<ProjectWizard*>( wizard() )->converter()->nbSession() )
		);
		
	}
}

/* ProgressWizardPage */

ProgressWizardPage::ProgressWizardPage( QWidget * parent ) : QWizardPage( parent ) {
	setTitle( tr("Progress of the conversion") );
	setSubTitle( tr("Please waite ...") );
	
	QVBoxLayout * layout = new QVBoxLayout( this );
	m_progressBar = new QProgressBar( this );
	layout->addWidget( m_progressBar );
}

void ProgressWizardPage::initializePage() {
	m_progressBar->setValue(50);
}

/* ConclusionWizardPage */

ConclusionWizardPage::ConclusionWizardPage( QWidget * parent ) : QWizardPage( parent ) {
	setTitle( tr("Conversion finished") );
	setSubTitle( tr("The conversion is terminated, you can now reopen the project.") );
	
	QVBoxLayout * layout = new QVBoxLayout( this );
	layout->addWidget( new QLabel( tr("The project is now converted. XINX can now open the project file normally."), this ) );	
	layout->addWidget( m_openCheck = new QCheckBox( tr("Re-open the project with XINX automatically"), this ) );

	registerField( "project.open", m_openCheck );
}
