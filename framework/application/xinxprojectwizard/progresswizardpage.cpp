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
#include "progresswizardpage.h"
#include "projectconverter.h"
#include "projectwizard.h"

// Qt header
#include <QApplication>
#include <QVBoxLayout>
#include <QProgressBar>

/* ProgressWizardPage */

ProgressWizardPage::ProgressWizardPage(QWidget * parent) : QWizardPage(parent)
{
	setTitle(tr("Progress of the conversion"));
	setSubTitle(tr("Please wait ..."));

	QVBoxLayout * layout = new QVBoxLayout(this);
	m_progressBar = new QProgressBar(this);
	layout->addWidget(m_progressBar);
}

void ProgressWizardPage::initializePage()
{
	if (dynamic_cast<ProjectWizard*>(wizard())->converter())
	{
		ProjectConverter * converter = dynamic_cast<ProjectWizard*>(wizard())->converter();
		connect(converter, SIGNAL(setValue(int)), m_progressBar, SLOT(setValue(int)));
		connect(converter, SIGNAL(setValue(int)), this, SLOT(processMessages()));
		connect(converter, SIGNAL(setMaximum(int)), m_progressBar, SLOT(setMaximum(int)));
		converter->process();
		m_progressBar->setMaximum(converter->nbSession() + XINX_PROJECT_VERSION - converter->version());
	}
}

void ProgressWizardPage::processMessages()
{
	qApp->processEvents();
}

