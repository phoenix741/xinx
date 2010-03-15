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

#ifndef XINXCONFIG_H
#define XINXCONFIG_H
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <core/xinxcore.h>
#include <core/appsettings.h>
#include <core/exceptions.h>

// Qt header
#include <QPointer>

class XinxFormatFactory;
class XinxLanguageFactory;
class XinxFormatScheme;
class QWidget;

class LIBEXPORT ToolsNotDefinedException : public XinxException
{
public:
	ToolsNotDefinedException(const QString & tool);
};

/*!
 * Represente the configuration of XINX. The configuration of unique for a
 * unique process.
 */
class LIBEXPORT XINXConfig : public QObject, public AppSettings
{
	Q_OBJECT
public:
	/*! Create a configuration by assignation */
	XINXConfig(const XINXConfig & origine);
	/*! Create an empty configuration */
	XINXConfig();
	/*! Destroy the configuration object */
	virtual ~XINXConfig();

	/*! Self create a XINX configuration file if necessary */
	static XINXConfig * self();

	//! Get a created format scheme
	XinxFormatScheme * scheme(const QString & highlighter);

	/*!
	 * Add a new format scheme to XINX
	 * \param id the id to use for the scheme
	 * \param scheme the added scheme
	 */
	void addFormatScheme(const QString & id, XinxFormatScheme * scheme);

	/*! Return the language factory (from QCodeEdit library) for the definition in config */
	XinxLanguageFactory * languageFactory();

	/*! Load the configuration from the QSettings (Registry, INI File, ...) */
	virtual void load();
	/*! Save the configuration to the QSettings (Registry, INI File, ...) */
	virtual void save();

	/*!
	 * Get the tool name from the configuration, and if the tool doesn't exist or
	 * is not defined a dialog box to ask the user to define the tool.
	 * \param tool The tool to use (cvs, svn, diff, ...)
	 * \param showDialog Show a dialog box if the tool is not defined (else throw an exception)
	 * \param parentWindow The parent windows used to show the dialog.
	 * \throw ToolsNotDefinedException
	 */
	QString getTools(const QString & tool, bool showDialog = true, QWidget * parentWindow = 0);
	/*!
	 * Add a new tool in the configuration with a default value.
	 * \param tool The tool to add (cvs, svn, diff, ...).
	 * \param defaultValue The default value to use, if not already defined.
	 */
	void addDefaultTool(const QString & tool, const QString & defaultValue);

	/*! Assign a configuration to another */
	XINXConfig & operator=(const XINXConfig& p);
signals:
	/*! Signal emited when the configuration is changed */
	void changed();
public slots:
	//! Update the list of format (can't delete existing format)
	void updateFormatsSchemeFromConfig();
	//! Put format to config
	void putFormatsSchemeToConfig();

protected:
	virtual struct_globals getDefaultGlobals();
	virtual struct_editor getDefaultEditor();
private:
	static XINXConfig * s_self;
	QPointer<XinxLanguageFactory> m_languages;
	QHash<QString,XinxFormatScheme*> m_formatScheme;
};

#endif
