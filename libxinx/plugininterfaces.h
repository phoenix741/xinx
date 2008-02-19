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
#include <isyntaxhighlighter.h>
#include <filecontentstructure.h>
#include <iextendededitor.h>

class SyntaxHighlighter;
class XINXConfig;
class QObject;
class QTextDocument;
class QTextEdit;
class QKeyEvent;
class QCompleter;

class IXinxPlugin {
public:
	enum PluginAttribute { PLG_NAME, PLG_DESCRIPTION, PLG_ICON, PLG_AUTHOR, PLG_EMAIL, PLG_WEBSITE, PLG_VERSION, PLG_LICENCE };
	
	virtual ~IXinxPlugin() {};

	virtual bool initializePlugin( const QString & lang ) = 0;
//	virtual QString getPluginAttribute( enum IXinxPlugin::PluginAttribute & attr ) = 0;
	
	/* TODO: Pour la boite de dialogue, il faudra utiliser un objet spécialisé qui sait faire load an save */
};

class IFilePlugin : public IXinxPlugin {
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
	//! Return the description of each plugins
	virtual QHash<QString,QString> descriptionOfHighlighters() = 0;

	//! Return the highlighter that can manage the extention.
	virtual QString highlighterOfExtention( const QString & extention ) = 0;
	//! Return the default formats of a highlighter
	virtual QHash<QString,QTextCharFormat> formatOfHighlighter( const QString & highlighter ) = 0;
	//! Return an example of highlighter.
	virtual QString exampleOfHighlighter( const QString & highlighter ) = 0;
	/*!
	 * Process a block
	 * \param highlighter The highlighter to used
	 * \param formats Formats to used for the coloration.
	 * \param i The interface with the application.
	 * \param text The text to process.
	 */
	virtual void highlightBlock( const QString & highlighter, const QHash<QString,QTextCharFormat> & formats, IXinxSyntaxHighlighter * i, const QString& text ) = 0;
};

/*!
 * This interface represents a plugins used to auto-indent text.
 */
class IPluginPrettyPrint : virtual public IFilePlugin {
public:
	virtual ~IPluginPrettyPrint() {};

	//! Return the list of pretty printers
	virtual QStringList prettyPrinters() = 0;
	//! Description of a pretty printer.
	virtual QHash<QString,QString> descriptionOfPrettyPrinters() = 0;
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
	//! Description of extended editor.
	virtual QHash<QString,QString> descriptionOfExtendedEditors() = 0;
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
Q_DECLARE_INTERFACE(IFilePlugin, "org.shadoware.xinx.IFilePlugin/1.0")
Q_DECLARE_INTERFACE(IPluginSyntaxHighlighter, "org.shadoware.xinx.IPluginSyntaxHighlighter/1.0")
Q_DECLARE_INTERFACE(IPluginPrettyPrint, "org.shadoware.xinx.IPluginPrettyPrint/1.0")
Q_DECLARE_INTERFACE(IPluginExtendedEditor, "org.shadoware.xinx.IPluginExtendedEditor/1.0")

#endif /*INTERFACES_H_*/
