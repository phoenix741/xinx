/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Xinx header
#include "conclusionwizardpage.h"

// Qt header
#include <QVBoxLayout>
#include <QLabel>

/* ConclusionWizardPage */

ConclusionWizardPage::ConclusionWizardPage(QWidget * parent) : QWizardPage(parent)
{
	setTitle(tr("Conversion finished"));
	setSubTitle(tr("The conversion is terminated, you can now reopen the project."));

	QVBoxLayout * layout = new QVBoxLayout(this);
	layout->addWidget(new QLabel(tr("The project is now converted. XINX can now open the project file normally."), this));
	//  layout->addWidget( m_openCheck = new QCheckBox( tr("Re-open the project with XINX automatically"), this ) );

	//  m_openCheck->setChecked( true );

	//  registerField( "project.open", m_openCheck );
}