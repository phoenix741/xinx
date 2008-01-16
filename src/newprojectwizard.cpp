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
#include "newprojectwizard.h"
#include "globals.h"
#include "xinxconfig.h"

// Qt header
#include <QDir>

/* NewProjectWizard */

NewProjectWizard::NewProjectWizard( QWidget * widget ) : QWizard( widget ) {
	setButtonText( QWizard::CustomButton1, tr("&Expert ...") );
	setOption( QWizard::HaveCustomButton1, true );
	
    setPage( Page_Projet, new ProjectPageImpl );
    setPage( Page_Specifique, new SpecifiquePageImpl );
    setPage( Page_Services, new ServicesPageImpl );
    setPage( Page_ServicesList, new ServicesListPageImpl );
    setPage( Page_Versions, new VersionsPageImpl );

    setStartId( Page_Projet );

#ifndef Q_WS_MAC
	setWizardStyle( ModernStyle );
#endif
	setPixmap( QWizard::LogoPixmap, QPixmap(":/images/splash.png").scaled( QSize( 48, 48 ) ) );
	
	button( QWizard::CancelButton )->setIcon( QPixmap( ":/images/button_cancel.png" ) );
	button( QWizard::BackButton )->setIcon( QPixmap( ":/images/bookmarkprevious.png" ) );
	button( QWizard::NextButton )->setIcon( QPixmap( ":/images/bookmarknext.png" ) );
	button( QWizard::FinishButton )->setIcon( QPixmap( ":/images/button_apply.png" ) );
	button( QWizard::CommitButton )->setIcon( QPixmap( ":/images/button_apply.png" ) );

	setWindowTitle( tr("New Project Wizard") );
}

/* ProjectPageImpl */

ProjectPageImpl::ProjectPageImpl( QWidget * parent ) : QWizardPage( parent ) {
	setupUi( this );
	setTitle( windowTitle() );

	registerField( "project.name*",    m_projectNameEdit );
    registerField( "project.as*",      m_ASPathEdit );
    registerField( "project.log*",     m_logPathEdit );
    registerField( "project.project*", m_projectPathEdit );
    
    connect( m_projectNameEdit, SIGNAL(textChanged(QString)), this, SLOT(changeProjectName(QString)) );
    connect( m_ASPathEdit, SIGNAL(textChanged(QString)), this, SLOT(changeASPath(QString)) );

	m_ASPathEdit->setText( global.m_config->config().project.defaultPath );
}

int ProjectPageImpl::nextId() const {
	if( m_derivatedRadio->isChecked() )
		return NewProjectWizard::Page_Specifique;
	else
		return NewProjectWizard::Page_Services;
}

void ProjectPageImpl::on_m_ASPathBtn_clicked() {
	m_ASPathEdit->changePath( this, global.m_config->config().project.defaultPath );
}

void ProjectPageImpl::on_m_logPathBtn_clicked() {
	m_logPathEdit->changePath( this, global.m_config->config().project.defaultPath );
}

void ProjectPageImpl::on_m_projectPathBtn_clicked() {
	m_projectPathEdit->changePath( this, global.m_config->config().project.defaultPath );
}

void ProjectPageImpl::changeProjectName( const QString & text ) {
	m_projectPathEdit->setText( 
			QDir( m_ASPathEdit->text() ).absoluteFilePath( 
					QString( "j2ee/home/applications/%1/btoe" ).arg( text ) 
			) 
	);
}

void ProjectPageImpl::changeASPath( const QString & text ) {
	m_projectPathEdit->setText( 
			QDir( text ).absoluteFilePath( 
					QString( "j2ee/home/applications/%1/btoe" ).arg( m_projectNameEdit->text() ) 
			) 
	);
	m_logPathEdit->setText( 
			QDir( text ).absoluteFilePath( "j2ee/home/applications/log" ) 
	);
}


/* SpecifiquePageImpl */

SpecifiquePageImpl::SpecifiquePageImpl( QWidget * parent ) : QWizardPage( parent ) {
	setupUi( this );
	setTitle( windowTitle() );
	setSubTitle( tr("Define if the project is specifique or not. A specifique (derivated) project "
					"is used when original's file can't be modified.") );

    registerField( "specifique.prefix*",    m_prefixEdit );
    registerField( "specifique.path*",      m_specifiquePathNameEdit );
}

int SpecifiquePageImpl::nextId() const {
	return NewProjectWizard::Page_Services;
}

/* ServicesPageImpl */

ServicesPageImpl::ServicesPageImpl( QWidget * parent ) : QWizardPage( parent ) {
	setupUi( this );
	setTitle( windowTitle() );
	setSubTitle( tr("Define if the project contains WebServices. WebServices can be used to "
					"query database.") );

}

int ServicesPageImpl::nextId() const {
	if( m_addWebServicesButton->isChecked() )
		return NewProjectWizard::Page_ServicesList; 	
	else
		return NewProjectWizard::Page_Versions;
}

/* ServicesListPageImpl */

ServicesListPageImpl::ServicesListPageImpl( QWidget * parent ) : QWizardPage( parent ) {
	setupUi( this );
	setTitle( windowTitle() );
	setSubTitle( tr("Define the list of WSDL. WSDL is used to describe the web services. This"
					"list contains link to WSDL.") );
}

int ServicesListPageImpl::nextId() const {
	return NewProjectWizard::Page_Versions; 
}

/* VersionsPageImpl */

VersionsPageImpl::VersionsPageImpl( QWidget * parent ) : QWizardPage( parent ) {
	setTitle( tr("Define Revision Control") );
	setSubTitle( tr("Define the revision control used for the project if you want to used one.") );
	
	m_noRevisionControl = new QRadioButton( tr("No revision control system.") );
	m_noRevisionControl->setChecked( true );
	
	m_revisionBtn.append( new QRadioButton( tr("CVS Revision Control System") ) );
	m_revisionBtn.append( new QRadioButton( tr("SubVersion Revision Control System") ) );
	
	QVBoxLayout * layout = new QVBoxLayout;
	layout->addWidget( m_noRevisionControl );
	foreach( QRadioButton * button, m_revisionBtn ) {
		layout->addWidget( button );
	}
	setLayout( layout );
}

int VersionsPageImpl::nextId() const {
	return -1;	
}
