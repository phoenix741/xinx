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
#include "filewizardpage.h"
#include <directoryedit.h>
#include <directoryeditwidget.h>
#include "projectconverter.h"
#include "projectwizard.h"

// Qt header
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QVariant>

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

