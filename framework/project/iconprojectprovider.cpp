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

// xinx header
#include <plugins/xinxpluginsloader.h>
#include "iconprojectprovider.h"
#include <plugins/interfaces/files.h>
#include <editors/filetypepool.h>

IconProjectProvider::IconProjectProvider() : QFileIconProvider()
{

}

IconProjectProvider::~IconProjectProvider()
{

}

QIcon IconProjectProvider::icon(const QFileInfo & info) const
{
	QList<IFileTypePluginPtr> plugin = XinxPluginsLoader::self()->fileTypePool()->matchedFileType(info.fileName());
	if (plugin.size() > 0)
	{
		QIcon icon = QIcon(plugin.at(0)->icon());
		if (! icon.isNull())
			return icon;
	}
	return QFileIconProvider::icon(info);
}

