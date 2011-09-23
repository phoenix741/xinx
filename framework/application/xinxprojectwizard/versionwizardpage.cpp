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
#include "versionwizardpage.h"
#include "projectwizard.h"
#include "projectconverter.h"

// Qt header
#include <QVBoxLayout>
#include <QLabel>

/* VersionWizardPage */

VersionWizardPage::VersionWizardPage(QWidget * parent) : QWizardPage(parent)
{
	setTitle(tr("Version informations"));
	setSubTitle(tr("This page show you some informations about the selected project file."));
	setCommitPage(true);

	QVBoxLayout * layout = new QVBoxLayout(this);
	m_resume = new QLabel(this);
	m_resume->setWordWrap(true);

	layout->addWidget(m_resume);
}

void VersionWizardPage::initializePage()
{
	if (dynamic_cast<ProjectWizard*>(wizard())->converter())
	{
		m_resume->setText(
		tr("You want convert a %1 (version %2).\nThis wizard will convert the project to the last version of XINX. Wizard must convert %3 opened file.")
		.arg(dynamic_cast<ProjectWizard*>(wizard())->converter()->type())
		.arg(dynamic_cast<ProjectWizard*>(wizard())->converter()->version())
		.arg(dynamic_cast<ProjectWizard*>(wizard())->converter()->nbSession())
		);

	}
}
