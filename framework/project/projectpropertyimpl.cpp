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
#include "projectpropertyimpl.h"
#include <project/xinxprojectproject.h>
#include <core/xinxconfig.h>
#include <core/exceptions.h>
#include <plugins/xinxpluginsloader.h>
#include <plugins/interfaces/gui.h>
#include <plugins/interfaces/files.h>
#include <contentview3/cache.h>
#include <plugins/xinxpluginelement.h>
#include <versioncontrol/versioncontrolmanager.h>
#include <editors/filetypepool.h>

// Qt header
#include <QDir>
#include <QInputDialog>
#include <QPushButton>
#include <QTimer>

/* ProjectPropertyImpl */

ProjectPropertyImpl::ProjectPropertyImpl(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(this);

	m_buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);

	QTreeWidgetItem * generalElement = new QTreeWidgetItem(m_tree, QStringList() << tr("General"));
	generalElement->setIcon(0, QIcon(":/images/preferences-general.png"));

	/* Project Definition */

	QTreeWidgetItem * projectDefinition = new QTreeWidgetItem(generalElement, QStringList() << tr("Project Definition"));
	generalElement->setData(0, Qt::UserRole, QVariant::fromValue(m_projectDefinitionPage));

	m_projectRCSComboBox->addItem(tr("<No Revision Control System>"));
	QMapIterator<QString,QString> revision_control_iterator(VersionControl::Manager::self()->revisionControlIds());
	while (revision_control_iterator.hasNext())
	{
		revision_control_iterator.next();

		m_projectRCSComboBox->addItem(revision_control_iterator.value(), revision_control_iterator.key());
	}

	projectDefinition->setData(0, Qt::UserRole, QVariant::fromValue(m_projectDefinitionPage));

	/* Preloaded Page */

	QTreeWidgetItem * preloadedFiles = new QTreeWidgetItem(generalElement, QStringList() << tr("Preloaded files"));
	preloadedFiles->setData(0, Qt::UserRole, QVariant::fromValue(m_preloadedFilesPage));

	/* Directories path */

	QTreeWidgetItem * directoryPath = new QTreeWidgetItem(generalElement, QStringList() << tr("Directories path"));
	directoryPath->setData(0, Qt::UserRole, QVariant::fromValue(m_directoryPage));

	/* Plugin */

	foreach(XinxPluginElement * element, XinxPluginsLoader::self()->plugins())
	{
		if (element->isActivated() && qobject_cast<IXinxPluginProjectConfiguration*>(element->plugin()))
		{
			IXinxPluginProjectConfiguration * interface = qobject_cast<IXinxPluginProjectConfiguration*>(element->plugin());
			QList<IXinxPluginProjectConfigurationPage*> pages = interface->createProjectSettingsPage(this);
			QTreeWidgetItem * rootElement = new QTreeWidgetItem(m_tree, QStringList() << element->name());
			rootElement->setIcon(0, element->pixmap());
			if (element->isModifiable())
			{
				m_pluginsCheck.insert(element->objectName(), rootElement);
			}

			foreach(IXinxPluginProjectConfigurationPage * page, pages)
			{
				QTreeWidgetItem * pageElement = new QTreeWidgetItem(rootElement, QStringList() << page->name());
				pageElement->setData(0, Qt::UserRole, QVariant::fromValue(page->settingsDialog()));

				m_pluginPages.append(page);
				m_pages->addWidget(page->settingsDialog());
			}
		}
	}

	/* Update button */

	QTimer * updateOkTimer = new QTimer(this);
	updateOkTimer->setInterval(250);
	updateOkTimer->setSingleShot(false);
	connect(updateOkTimer, SIGNAL(timeout()), SLOT(updateOkButton()));

	updateOkTimer->start();

	/* Final Tree */

	m_tree->expandAll();
}

ProjectPropertyImpl::~ProjectPropertyImpl()
{

}

void ProjectPropertyImpl::loadFromProject(XinxProject::ProjectPtr project)
{
	m_nameLineEdit->setText(project->projectName());
	_projectLineEdit->setText(QDir::toNativeSeparators(project->projectPath()));
	linkedDirectoryList->setDefaultProposedValue(project->projectPath());

	int index = m_projectRCSComboBox->findData(project->projectRCS());
	if (index < 0) index = 0;
	m_projectRCSComboBox->setCurrentIndex(index);

	foreach(QString filename, project->cache()->cachedFiles())
	{
		QList<IFileTypePluginPtr> fileTypes = XinxPluginsLoader::self()->fileTypePool()->matchedFileType(filename);
		QString fn = QDir(project->projectPath()).relativeFilePath(filename);
		if (fileTypes.size())
			new QListWidgetItem(QIcon(fileTypes.at(0)->icon()), fn, m_preloadedFiles);
		else
			new QListWidgetItem(fn, m_preloadedFiles);
	}

	QStringList directories;
	foreach(QString directory, project->linkedPath())
	{
		directories.append(QDir::toNativeSeparators(directory));
	}
	linkedDirectoryList->setValues(directories);
	excludeRegexpPath->setValues(project->excludedPath());

	QStringList activatedPlugin = project->activatedPlugin();
	foreach(const QString & plugin, m_pluginsCheck.keys())
	{
		if (activatedPlugin.contains(plugin))
			m_pluginsCheck.value(plugin)->setCheckState(0, Qt::Checked);
		else
			m_pluginsCheck.value(plugin)->setCheckState(0, Qt::Unchecked);
	}

	foreach(IXinxPluginProjectConfigurationPage * page, m_pluginPages)
	{
		page->setProject(project);
		if (! page->loadSettingsDialog())
			qWarning(qPrintable(tr("Can't load \"%1\" page").arg(page->name())));
	}
}

void ProjectPropertyImpl::saveToProject(XinxProject::ProjectPtr project)
{
	project->setProjectName(m_nameLineEdit->text());
	project->setProjectRCS(m_projectRCSComboBox->itemData(m_projectRCSComboBox->currentIndex()).toString());

	foreach(IXinxPluginProjectConfigurationPage * page, m_pluginPages)
	{
		page->setProject(project);
		if (! page->saveSettingsDialog())
			qWarning(qPrintable(tr("Can't save \"%1\" page").arg(page->name())));
	}

	QStringList directories;
	foreach(QString directory, linkedDirectoryList->values())
	{
		directories.append(QDir::fromNativeSeparators(directory));
	}
	project->setLinkedPath(directories);
	project->setExcludedPath(excludeRegexpPath->values());

	QStringList activatedPlugin;
	foreach(const QString & plugin, m_pluginsCheck.keys())
	{
		if (m_pluginsCheck.value(plugin)->checkState(0) == Qt::Checked)
			activatedPlugin << plugin;
	}
	project->setActivatedPlugin(activatedPlugin);
}

void ProjectPropertyImpl::updateOkButton()
{
	foreach(IXinxPluginProjectConfigurationPage * page, m_pluginPages)
	{
		QString message;
		if (! page->isSettingsValid(message))
		{
			m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
			return;
		}
	}

	m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}


void ProjectPropertyImpl::on_m_tree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	Q_UNUSED(previous);

	QWidget * widget = current->data(0, Qt::UserRole).value<QWidget*>();
	if (widget)
		m_pages->setCurrentWidget(widget);

}
