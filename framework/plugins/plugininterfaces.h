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

#ifndef INTERFACES_H_
#define INTERFACES_H_
#pragma once

// Qt header
#include <QtPlugin>
#include <QHash>
#include <QStringList>
#include <QTextCharFormat>
#include <QIcon>
#include <QWizardPage>

// Xinx header
#include <core/appsettings.h>
#include <actions/actioninterface.h>

class XINXConfig;
class QObject;
class QKeyEvent;
class QCompleter;
class RCS;
class AbstractFileContainer;
class AbstractEditor;
class QWizardPage;
class XinxProject;
class XinxFormatScheme;
class XsltParser;

namespace ContentView2
{
class Parser;
}

/*!
 * This intereface is used to create a plugin used by XINX.
 * At start, XINX load all plugins and call the initializePlugin method from the plugin.
 *
 * The plugin must define some attribute used to show informations to user (in about box and
 * in the list of Plugin).
 *
 * A plugin can return a list of tools (with a default value). This tool will be show in the
 * customize dialog of XINX.
 */
class IXinxPlugin
{
public:
	/*! Information that can be asked to plugin */
	enum PluginAttribute
	{
		PLG_NAME        = 1001, //!< Name of the plugin
		PLG_DESCRIPTION = 1002, //!< Long description of the plugin
		PLG_ICON        = 1003, //!< An icon that represent the plugin
		PLG_AUTHOR      = 1004, //!< The author (or authors) of the plugin
		PLG_EMAIL       = 1005, //!< Email to contact the author (or team)
		PLG_WEBSITE     = 1006, //!< The site where we can find the plugin
		PLG_VERSION     = 1007, //!< The Version of the plugin
		PLG_LICENCE     = 1008  //!< The Licence of the plugin
	};

	//! Destroy the plugin
	virtual ~IXinxPlugin() {}

	/*!
	 * Called when the plugin is loaded.
	 * \param lang The lang in which the plugin must be load.
	 */
	virtual bool initializePlugin(const QString & lang) = 0;
	/*! Get an information from the attribute. List of informations can be found in the \e PluginAttribute enum. */
	virtual QVariant getPluginAttribute(const enum IXinxPlugin::PluginAttribute & attr) = 0;

	/*! List of tools with the default value where find the tool */
	virtual QList< QPair<QString,QString> > pluginTools()
	{
		return QList< QPair<QString,QString> >();
	}

	//! Return a list of action (order in menu) used for dynamic action
	virtual XinxAction::MenuList actions()
	{
		return XinxAction::MenuList();
	}
};

class IContentViewParserPlugin : public virtual IXinxPlugin
{
public:
	//! Destroy the plugin
	virtual ~IContentViewParserPlugin() {}

	/*!
	 * Create a content view parser, this content view parser will any file (of the correct type of course)
	 * If the parser is null, no content view will be load.
	 */
	virtual ContentView2::Parser * createParser(const QString & type) = 0;
};

/*!
 * This interface is used to create new XsltParser for XINX if necessary.
 */
class IXinxXsltParser : public virtual IXinxPlugin
{
public:
	//! Destroy the plugin
	virtual ~IXinxXsltParser() {}

	/*!
	 * Create a parser of Xslt Stylesheet.
	 */
	virtual XsltParser * createXsltParser() = 0;
};

/*!
 * A project initialisation step
 */
class IProjectInitialisationStep {
public:
	//! Destroy the step
	virtual ~IProjectInitialisationStep() {}

	virtual QString name() = 0;
	virtual bool process() = 0;
};

/*!
 * This interface is used to change the input/output of XINX
 */
class IXinxInputOutputPlugin : public virtual IXinxPlugin
{
public:
	//! Destroy a Input/Output plugin
	virtual ~IXinxInputOutputPlugin() {}

	//! List of step of initialisation of XINX project
	virtual QList<IProjectInitialisationStep*> loadProjectStep(XinxProject * project) = 0;
	//! List of step of deinitialisation of XINX project
	virtual QList<IProjectInitialisationStep*> closeProjectStep(XinxProject * project) = 0;

	//! Call when a file is loaded
	virtual QIODevice * loadFile(const QString & filename) = 0;
	/*!
	 * For the given \e filename, get a a dialog box to save the file. If the
	 * the accept boolean is true, XINX don't call other plugin. If the file
	 * return false, the save is cancelled, else, the save is called.
	 */
	virtual QString getFilename(const QString & filename, const QString & defaultFilename, const QString & filter, bool saveAs, bool & accept, QWidget * widget = 0) = 0;
	//! Call when a file is saved
	virtual QIODevice * saveFile(const QString & filename, const QString & oldfilename) = 0;
};

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
	//! Validate dialog
	virtual bool isSettingsValid() = 0;
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
	virtual void setProject(XinxProject * project) = 0;
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
class IXinxPluginConfiguration : virtual public IXinxPlugin
{
public:
	//! Destroy the interface
	virtual ~IXinxPluginConfiguration() {}

	//! Create a widget used in a wrapper for the configuration dialog box.
	virtual QList<IXinxPluginConfigurationPage*> createSettingsDialog(QWidget * parent) = 0;
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
	virtual bool saveSettingsDialog(XinxProject * project) = 0;
private:
};

/*!
 * This class is used to permit to read and save some property in the project file. To do this the plugin can
 * also propose an user interface and a list of wizard pages.
 */
class IXinxPluginProjectConfiguration : virtual public IXinxPlugin
{
public:
	//! Destroy the interface
	virtual ~IXinxPluginProjectConfiguration() {}

