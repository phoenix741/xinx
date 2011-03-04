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
#include "newprojectwizard.h"
#include <core/xinxconfig.h>
#include <project/xinxprojectproject.h>
#include <plugins/xinxpluginsloader.h>
#include "projectpropertyimpl.h"
#include "templatedialogimpl.h"
#include <project/newprojecttemplate.h>
#include <project/xinxprojectproject.h>
#include <plugins/xinxpluginelement.h>

// Qt header
#include <QDir>
#include <QRegExpValidator>
#include <QInputDialog>

/* NewProjectWizard */
#include <versioncontrol/versioncontrolmanager.h>

NewProjectWizard::NewProjectWizard(const QString & filename, QWidget * widget, Qt::WFlags f) : QWizard(widget, f)
{
	m_templateDialog = new TemplateDialogImpl;
	m_projectPage = new ProjectPageImpl(filename);
	IXinxPluginNewProjectConfigurationPage * versionPage = new VersionsPageImpl;
	m_lastDialog = new LastPageImpl;

	m_wizardPages.insert(m_templateDialog->pagePluginId(), m_templateDialog);
	m_wizardPages.insert(m_projectPage->pagePluginId(),    m_projectPage);
	m_wizardPages.insert(versionPage->pagePluginId(),      versionPage);
	m_wizardPages.insert(m_lastDialog->pagePluginId(),     m_lastDialog);

	foreach(XinxPluginElement * element, XinxPluginsLoader::self()->plugins())
	{
		if (element->isActivated() && qobject_cast<IXinxPluginProjectConfiguration*>(element->plugin()))
		{
			IXinxPluginProjectConfiguration* interface = qobject_cast<IXinxPluginProjectConfiguration*>(element->plugin());

			QList<IXinxPluginNewProjectConfigurationPage*> pages = interface->createNewProjectSettingsPages();
			foreach(IXinxPluginNewProjectConfigurationPage* page, pages)
			{
				m_wizardPages.insert(page->pagePluginId(), page);
			}
		}
	}

	foreach(IXinxPluginNewProjectConfigurationPage * page, m_wizardPages.values())
	{
		setPage(qHash(page->pagePluginId()), page);
	}


	setStartId(qHash(m_templateDialog->pagePluginId()));

	m_templateDialog->setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/newprojectwatermark.png"));
	setPixmap(QWizard::LogoPixmap, QPixmap(":/images/splash.png").scaled(QSize(48, 48)));
	setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner_wizard.png"));

	button(QWizard::CancelButton)->setIcon(QPixmap(":/images/button_cancel.png"));
	button(QWizard::BackButton)->setIcon(QPixmap(":/images/bookmarkprevious.png"));
	button(QWizard::NextButton)->setIcon(QPixmap(":/images/bookmarknext.png"));
	button(QWizard::FinishButton)->setIcon(QPixmap(":/images/button_apply.png"));
	button(QWizard::CommitButton)->setIcon(QPixmap(":/images/button_ok.png"));

	setWindowTitle(tr("New Project Wizard"));
	adjustSize();
}

XinxProject::ProjectPtr NewProjectWizard::createProject()
{
	if (! m_project)
	{
		const QString projectPath = QDir::fromNativeSeparators(field("project.path").toString());

		m_project = XinxProject::Project::create(projectPath, true);
		m_project->setActivatedPlugin(m_plugins);

		NewProjectTemplate * templateProject = m_templateDialog->currentTemplate();
		if (templateProject)
		{
			foreach(NewProjectTemplate::Property p, templateProject->properties())
			{
				m_project->writeProperty(p.first, p.second);
			}

			foreach(IXinxPluginNewProjectConfigurationPage * page, m_wizardPages)
			{
				if (templateProject->wizardPages().contains(page->pagePluginId()))
					if (! page->saveSettingsDialog(m_project))
						qWarning(qPrintable(tr("Can't save \"%1\" page").arg(page->windowTitle())));
			}
		}
	}
	return m_project;
}

