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
#include "projectpropertyimpl.h"
#include <project/xinxproject.h>
#include <core/xinxconfig.h>
#include <core/exceptions.h>
#include <plugins/xinxpluginsloader.h>
#include <contentview2/contentview2cache.h>
#include <contentview2/contentview2manager.h>

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
	QPair<QString,QString> revisionControl;
	foreach(revisionControl, XinxPluginsLoader::self()->revisionsControls())
	m_projectRCSComboBox->addItem(revisionControl.second, revisionControl.first);

	projectDefinition->setData(0, Qt::UserRole, QVariant::fromValue(m_projectDefinitionPage));

	m_projectLineEdit->setDefaultValue(XINXConfig::self()->config().project.defaultPath);


	/* Preloaded Page */

	QTreeWidgetItem * preloadedFiles = new QTreeWidgetItem(generalElement, QStringList() << tr("Preloaded files"));
	preloadedFiles->setData(0, Qt::UserRole, QVariant::fromValue(m_preloadedFilesPage));

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

void ProjectPropertyImpl::loadFromProject(XinxProject * project)
{
	m_nameLineEdit->setText(project->projectName());
	m_projectLineEdit->lineEdit()->setText(QDir::toNativeSeparators(project->projectPath()));

	int index = m_projectRCSComboBox->findData(project->projectRCS());
	if (index < 0) index = 0;
	m_projectRCSComboBox->setCurrentIndex(index);

	foreach(QString filename, ContentView2::Manager::self()->cache()->contentsViewLoaded())
	{
		IFileTypePlugin * fileType = XinxPluginsLoader::self()->matchedFileType(filename);
		QString fn = QDir(project->projectPath()).relativeFilePath(filename);
		if (fileType)
			new QListWidgetItem(QIcon(fileType->icon()), fn, m_preloadedFiles);
		else
			new QListWidgetItem(fn, m_preloadedFiles);
	}

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

void ProjectPropertyImpl::saveToProject(XinxProject * project)
{
	project->setProjectName(m_nameLineEdit->text());
	project->setProjectPath(QDir::fromNativeSeparators(m_projectLineEdit->lineEdit()->text()));

	project->setProjectRCS(m_projectRCSComboBox->itemData(m_projectRCSComboBox->currentIndex()).toString());

	foreach(IXinxPluginProjectConfigurationPage * page, m_pluginPages)
	{
		page->setProject(project);
		if (! page->saveSettingsDialog())
			qWarning(qPrintable(tr("Can't save \"%1\" page").arg(page->name())));
	}

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
	bool projectLineOk = !(m_projectLineEdit->lineEdit()->text().isEmpty() || !QDir(m_projectLineEdit->lineEdit()->text()).exists());
	if (! projectLineOk)
	{
		m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		return;
	}

	foreach(IXinxPluginProjectConfigurationPage * page, m_pluginPages)
	{
		if (! page->isSettingsValid())
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