	//! Create a widget used in the project dialog
	virtual QList<IXinxPluginProjectConfigurationPage*> createProjectSettingsPage(QWidget * parent) = 0;

	//! Create some page used in the wizard page
	virtual QList<IXinxPluginNewProjectConfigurationPage*> createNewProjectSettingsPages() = 0;
};

class IDockPlugin : virtual public IXinxPlugin
{
public:
	//! Destroy the interface
	virtual ~IDockPlugin() {}

	/*!
	 * This method is call when the main form is created to create new
	 * dock widget. A number (for quick access) is associate if free.
	 */
	virtual QList<QWidget*> createDocksWidget(QWidget * parent) = 0;
};

/*!
 * This plugins is used to add a new revision control system to XINX. For this purpose,
 * the plugin give a list of managed rcs and a description (show in the project page).
 *
 * When XINX must create a RCS object, he call the IRCSPlugin::createRCS() method with
 * the parameter used in the constructor, and the revision control system to create (one from
 * the rcs() method).
 * The method will create a derivated object of RCS with the required implementation for the
 * revision control system.
 */
class IRCSPlugin : virtual public IXinxPlugin
{
public:
	/// Destroy the plugin
	virtual ~IRCSPlugin() {}

	/// List of revision control system proposed by the plugin
	virtual QStringList rcs() = 0;
	/// Description of each revision control system
	virtual QString descriptionOfRCS(const QString & rcs) = 0;
	/*!
	 * Create a revision control system for the revision control system \e rcs
	 * and the path \e basePath.
	 */
	virtual RCS * createRCS(const QString & rcs, const QString & basePath = QString()) = 0;
};

/*!
 * This interface is used to create an editor for an associated file type.
 */
class IFileTypePlugin
{
public:
	//! Destroy a file type
	virtual ~IFileTypePlugin() {}

	//! A description of a file type
	virtual QString description() = 0;
	//! Used to choose a file in a dialog box
	virtual QString match() = 0;
	//! Return the icon for the filte type.
	virtual QString icon() = 0;

	virtual QString parserType() = 0;

	//! Create an editor with the given filename
	virtual AbstractEditor * createEditor(const QString & filename = QString()) = 0;
	friend class EditorFactory;
};

/*!
 * This interface is used to create an editor for text file type.
 */
class IFileTextPlugin : public IFileTypePlugin
{
public:
	//! Identifier used to find the correct file type
	virtual QString highlighterId() const = 0;

	//! Return the format scheme used to draw the text.
	virtual XinxFormatScheme * createFormatScheme(XINXConfig * config) const = 0;

	//! Create a language description that can be used with QCodeEdit.
	virtual QString createLanguageDescription() const = 0;

	//! Return an example of highlighter.
	virtual QString fileExample() const = 0;
};

/*!
 * This interface represents a plugins used for show manage an extention
 */
class IFilePlugin : virtual public IXinxPlugin
{
public:
	//! Destroy the interface. Used to hide warning when using the interface.
	virtual ~IFilePlugin() {}

	//! List of file type knew by the plugins.
	virtual QList<IFileTypePlugin*> fileTypes() = 0;
};

/*!
  * Define a filename resolver  object used to find the import, and find file in parsing of stylesheet
  */
class IFileResolverPlugin
{
public:
	//! Destroy the interface. Used to hide warning when using the interface
	virtual ~IFileResolverPlugin() {}

	//! Return the name of the resolver
	virtual QString name() = 0;
	//! Resolver id
	virtual QString id() = 0;
	//! Return true if the resolver can be called, else return false.
	virtual bool isActivated() = 0;
	//! Resolve the file \e nameToResolve. The \e currentPath, can be used to find the file relatively of the current open editor.
	virtual QString resolveFileName(const QString & nameToResolve, const QString & currentPath = QString()) = 0;
};

/*!
 * Return the list of resolver than can be used in the application.
 * Actually, only resolver of imported file exists.
 */
class IResolverPlugin : virtual public IXinxPlugin
{
public:
	//! Destroy the interface. Used to hide warning when using the interface
	virtual ~IResolverPlugin() {}

	//! Return the list of resolver used to find import
	virtual QList<IFileResolverPlugin*> fileResolvers() = 0;
};

Q_DECLARE_INTERFACE(IXinxPlugin, "org.shadoware.xinx.IXinxPlugin/1.0");
Q_DECLARE_INTERFACE(IXinxXsltParser, "org.shadoware.xinx.IXinxXsltParser/1.0");
Q_DECLARE_INTERFACE(IXinxInputOutputPlugin, "org.shadoware.xinx.IXinxInputOutputPlugin/1.0");
Q_DECLARE_INTERFACE(IXinxPluginConfiguration, "org.shadoware.xinx.IXinxPluginConfiguration/1.0");
Q_DECLARE_INTERFACE(IXinxPluginProjectConfiguration, "org.shadoware.xinx.IXinxPluginProjectConfiguration/1.0");
Q_DECLARE_INTERFACE(IDockPlugin, "org.shadoware.xinx.IDockPlugin/1.0");
Q_DECLARE_INTERFACE(IRCSPlugin, "org.shadoware.xinx.IRCSPlugin/1.0");
Q_DECLARE_INTERFACE(IFilePlugin, "org.shadoware.xinx.IFilePlugin/1.0");
Q_DECLARE_INTERFACE(IResolverPlugin, "org.shadoware.xinx.IResolverPlugin/1.0");
Q_DECLARE_INTERFACE(IContentViewParserPlugin, "org.shadoware.xinx.IContentViewParserPlugin/1.0")

#endif /*INTERFACES_H_*/
