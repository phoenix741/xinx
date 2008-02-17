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

// Qt header
#include <QAbstractButton>

ProjectWizard::ProjectWizard( QWidget * parent ) : QWizard( parent ) {
	addPage( new FileWizardPage );
	addPage( new VersionWizardPage );
	addPage( new ProgressWizardPage );
	addPage( new ConclusionWizardPage );
	
	setPixmap( QWizard::LogoPixmap, QPixmap(":/images/splash.png").scaled( QSize( 48, 48 ) ) );
	 
	button( QWizard::CancelButton )->setIcon( QPixmap( ":/images/button_cancel.png" ) );
	button( QWizard::BackButton )->setIcon( QPixmap( ":/images/bookmarkprevious.png" ) );
	button( QWizard::NextButton )->setIcon( QPixmap( ":/images/bookmarknext.png" ) );
	button( QWizard::FinishButton )->setIcon( QPixmap( ":/images/button_apply.png" ) );
	button( QWizard::CommitButton )->setIcon( QPixmap( ":/images/button_ok.png" ) );

	setWindowTitle( tr( "Project wizard" ) );
}

FileWizardPage::FileWizardPage( QWidget * parent ) {
	
}

VersionWizardPage::VersionWizardPage( QWidget * parent ) {
	
}

ProgressWizardPage::ProgressWizardPage( QWidget * parent ) {
	
}

ConclusionWizardPage::ConclusionWizardPage( QWidget * parent ) {
	
}
