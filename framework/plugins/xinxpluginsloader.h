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
#include <core/xinxsingleton.h>
#include <editors/filetypepool.h>
#include <codecompletion/pool.h>

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
class LIBEXPORT XinxPluginsLoader : public XinxLibSingleton<XinxPluginsLoader>
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

	/// Pool de code completion
	CodeCompletion::Pool * codeCompletionPool() const;
	/// Pool de file type
	FileTypePool * fileTypePool() const;
signals:
	void pluginActivated(const QString & name);
	void pluginDesactivated(const QString & name);
private slots:
	//! Update activated plugin
	void updateActivatedPlugin();
private:
	XinxPluginsLoader();

	void addPlugin(QObject * plugin, bool staticLoaded = false);

	QStringList _activatedPlugin;
	QScopedPointer<CodeCompletion::Pool> _codeCompletionPool;
	QScopedPointer<FileTypePool> _fileTypePool;
	QMap<QString, XinxPluginElement*> m_plugins;
	friend class XinxLibSingleton<XinxPluginsLoader>;
};

#endif /*XINXPLUGINSLOADER_H_*/
