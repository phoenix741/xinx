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
#include "projectwizard.h"
#include <directoryeditwidget.h>
#include <directoryedit.h>
#include "projectconverter.h"
#include "filewizardpage.h"
#include "versionwizardpage.h"
#include "progresswizardpage.h"
#include "conclusionwizardpage.h"

// Qt header
#include <QApplication>
#include <QAbstractButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include <QVariant>

/* ProjectWizard */

ProjectWizard::ProjectWizard(QString filename, QWidget * parent) : QWizard(parent), m_converter(0), m_filename(filename)
{
	addPage(new FileWizardPage(filename));
	addPage(new VersionWizardPage);
	addPage(new ProgressWizardPage);
	addPage(new ConclusionWizardPage);

	setPixmap(QWizard::LogoPixmap, QPixmap(":/images/splash.png").scaled(QSize(48, 48)));
	setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner_wizard.png"));

	button(QWizard::CancelButton)->setIcon(QPixmap(":/images/button_cancel.png"));
	button(QWizard::BackButton)->setIcon(QPixmap(":/images/bookmarkprevious.png"));
	button(QWizard::NextButton)->setIcon(QPixmap(":/images/bookmarknext.png"));
	button(QWizard::FinishButton)->setIcon(QPixmap(":/images/button_apply.png"));
	button(QWizard::CommitButton)->setIcon(QPixmap(":/images/button_ok.png"));

	setWindowTitle(tr("Project wizard"));
}

ProjectConverter * ProjectWizard::converter() const
{
	return m_converter;
}

void ProjectWizard::setConverter(ProjectConverter * c)
{
	m_converter  = c;
}

void ProjectWizard::accept()
{
	if (m_converter)
		m_converter->save();

	QWizard::accept();
}

