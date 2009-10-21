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
class ContentViewParser;
class QDockWidget;

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
class IXinxPlugin {
public:
	/*! Information that can be asked to plugin */
	enum PluginAttribute {
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
	virtual bool initializePlugin( const QString & lang ) = 0;
	/*! Get an information from the attribute. List of informations can be found in the \e PluginAttribute enum. */
	virtual QVariant getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr ) = 0;

	/*! List of tools with the default value where find the tool */
	virtual QList< QPair<QString,QString> > pluginTools() { return QList< QPair<QString,QString> >(); }

	//! Call when a new project is created or opened
	virtual bool initializeProject( XinxProject * project ) { Q_UNUSED( project ); return true; }
	//! Call before the project is closed
	virtual bool destroyProject( XinxProject * project ) { Q_UNUSED( project ); return true; }
	//! Return a list of action (order in menu) used for dynamic action
	virtual XinxAction::MenuList actions() { return XinxAction::MenuList(); }
};

/*!
 * This interface is used to propose one page of configuration. For each page, the program
 * can modify settings and save or restore value.
 */
class IXinxPluginConfigurationPage {
	virtual ~IXinxPluginConfigurationPage() {}

	//! Return an image for the configration page.
	virtual QPixmap image();
	//! Return a name for the configuration page. The name must be short
	virtual QPixmap name();

	//! Create a widget used in a wrapper for the configuration dialog box.
	virtual QWidget * settingsDialog() = 0;
	//! Load settings to dialog box
	virtual bool loadSettingsDialog() = 0;
	//! Save settings to dialog box
	virtual bool saveSettingsDialog() = 0;
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
class IXinxPluginConfiguration {
public:
	//! Destroy the interface
	virtual ~IXinxPluginConfiguration() {}

	//! Create a widget used in a wrapper for the configuration dialog box.
	virtual QList<IXinxPluginConfigurationPage*> createSettingsDialog() = 0;
};

/*!
 * This class is used to permit to read and save some property in the project file. To do this the plugin can
 * also propose an user interface and a list of wizard pages.
 */
class IXinxPluginProjectConfiguration {
public:
	//! Destroy the interface
	virtual ~IXinxPluginProjectConfiguration() {}

	//! Create a widget used in the project dialog
	virtual QWidget * createProjectSettingsPage() = 0;
	//! Load the settings in the settings page
	virtual bool loadProjectSettingsPage( QWidget * widget ) = 0;
	//! Save the settings in the settings page
	virtual bool saveProjectSettingsPage( QWidget * widget ) = 0;

	//! Create some page used in the wizard page
	virtual QList<QWizardPage*> createNewProjectSettingsPages( int nextid ) = 0;
	//! Save the wizard settings page in the project
	virtual bool saveNewProjectSettingsPage( XinxProject * project, QWizardPage * page ) = 0;
};

class IDockPlugin : virtual public IXinxPlugin {
public:
	//! Destroy the interface
	virtual ~IDockPlugin() {}

	/*!
	 * This method is call when the main form is created to create new
	 * dock widget. A number (for quick access) is associate if free.
	 */
	virtual QDockWidget * createDockWidget( QWidget * parent ) = 0;
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
class IRCSPlugin : virtual public IXinxPlugin {
public:
	/// Destroy the plugin
	virtual ~IRCSPlugin() {}

	/// List of revision control system proposed by the plugin
	virtual QStringList rcs() = 0;
	/// Description of each revision control system
	virtual QString descriptionOfRCS( const QString & rcs ) = 0;
	/*!
	 * Create a revision control system for the revision control system \e rcs
	 * and the path \e basePath.
	 */
	virtual RCS * createRCS( const QString & rcs, const QString & basePath ) = 0;
};

/*!
 * This interface is used to create an editor for an associated file type.
 */
class IFileTypePlugin {
public:
	//! Destroy a file type
	virtual ~IFileTypePlugin() {}

	//! A description of a file type
	virtual QString description() = 0;
	//! Used to choose a file in a dialog box
	virtual QString match() = 0;
	//! Return the icon for the filte type.
	virtual QString icon() = 0;

	//! Return some properties for the file type
	virtual AppSettings::struct_extentions properties() = 0;
	/*!
	 * Create a content view parser, this content view parser will any file (of the correct type of course)
	 * If the parser is null, no content view will be load.
	 */
	virtual ContentViewParser * createParser() = 0;
protected:
	//! Create an editor with the given filename
	virtual AbstractEditor * createEditor( const QString & filename = QString() ) = 0;
	friend class EditorFactory;
};

/*!
 * This interface is used to create an editor for text file type.
 */
class IFileTextPlugin : public IFileTypePlugin {
public:
	//! Identifier used to find the correct file type
	virtual QString highlighterId() const = 0;

	//! Return the format scheme used to draw the text.
	virtual XinxFormatScheme * createFormatScheme( XINXConfig * config ) const = 0;

	//! Create a language description that can be used with QCodeEdit.
	virtual QString createLanguageDescription() const = 0;

	//! Return an example of highlighter.
	virtual QString fileExample() const = 0;
};

/*!
 * This interface represents a plugins used for show manage an extention
 */
class IFilePlugin : virtual public IXinxPlugin {
public:
	//! Destroy the interface. Used to hide warning when using the interface.
	virtual ~IFilePlugin() {}

	//! List of file type knew by the plugins.
	virtual QList<IFileTypePlugin*> fileTypes() = 0;
};

Q_DECLARE_INTERFACE(IXinxPlugin, "org.shadoware.xinx.IXinxPlugin/1.0");
Q_DECLARE_INTERFACE(IXinxPluginConfiguration, "org.shadoware.xinx.IXinxPluginConfiguration/1.0");
Q_DECLARE_INTERFACE(IXinxPluginProjectConfiguration, "org.shadoware.xinx.IXinxPluginProjectConfiguration/1.0");
Q_DECLARE_INTERFACE(IDockPlugin, "org.shadoware.xinx.IDockPlugin/1.0");
Q_DECLARE_INTERFACE(IRCSPlugin, "org.shadoware.xinx.IRCSPlugin/1.0");
Q_DECLARE_INTERFACE(IFilePlugin, "org.shadoware.xinx.IFilePlugin/1.2");

#endif /*INTERFACES_H_*/
