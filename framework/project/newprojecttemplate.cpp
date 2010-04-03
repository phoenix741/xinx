/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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
#include "newprojecttemplate.h"

// Qt header
#include <QFile>
#include <QDomDocument>
#include <QDomElement>

/* NewProjectTemplateException */

NewProjectTemplateException::NewProjectTemplateException(const QString & message) : XinxException(message)
{
}

/* NewProjectTemplate */

NewProjectTemplate::NewProjectTemplate(const QString & filename)
{
	QFile newProjectTemplateFile(filename);
	if (! newProjectTemplateFile.open(QFile::ReadOnly))
		throw NewProjectTemplateException(newProjectTemplateFile.errorString());

	QString errorString;
	QDomDocument document;
	if (! document.setContent(&newProjectTemplateFile, &errorString))
		throw NewProjectTemplateException(errorString);

	QDomElement rootNode = document.documentElement();
	if (rootNode.nodeName() != "ProjectTemplate")
		throw NewProjectTemplateException(tr("The file %1 is not a New Project Template file.").arg(filename));

	m_projectName = rootNode.attribute("name");

	QDomElement wizardPage = rootNode.firstChildElement("WizardPage");
	QDomElement page = wizardPage.firstChildElement("Page");
	while (! page.isNull())
	{
		m_wizardPages.append(page.attribute("name"));
		page = page.nextSiblingElement("Page");
	}

	QDomElement properties = rootNode.firstChildElement("Properties");
	QDomElement property = properties.firstChildElement("Property");
	while (! property.isNull())
	{
		m_properties.append(qMakePair(property.attribute("name"), property.attribute("value")));
		property = property.nextSiblingElement("Property");
	}


	QDomElement plugins = rootNode.firstChildElement("Plugins");
	QDomElement plugin  = plugins.firstChildElement("Plugin");
	while (! plugin.isNull())
	{
		m_plugins.append(plugin.attribute("name"));
		plugin = plugin.nextSiblingElement("Plugin");
	}
}

const QString & NewProjectTemplate::projectName() const
{
	return m_projectName;
}

const QStringList & NewProjectTemplate::wizardPages() const
{
	return m_wizardPages;
}

const QList<NewProjectTemplate::Property> & NewProjectTemplate::properties() const
{
	return m_properties;
}

const QStringList & NewProjectTemplate::plugins() const
{
	return m_plugins;
}
