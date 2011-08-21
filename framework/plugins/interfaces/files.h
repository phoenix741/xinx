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
#ifndef _FILES_INTERFACES_H_
#define _FILES_INTERFACES_H_

// Qt header
#include <QtPlugin>
#include <QMetaType>
#include <QStringList>
#include <QSharedPointer>

namespace XinxProject
{
class Project;

typedef QSharedPointer<Project> ProjectPtr;
}
namespace ContentView3
{
class Parser;
}
class AbstractEditor;
class XinxFormatScheme;
class XINXConfig;

/*!
 * This interface is used to create an editor for an associated file type.
 */
class IFileTypePlugin
{
public:
	//! Destroy a file type
	virtual ~IFileTypePlugin() {}

	//! The name of the file type plugin
	virtual QString name() = 0;
	//! A description of a file type
	virtual QString description() = 0;
	//! Used to choose a file in a dialog box
	virtual QString match() = 0;
	//! Return the icon for the filte type.
	virtual QString icon() = 0;
	/*!
	 * \brief Return the default file name to use by the editor in case of
	 * no file is defined.
	 */
	virtual QString defaultFileName() = 0;

	/*!
	 * \brief Define the name of the parser type to use to define the content view, and update the context.
	 */
	virtual QString parserType() = 0;

	//! Create an editor
	virtual AbstractEditor * createEditor() = 0;
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
 * A project initialisation step
 */
class IProjectInitialisationStep
{
public:
	//! Destroy the step
	virtual ~IProjectInitialisationStep() {}

	virtual QString name() = 0;
	virtual bool process() = 0;
};

class IContentViewParserPlugin
{
public:
	//! Destroy the plugin
	virtual ~IContentViewParserPlugin() {}

	/*!
	* Create a content view parser, this content view parser will any file (of the correct type of course)
	* If the parser is null, no content view will be load.
	*/
	virtual ContentView3::Parser * createContentParser(const QString & type) = 0;
};

/*!
* This interface represents a plugins used for show manage an extention
*/
class IFilePlugin
{
public:
	//! Destroy the interface. Used to hide warning when using the interface.
	virtual ~IFilePlugin() {}

	//! List of file type knew by the plugins.
	virtual QList<IFileTypePlugin*> createFileTypes() = 0;
};

/*!
 * This interface is used to change the input/output of XINX
 */
class IXinxInputOutputPlugin
{
public:
	//! Destroy a Input/Output plugin
	virtual ~IXinxInputOutputPlugin() {}

	//! List of step of initialisation of XINX project
	virtual QList<IProjectInitialisationStep*> loadProjectStep(XinxProject::ProjectPtr project) = 0;
	//! List of step of deinitialisation of XINX project
	virtual QList<IProjectInitialisationStep*> closeProjectStep(XinxProject::ProjectPtr project) = 0;

	//! Call when a file is loaded
	virtual QIODevice * loadFile(AbstractEditor * editor, const QString & filename) = 0;
	/*!
	 * For the given \e filename, get a a dialog box to save the file. If the
	 * the accept boolean is true, XINX don't call other plugin. If the file
	 * return false, the save is cancelled, else, the save is called.
	 */
	virtual QString getFilename(AbstractEditor * editor, const QString & filename, const QString & defaultFilename, const QString & filter, bool saveAs, bool & accept, QWidget * widget = 0) = 0;
	//! Call when a file is saved
	virtual QIODevice * saveFile(AbstractEditor * editor, const QString & filename, const QString & oldfilename) = 0;
};

typedef QSharedPointer<IFileTypePlugin> IFileTypePluginPtr;

Q_DECLARE_METATYPE(IFileTypePluginPtr);

Q_DECLARE_INTERFACE(IXinxInputOutputPlugin, "org.shadoware.xinx.IXinxInputOutputPlugin/1.0");
Q_DECLARE_INTERFACE(IContentViewParserPlugin, "org.shadoware.xinx.IContentViewParserPlugin/1.0")
Q_DECLARE_INTERFACE(IFilePlugin, "org.shadoware.xinx.IFilePlugin/1.0");

#endif /*_FILES_INTERFACES_H_*/
