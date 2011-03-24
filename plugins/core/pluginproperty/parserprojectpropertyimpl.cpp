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
#include "parserprojectpropertyimpl.h"
#include <project/xinxprojectproject.h>
#include <core/xinxconfig.h>
#include <directoryedit.h>

// Qt header
#include <QGridLayout>

/* ParserProjectPropertyImpl */

ParserProjectPropertyImpl::ParserProjectPropertyImpl(QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	setupUi(this);
}

ParserProjectPropertyImpl::~ParserProjectPropertyImpl()
{
}

void ParserProjectPropertyImpl::setProject(XinxProject::ProjectPtr project)
{
	m_project = project;
}

QPixmap ParserProjectPropertyImpl::image()
{
	return QPixmap();
}

QString ParserProjectPropertyImpl::name()
{
	return this->windowTitle();
}

bool ParserProjectPropertyImpl::loadSettingsDialog()
{
	const QString moduleAddress = m_project->readProperty("moduleInternetAdresse").toString();

	if (QFileInfo(moduleAddress).isRelative())
		m_urlLocationEdit->lineEdit()->setText(QDir(m_project->projectPath()).absoluteFilePath(moduleAddress));
	else
		m_urlLocationEdit->lineEdit()->setText(moduleAddress);

	return true;
}

bool ParserProjectPropertyImpl::saveSettingsDialog()
{
	m_project->writeProperty("moduleInternetAdresse", QDir(m_project->projectPath()).relativeFilePath(m_urlLocationEdit->lineEdit()->text()));
	return true;
}

bool ParserProjectPropertyImpl::cancelSettingsDialog()
{
	return true;
}

QWidget * ParserProjectPropertyImpl::settingsDialog()
{
	return this;
}

bool ParserProjectPropertyImpl::isSettingsValid(QString& message)
{
	return true;
}

bool ParserProjectPropertyImpl::isVisible()
{
	return true;
}

/* WebPluginProjectPropertyWizard */

WebPluginProjectPropertyWizard::WebPluginProjectPropertyWizard()
{
	setTitle(tr("Web Module Property Page"));
	setSubTitle(tr("Define information about the web module, like the directory where find JS, Image and CSS."));


	QFormLayout * formLayout = new QFormLayout(this);
	formLayout->addRow(tr("Directory used to find CSS, JS and Image files :"), m_urlLocationEdit = new QLineEdit(this));

	m_urlLocationEdit->setText(field("project.path").toString());

	registerField("webplugin.adresse", m_urlLocationEdit);
}

void WebPluginProjectPropertyWizard::initializePage()
{

}

QString WebPluginProjectPropertyWizard::pagePluginId() const
{
	return "CoreInformation";
}

bool WebPluginProjectPropertyWizard::pageIsVisible() const
{
	return true;
}

bool WebPluginProjectPropertyWizard::saveSettingsDialog(XinxProject::ProjectPtr project)
{
	project->writeProperty("moduleInternetAdresse", m_urlLocationEdit->text());

	return true;
}
