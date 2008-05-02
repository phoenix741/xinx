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
#include <iextendededitor.h>

class SyntaxHighlighter;
class XINXConfig;
class QObject;
class QTextDocument;
class QTextEdit;
class QKeyEvent;
class QCompleter;
class RCS;

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
 * \internal
 * Structure used for communication beetween XinxPluginsLoader and XinxPluginSelector 
 */
struct XinxPluginElement {
	bool isStatic;
	bool isActivated;
	QObject * plugin;
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
 * This interface represents a plugins used for show manage an extention 
 */
class IFilePlugin : virtual public IXinxPlugin {
public:
	//! Destroy the interface. Used to hide warning when using the interface.
	virtual ~IFilePlugin() {};
	
	//! List of extentions that can manage the plugins.
	virtual QStringList extentions() = 0;
	//! Description of the extention
	virtual QHash<QString,QString> extentionsDescription() = 0;
	
	//! Return the icon for the plugin.
	virtual QIcon icon( const QString & extention ) = 0;
};

/*!
 * This interface represents a plugins. The plugins is used to highlight the text.
 */
class IPluginSyntaxHighlighter : virtual public IFilePlugin {
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

	virtual SyntaxHighlighter * createHighlighter( const QString & highlighter, QObject* parent = NULL, XINXConfig * config = NULL ) = 0;
	virtual SyntaxHighlighter * createHighlighter( const QString & highlighter, QTextDocument* parent, XINXConfig * config = NULL ) = 0;
	virtual SyntaxHighlighter * createHighlighter( const QString & highlighter, QTextEdit* parent, XINXConfig * config = NULL ) = 0;
};

/*!
 * This interface represents a plugins used to auto-indent text.
 */
class IPluginPrettyPrint : virtual public IFilePlugin {
public:
	virtual ~IPluginPrettyPrint() {};

	//! Return the list of pretty printers
	virtual QStringList prettyPrinters() = 0;
	//! Give the pretty printer for an extention
	virtual QString prettyPrinterOfExtention( const QString & extention ) = 0;
	
	//! Pretty Print a text \e text. If an error occure, the error is returned in variables \e errorStr, \e line, and \e column
	virtual QString prettyPrint( const QString & plugin, const QString & text, QString * errorStr = NULL, int * line = 0, int * column = 0 ) = 0;
};

class IPluginExtendedEditor : virtual public IFilePlugin {
public:
	virtual ~IPluginExtendedEditor() {};
	
	//! Return possibles extended editor.
	virtual QStringList extendedEditors() = 0;
	//! Give the extended editor to use for this extentions
	virtual QString extendedEditorOfExtention( const QString & extention ) = 0;
	
	virtual void commentSelectedText( const QString & plugin, IXinxExtendedEditor * editor, bool uncomment ) = 0;
	virtual FileContentElement * createModelData( const QString & plugin, IXinxExtendedEditor * editor, FileContentElement * parent = NULL, const QString & filename = QString(), int line = 0 ) = 0;
	virtual void createCompleter( const QString & plugin, IXinxExtendedEditor * editor ) = 0;
	virtual QCompleter * completer( const QString & plugin, IXinxExtendedEditor * editor ) = 0;
	virtual bool keyPress( const QString & plugin, IXinxExtendedEditor * editor, QKeyEvent * event ) = 0;
	virtual QPair<QString,int> searchWord( const QString & plugin, IXinxExtendedEditor * editor, const QString & word ) = 0;
};

Q_DECLARE_INTERFACE(IXinxPlugin, "org.shadoware.xinx.IXinxPlugin/1.0")
Q_DECLARE_INTERFACE(IXinxPluginConfiguration, "org.shadoware.xinx.IXinxPluginConfiguration/1.0")
Q_DECLARE_INTERFACE(IRCSPlugin, "org.shadoware.xinx.IRCSPlugin/1.0")
Q_DECLARE_INTERFACE(IFilePlugin, "org.shadoware.xinx.IFilePlugin/1.0")
Q_DECLARE_INTERFACE(IPluginSyntaxHighlighter, "org.shadoware.xinx.IPluginSyntaxHighlighter/1.0")
Q_DECLARE_INTERFACE(IPluginPrettyPrint, "org.shadoware.xinx.IPluginPrettyPrint/1.0")
Q_DECLARE_INTERFACE(IPluginExtendedEditor, "org.shadoware.xinx.IPluginExtendedEditor/1.0")

#endif /*INTERFACES_H_*/
