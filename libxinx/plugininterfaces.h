/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef INTERFACES_H_
#define INTERFACES_H_

// Qt header
#include <QtPlugin>
#include <QHash>
#include <QStringList>
#include <QTextCharFormat>
#include <QIcon>

// Xinx header
#include <syntaxhighlighter.h>
#include <filecontentstructure.h>

class SyntaxHighlighter;
class XINXConfig;
class QObject;
class QTextDocument;
class QKeyEvent;
class QCompleter;
class RCS;
class AbstractFileContainer;
class AbstractEditor;
class QWizardPage;
class XSLProject;

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
	virtual ~IXinxPlugin() {};

	/*!
	 * Called when the plugin is loaded.
	 * \param lang The lang in which the plugin must be load.
	 */
	virtual bool initializePlugin( const QString & lang ) = 0;
	/*! Get an information from the attribute. List of informations can be found in the \e PluginAttribute enum. */
	virtual QVariant getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr ) = 0;

	/*! List of tools with the default value where find the tool */
	virtual QList< QPair<QString,QString> > pluginTools() { return QList< QPair<QString,QString> >(); };

	//! Call when a new project is created or opened
	virtual bool initializeProject( XSLProject * project ) { Q_UNUSED( project ); return true; };
	///! Call before the project is closed
	virtual bool destroyProject( XSLProject * project ) { Q_UNUSED( project ); return true; };
};

/*!
 * This class is used to propose to the user modify their options. If the Plugins inherits from this interface
 * the XinxPluginSelector propose automatically a custom button.
 *
 * The Plugin create a wigdet contains the user interface, and have two method for put and get the configuration
 * in the dialog.
 *
 * Xinx create the dialog with the Ok and Cancel button.
 */
class IXinxPluginConfiguration {
public:
	//! Destroy the interface
	virtual ~IXinxPluginConfiguration() {};

	//! Create a widget used in a wrapper for the configuration dialog box.
	virtual QWidget * createSettingsDialog() = 0;
	//! Load settings to dialog box
	virtual bool loadSettingsDialog( QWidget * widget ) = 0;
	//! Save settings to dialog box
	virtual bool saveSettingsDialog( QWidget * widget ) = 0;
};

/*!
 * This class is used to permit to read and save some property in the project file. To do this the plugin can
 * also propose an user interface and a list of wizard pages.
 */
class IXinxPluginProjectConfiguration {
public:
	//! Destroy the interface
	virtual ~IXinxPluginProjectConfiguration() {};

	//! Create a widget used in the project dialog
	virtual QWidget * createProjectSettingsPage() = 0;
	//! Load the settings in the settings page
	virtual bool loadProjectSettingsPage( QWidget * widget ) = 0;
	//! Save the settings in the settings page
	virtual bool saveProjectSettingsPage( QWidget * widget ) = 0;

	//! Create some page used in the wizard page
	virtual QList<QWizardPage*> createNewProjectSettingsPages( int nextid ) = 0;
	//! Save the wizard settings page in the project
	virtual bool saveNewProjectSettingsPage( XSLProject * project, QWizardPage * page ) = 0;
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
	virtual ~IRCSPlugin() {};

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
	/*!
	 * Structure indicate if a file type plugin can be save as specifique and the
	 * directory where the file must be stored if it can. This is the default value,
	 * Real value are stored in the configuration.
	 */
	struct struct_properties {
		bool canBeCommitToRCS;         //!< not yet implemented
		bool canBeFindInConfiguration; //!< not yet implemented
		bool canBeSaveAsSpecifique;
		QString specifiqueSubDirectory;
	};

	//! Destroy a file type
	virtual ~IFileTypePlugin() {};

	//! A description of a file type
	virtual QString description() = 0;
	//! Used to choose a file in a dialog box
	virtual QString match() = 0;
	//! Return the icon for the filte type.
	virtual QIcon icon() = 0;

	//! Return some properties for the file type
	virtual struct_properties properties() = 0;

	//! Create an editor with the given filename
	virtual AbstractEditor * createEditor( const QString & filename = QString() ) = 0;
	//! Create a parser
	virtual FileContentElement * createElement( FileContentElement * parent = 0, int line = -1, const QString & filename = QString() ) = 0;
};

/*!
 * This interface represents a plugins used for show manage an extention
 */
class IFilePlugin : virtual public IXinxPlugin {
public:
	//! Destroy the interface. Used to hide warning when using the interface.
	virtual ~IFilePlugin() {};

	//! List of file type knew by the plugins.
	virtual QList<IFileTypePlugin*> fileTypes() = 0;
};

/*!
 * This interface represents a plugins. The plugins is used to highlight the text.
 */
class IPluginSyntaxHighlighter : virtual public IXinxPlugin {
public:
	//! Destroy the interface. Used to hide warning when using the interface.
	virtual ~IPluginSyntaxHighlighter() {};

	//! Return the list of plugins what can process the file for syntax highlighter.
	virtual QStringList highlighters() = 0;

	//! Return the highlighter that can manage the extention.
	virtual QString highlighterOfExtention( const QString & extention ) = 0;
	//! Return the default formats of a highlighter
	virtual QHash<QString,QTextCharFormat> formatOfHighlighter( const QString & highlighter ) = 0;
	//! Return an example of highlighter.
	virtual QString exampleOfHighlighter( const QString & highlighter ) = 0;

	//! Create a syntax highlighter based on a text document.
	virtual SyntaxHighlighter * createHighlighter( const QString & highlighter, QTextDocument* parent, XINXConfig * config = NULL ) = 0;
};

Q_DECLARE_INTERFACE(IXinxPlugin, "org.shadoware.xinx.IXinxPlugin/1.0")
Q_DECLARE_INTERFACE(IXinxPluginConfiguration, "org.shadoware.xinx.IXinxPluginConfiguration/1.0")
Q_DECLARE_INTERFACE(IXinxPluginProjectConfiguration, "org.shadoware.xinx.IXinxPluginProjectConfiguration/1.0")
Q_DECLARE_INTERFACE(IRCSPlugin, "org.shadoware.xinx.IRCSPlugin/1.0")
Q_DECLARE_INTERFACE(IFilePlugin, "org.shadoware.xinx.IFilePlugin/1.0")
Q_DECLARE_INTERFACE(IPluginSyntaxHighlighter, "org.shadoware.xinx.IPluginSyntaxHighlighter/1.0")

#endif /*INTERFACES_H_*/
