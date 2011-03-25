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
#include "servicesplugin.h"
#include "servicesprojectpropertyimpl.h"
#include "servicesprojectwizard.h"
#include "webservices.h"
#include "webservicesfiletype.h"
#include "webservicesaction.h"
#include <actions/actionmanager.h>
#include <project/xinxprojectproject.h>

// Qt header
#include <QString>
#include <QVariant>
#include <QApplication>

class OpenWebServicesObjectStep : public IProjectInitialisationStep
{
public:
	OpenWebServicesObjectStep(XinxProject::ProjectPtr project) : _project(project)
	{
	}

	virtual ~OpenWebServicesObjectStep()
	{
	}

	virtual QString name()
	{
		return ServicesPlugin::tr("Open WebServices project");
	}

	virtual bool process()
	{
		_project->addObject("webservice", new WebServicesManager(_project));

		return true;
	}

	XinxProject::ProjectPtr _project;
};

/* ServicesPlugin */

ServicesPlugin::ServicesPlugin()
{
	Q_INIT_RESOURCE(servicesplugin);

	m_fileTypes << new WebServicesFileType;
}

ServicesPlugin::~ServicesPlugin()
{
	qDeleteAll(m_fileTypes);
}

bool ServicesPlugin::initializePlugin(const QString & lang)
{
	return true;
}

QVariant ServicesPlugin::getPluginAttribute(const enum IXinxPlugin::PluginAttribute & attr)
{
	switch (attr)
	{
	case PLG_NAME:
		return tr("XINX WebServices");
	case PLG_DESCRIPTION:
		return tr("Enable the WebServices Capabilities to XINX.");
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
	case PLG_ICON:
		return QPixmap(":/services/images/webservices.png");
	case PLG_EMAIL:
		return "ulrich.vdh@shadoware.org";
	case PLG_WEBSITE:
		return "http://www.shadoware.org";
	case PLG_VERSION:
		return "0.1";
	case PLG_LICENCE:
		return "GPL v2.0 or later";
	}
	return QVariant();
}

QList<IFileTypePlugin*> ServicesPlugin::fileTypes()
{
	return m_fileTypes;
}

void ServicesPlugin::generateActionMenu()
{
	XinxAction::Action * refreshAction = new WebServicesRefreshAction;
	XinxAction::Action * runAction = new WebServicesRunAction;
	XinxAction::Action * runAllAction = new WebServicesRunAllAction;

	XinxAction::ActionManager::self()->insertNameOfMenu("execute", tr("&Execute"));


	XinxAction::ActionManager::self()->addToolBarSeparator("execute");
	XinxAction::ActionManager::self()->addToolBarItem("execute", runAction);
	XinxAction::ActionManager::self()->addToolBarItem("execute", refreshAction);

	XinxAction::ActionManager::self()->addMenuSeparator("execute");
	XinxAction::ActionManager::self()->addMenuItem("execute", runAllAction);
	XinxAction::ActionManager::self()->addMenuItem("execute", runAction);
	XinxAction::ActionManager::self()->addMenuItem("execute", refreshAction);
}

QList<IXinxPluginProjectConfigurationPage*> ServicesPlugin::createProjectSettingsPage(QWidget * parent)
{
	QList<IXinxPluginProjectConfigurationPage*> list;
	list << new ServicesProjectPropertyImpl(parent);
	return list;
}

QList<IXinxPluginNewProjectConfigurationPage*> ServicesPlugin::createNewProjectSettingsPages()
{
	QList<IXinxPluginNewProjectConfigurationPage*> pages;
	pages << new ServicesListPageImpl();
	return pages;
}

QList< IProjectInitialisationStep* > ServicesPlugin::loadProjectStep(XinxProject::ProjectPtr project)
{
	return QList<IProjectInitialisationStep*>() << new OpenWebServicesObjectStep(project);
}

QList< IProjectInitialisationStep* > ServicesPlugin::closeProjectStep(XinxProject::ProjectPtr project)
{
	return QList<IProjectInitialisationStep*>();
}

QString ServicesPlugin::getFilename(AbstractEditor* editor, const QString& filename, const QString& defaultFilename, const QString& filter, bool saveAs, bool& accept, QWidget* widget)
{
	return QString();
}

QIODevice* ServicesPlugin::loadFile(AbstractEditor* editor, const QString& filename)
{
	return NULL;
}

QIODevice* ServicesPlugin::saveFile(AbstractEditor* editor, const QString& filename, const QString& oldfilename)
{
	return NULL;
}


Q_EXPORT_PLUGIN2(servicesplugin, ServicesPlugin)