int NewProjectWizard::nextId() const
{
	if (currentId() == startId())
	{
		NewProjectTemplate * templateProject = m_templateDialog->currentTemplate();
		if (! templateProject)
			return qHash(m_projectPage->pagePluginId());

		QString id = m_templateDialog->pagePluginId();
		foreach(const QString & pageName, templateProject->wizardPages())
		{
			m_nextId.insert(qHash(id), pageName);
			id = pageName;
		}
		m_nextId.insert(qHash(id), m_lastDialog->pagePluginId());
		m_nextId.insert(qHash(m_lastDialog->pagePluginId()), QString());

		m_plugins = templateProject->plugins();
	}
	QString id = m_nextId.value(currentId());
	while (! id.isEmpty())
	{
		IXinxPluginNewProjectConfigurationPage * page = m_wizardPages.value(id);
		if (page && page->pageIsVisible()) break;
		id = m_nextId.value(qHash(id));
	}

	if (! id.isEmpty())
		return qHash(id);
	else
		return -1;
}

/* ProjectPageImpl */

ProjectPageImpl::ProjectPageImpl(const QString & filename)
{
	setupUi(this);
	setTitle(windowTitle());

	_path = QString();
	m_projectPathEdit->setText(filename);

	registerField("project.name*",     m_projectNameEdit);
	registerField("project.path",     m_projectPathEdit);

	connect(m_projectPathEdit, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
}

bool ProjectPageImpl::isComplete() const
{
	if (! QWizardPage::isComplete()) return false;
	if (m_projectPathEdit->text().isEmpty()) return false;
	if (! QDir(QDir::fromNativeSeparators(m_projectPathEdit->text())).exists()) return false;
	return true;
}

void ProjectPageImpl::on_m_projectPathBtn_clicked()
{
	m_projectPathEdit->changePath(this, XINXConfig::self()->config().project.defaultPath);
}

void ProjectPageImpl::on_m_projectNameEdit_textChanged(const QString & text)
{
	QString newProjectDirectory = QDir(XINXConfig::self()->config().project.defaultPath).absoluteFilePath(text);
	if (m_projectPathEdit->text() == _path)
	{
		_path = QDir::toNativeSeparators(newProjectDirectory);
		m_projectPathEdit->setText(_path);
	}
}

QString ProjectPageImpl::pagePluginId() const
{
	return "StdProjectName";
}

bool ProjectPageImpl::pageIsVisible() const
{
	return true;
}

bool ProjectPageImpl::saveSettingsDialog(XinxProject::ProjectPtr project)
{
	project->setProjectName(field("project.name").toString());

	return true;
}

/* VersionsPageImpl */

VersionsPageImpl::VersionsPageImpl()
{
	setTitle(tr("Define Revision Control"));
	setSubTitle(tr("Define the revision control used for the project if you want to used one."));

	m_noRevisionControl = new QRadioButton(tr("No revision control system."), this);
	m_noRevisionControl->setChecked(true);

	QMapIterator<QString,QString> revision_control_iterator(VersionControl::Manager::self()->revisionControlIds());
	while (revision_control_iterator.hasNext())
	{
		revision_control_iterator.next();

		m_revisionBtn.append(qMakePair(new QRadioButton(revision_control_iterator.value(), this), revision_control_iterator.key()));
	}

	QVBoxLayout * layout = new QVBoxLayout;
	layout->addWidget(m_noRevisionControl);
	QPair<QRadioButton*,QString> button;
	foreach(button, m_revisionBtn)
	{
		layout->addWidget(button.first);
	}
	setLayout(layout);

}

QString VersionsPageImpl::pagePluginId() const
{
	return "StdProjectRevision";
}

bool VersionsPageImpl::pageIsVisible() const
{
	return true;
}

bool VersionsPageImpl::saveSettingsDialog(XinxProject::ProjectPtr project)
{
	QString rcs = QString();
	if (! m_noRevisionControl->isChecked())
	{
		QPair<QRadioButton*,QString> btn;
		foreach(btn, m_revisionBtn)
		{
			if (btn.first->isChecked())
			{
				rcs = btn.second;
				break;
			}
		}
	}
	project->setProjectRCS(rcs);

	return true;
}

/* LastPageImpl */

LastPageImpl::LastPageImpl()
{
	setTitle(tr("Project created"));
	setSubTitle(tr("The project is now created."));

	QLabel * label = new QLabel(tr("The project is now created, please enjoy."), this);

	QVBoxLayout * layout = new QVBoxLayout;
	layout->addWidget(label);
	setLayout(layout);

}

QString LastPageImpl::pagePluginId() const
{
	return "StdFinish";
}

bool LastPageImpl::pageIsVisible() const
{
	return true;
}

bool LastPageImpl::saveSettingsDialog(XinxProject::ProjectPtr project)
{
	Q_UNUSED(project);
	return true;
}
