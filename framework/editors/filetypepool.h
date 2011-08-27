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
#ifndef FILETYPEPOOL_H
#define FILETYPEPOOL_H

#include <core/lib-config.h>
#include <core/xinxsingleton.h>

#include <QScopedPointer>
#include <QSharedPointer>

class IFileTextPlugin;
class QFormatScheme;
class XINXConfig;
class PrivateFileTypePool;
class IFileTypePlugin;
class XinxFormatScheme;
class XinxPluginsLoader;

typedef QSharedPointer<IFileTypePlugin> IFileTypePluginPtr;
typedef QSharedPointer<IFileTextPlugin> IFileTextPluginPtr;

class LIBEXPORT FileTypePool : public QObject
{
	Q_OBJECT
public:
	FileTypePool(XinxPluginsLoader * loader);
	virtual ~FileTypePool();

	/*! Return all the file type knew by XINX. */
	QList<IFileTypePluginPtr> fileTypes() const;
	/*! Return the file type with the name \p name */
	IFileTypePluginPtr fileType(const QString & name) const;
	/*! Search the file type for the corresponding filename  */
	QList<IFileTypePluginPtr> matchedFileType(const QString & filename) const;

	/*! Return the filter for a given file type */
	static QString fileTypeFilter(IFileTypePluginPtr fileType);
	//! Return a list of filter that can be used open dialog box
	QStringList openDialogBoxFilters() const;
	//! Return a list of filter
	QStringList managedFilters() const;


	//! Return the exemple of an highlighter
	QString exampleOfHighlighter(const QString & name) const;
	//! Create a format scheme for the given highlighter
	XinxFormatScheme* scheme(const QString & highlighter) const;
public slots:
	void updateFileTypes();
private slots:
	void activatePlugin(const QString & name);
	void desactivatePlugin(const QString & name);
private:
	QString allManagedFileFilter() const;
	QScopedPointer<PrivateFileTypePool> d;
};

#endif // FILETYPEPOOL_H
