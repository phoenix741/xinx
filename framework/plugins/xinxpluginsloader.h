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
#ifndef XINXPLUGINSLOADER_H_
#define XINXPLUGINSLOADER_H_

// Xinx header
#include <core/lib-config.h>
#include <core/xinxcore.h>

// Qt header
#include <QHash>
#include <QMap>
#include <QString>
#include <QDir>
#include <QObject>
#include <QIcon>
#include <QPair>

class RCS;
class XinxPluginElement;
class IFileTypePlugin;
class XinxFormatScheme;
class XINXConfig;

/*! The class XINX Plugins Loader is used to load all plugins and proposed some facilities method. */
class LIBEXPORT XinxPluginsLoader : public QObject
{
	Q_OBJECT
public:
	~XinxPluginsLoader();

	/*! Load the plugins */
	void loadPlugins();
	/*! List all the loaded plugins */
	QList<XinxPluginElement*> plugins() const;
	/*! Plugin of name \e name. */
	XinxPluginElement * plugin(const QString & name);

	/*! Return all the file type knew by XINX. */
	QList<IFileTypePlugin*> fileTypes() const;
	/*! Return the file type with the name \p name */
	IFileTypePlugin * fileType(const QString & name) const;
	/*! Search the file type for the corresponding filename  */
	QList<IFileTypePlugin*> matchedFileType(const QString & filename) const;
	/*! Return the filter for a given file type */
	static QString fileTypeFilter(IFileTypePlugin * fileType);
	//! Return a list of filter that can be used open dialog box
	QStringList openDialogBoxFilters() const;
	//! Return a list of filter
	QStringList managedFilters() const;

	//! Return the exemple of an highlighter
	QString exampleOfHighlighter(const QString & name) const;
	//! Create a format scheme for the given highlighter
	XinxFormatScheme * scheme(const QString & highlighter, XINXConfig * config);

	/*! Create a plugin loader object */
	static XinxPluginsLoader * self();
private:
	XinxPluginsLoader();

	void addPlugin(QObject * plugin, bool staticLoaded = false);
	QString allManagedFileFilter() const;

	QMap<QString, XinxPluginElement*> m_plugins;

	static XinxPluginsLoader * s_self;
};

#endif /*XINXPLUGINSLOADER_H_*/
