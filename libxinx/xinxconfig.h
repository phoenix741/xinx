/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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

#ifndef XINXCONFIG_H
#define XINXCONFIG_H

// Xinx header
#include "xinxcore.h"
#include "appsettings.h"
#include "exceptions.h"

// Qt header
#include <QPointer>

class XinxFormatFactory;
class XinxLanguageFactory;
class QWidget;

class ToolsNotDefinedException : public XinxException {
public:
	ToolsNotDefinedException( const QString & tool );
};

/*!
 * Represente the configuration of XINX. The configuration of unique for a
 * unique process.
 */
class XINXConfig : public QObject, public AppSettings {
	Q_OBJECT
public:
	/*! Create a configuration by assignation */
	XINXConfig( const XINXConfig & origine );
	/*! Create an empty configuration */
	XINXConfig();
	/*! Destroy the configuration object */
	virtual ~XINXConfig();

	/*! Self create a XINX configuration file if necessary */
	static XINXConfig * self();

	/*! Return the format factory (from QCodeEdit library) for the format in config */
	XinxFormatFactory * formatFactory();

	/*! Return the language factory (from QCodeEdit library) for the definition in config */
	XinxLanguageFactory * languageFactory();

	/*! Load the configuration from the QSettings (Registry, INI File, ...) */
	virtual void load();
	/*! Save the configuration to the QSettings (Registry, INI File, ...) */
	virtual void save();

	/*! Set the search path of data files. The Search path is accessible with datas:filename.xml */
	void setXinxDataFiles( const QString & path );

	/*!
	 * Get the tool name from the configuration, and if the tool doesn't exist or
	 * is not defined a dialog box to ask the user to define the tool.
	 * \param tool The tool to use (cvs, svn, diff, ...)
	 * \param showDialog Show a dialog box if the tool is not defined (else throw an exception)
	 * \param parentWindow The parent windows used to show the dialog.
	 * \throw ToolsNotDefinedException
	 */
	QString getTools( const QString & tool, bool showDialog = true, QWidget * parentWindow = 0 );
	/*!
	 * Add a new tool in the configuration with a default value.
	 * \param tool The tool to add (cvs, svn, diff, ...).
	 * \param defaultValue The default value to use, if not already defined.
	 */
	void addDefaultTool( const QString & tool, const QString & defaultValue );

	/*! Search the file type for the corresponding filename  */
	struct_extentions matchedFileType( const QString & filename );

	/*! Assign a configuration to another */
	XINXConfig & operator=( const XINXConfig& p );
signals:
	/*! Signal emited when the configuration is changed */
	void changed();
protected:
	virtual struct_globals getDefaultGlobals();
	virtual struct_editor getDefaultEditor();
private:
	static XINXConfig * s_self;
	QPointer<XinxFormatFactory> m_formats;
	QPointer<XinxLanguageFactory> m_languages;
};

#endif
