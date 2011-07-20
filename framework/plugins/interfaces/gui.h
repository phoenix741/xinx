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

#pragma once
#ifndef _GUI_INTERFACES_H_
#define _GUI_INTERFACES_H_

#include <QWizardPage>
#include <QtPlugin>
#include <QSharedPointer>

namespace XinxProject
{
class Project;

typedef QSharedPointer<Project> ProjectPtr;
}

class XinxDockWidget;

/*!
 * This interface is used to propose one page of configuration. For each page, the program
 * can modify settings and save or restore value.
 */
class IXinxPluginConfigurationPage
{
public:
	virtual ~IXinxPluginConfigurationPage() {}

	//! Return an image for the configration page.
	virtual QPixmap image() = 0;
	//! Return a name for the configuration page. The name must be short
	virtual QString name() = 0;

	//! Create a widget used in a wrapper for the configuration dialog box.
	virtual QWidget * settingsDialog() = 0;
	//! Load settings to dialog box
	virtual bool loadSettingsDialog() = 0;
	//! Save settings to dialog box
	virtual bool saveSettingsDialog() = 0;
	//! This method is called in the inverse case of \e saveSettingsDialog(). If the dialog must made process on cancel.
	virtual bool cancelSettingsDialog() = 0;
	//! Validate the dialog, if error, message must be filled
	virtual bool isSettingsValid(QString & message) = 0;
	//! Is visible
	virtual bool isVisible() = 0;
};

/*!
 * This class contains necessary method to use in a project property page.
 */
class IXinxPluginProjectConfigurationPage : public IXinxPluginConfigurationPage
{
public:
	virtual ~IXinxPluginProjectConfigurationPage() {}

	//! Set the project to use in the dialog to load and save settings.
	virtual void setProject(XinxProject::ProjectPtr project) = 0;
};

/*!
 * New WizardPage to use in XINX.
 */
class IXinxPluginNewProjectConfigurationPage : public QWizardPage
{
public:
	//! Return the page id used in project template
	virtual QString pagePluginId() const = 0;
	//! Return true if this page can be the next page
	virtual bool pageIsVisible() const = 0;
	//! Save the wizard settings page in the project
	virtual bool saveSettingsDialog(XinxProject::ProjectPtr project) = 0;
private:
};

/*!
* This class is used to propose to the user modify their options. If the Plugins inherits from this interface
* the custom dialog show pages.
*
* The Plugin create a wigdet contains the user interface, and have two method for put and get the configuration
* in the dialog.
*
* Xinx integrate the widget in the configuration dialog.
*/
class IXinxPluginConfiguration
{
public:
	//! Destroy the interface
	virtual ~IXinxPluginConfiguration() {}

	//! Create a widget used in a wrapper for the configuration dialog box.
	virtual QList<IXinxPluginConfigurationPage*> createSettingsDialog(QWidget * parent) = 0;
};

/*!
 * This class is used to permit to read and save some property in the project file. To do this the plugin can
 * also propose an user interface and a list of wizard pages.
 */
class IXinxPluginProjectConfiguration
{
public:
	//! Destroy the interface
	virtual ~IXinxPluginProjectConfiguration() {}

	//! Create a widget used in the project dialog
	virtual QList<IXinxPluginProjectConfigurationPage*> createProjectSettingsPage(QWidget * parent) = 0;

	//! Create some page used in the wizard page
	virtual QList<IXinxPluginNewProjectConfigurationPage*> createNewProjectSettingsPages() = 0;
};

class IDockPlugin
{
public:
	//! Destroy the interface
	virtual ~IDockPlugin() {}

	/*!
	 * This method is call when the main form is created to create new
	 * dock widget. A number (for quick access) is associate if free.
	 */
	virtual QList<XinxDockWidget*> createDocksWidget(QWidget * parent) = 0;
};

Q_DECLARE_INTERFACE(IXinxPluginConfiguration, "org.shadoware.xinx.IXinxPluginConfiguration/1.0");
Q_DECLARE_INTERFACE(IXinxPluginProjectConfiguration, "org.shadoware.xinx.IXinxPluginProjectConfiguration/1.0");
Q_DECLARE_INTERFACE(IDockPlugin, "org.shadoware.xinx.IDockPlugin/1.0");

#endif /*_GUI_INTERFACES_H_*/
