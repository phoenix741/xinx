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

/* FileWizardPage */

FileWizardPage::FileWizardPage(QString filename, QWidget * parent) : QWizardPage(parent), m_filename(filename)
{
	setTitle(tr("Project file selection"));
	setSubTitle(tr("This wizard will help you to migrate your project file to "
	               "the current version of XINX. Please fill all fields."));

	QLabel * directoryLabel;
	QVBoxLayout * layout = new QVBoxLayout(this);
	layout->addWidget(directoryLabel = new QLabel(tr("&Project file : "), this));
	layout->addWidget(m_projectEdit  = new DirectoryEditWidget(false, this));
	directoryLabel->setBuddy(m_projectEdit->lineEdit());

	registerField("project.name*", m_projectEdit->lineEdit());
}

void FileWizardPage::initializePage()
{
	m_projectEdit->lineEdit()->setText(m_filename);
}

bool FileWizardPage::validatePage()
{
	try
	{
		ProjectConverter * converter = dynamic_cast<ProjectWizard*>(wizard())->converter();
		delete converter;
		converter = new ProjectConverter(field("project.name").toString());
		dynamic_cast<ProjectWizard*>(wizard())->setConverter(converter);
	}
	catch (XinxException & e)
	{
		QMessageBox::critical(this, tr("Project Wizard"), e.getMessage());
		return false;
	}
	return true;
}

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
