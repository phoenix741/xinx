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

// Xinx header
#include "contentview3/parserfactory.h"
#include <plugins/xinxpluginsloader.h>
#include <plugins/xinxpluginelement.h>
#include <plugins/interfaces/files.h>
#include <editors/filetypepool.h>

// Qt header
#include <QDebug>

namespace ContentView3
{

/*!
 * \ingroup ContentView3
 * \class ParserFactory
 * \since 0.9.1.0
 *
 * \brief This class is used to create a parser for the type or file name selected.
 *
 * This class use the plugin system to create the parser from the file name or from
 * the type of parser to create.
 */

/*!
 * \brief Get the parser type from the file name.
 *
 * Search in the XinxPluginsLoader::matchedFileType() the file who matchi \p filename.
 * For this file, the method get the parser type by the method IFileTypePlugin::parserType()
 */
QString ParserFactory::getParserTypeByFilename(const QString & filename)
{
	if (! QFileInfo(filename).exists()) return QString();

	QStringList parserTypes;
	QList<IFileTypePluginPtr> fileTypes = FileTypePool::self()->matchedFileType(filename);
	foreach(IFileTypePluginPtr fileType, fileTypes)
	{
		QString parserType = fileType->parserType();
		if (! parserType.isEmpty())
		{
			parserTypes << parserType;
		}
	}

	if (parserTypes.size() > 1)
	{
		qWarning() << tr("Ambigous parser for file %1").arg(filename);
	}

	if (parserTypes.size() > 0)
		return parserTypes.at(0);

	return QString();
}

/*!
 * \brief Get the parser for the file name.
 *
 * \sa getParserTypeByFilename()
 */
Parser * ParserFactory::getParserByFilename(const QString & filename)
{
	QString parserType = getParserTypeByFilename(filename);
	if (! parserType.isEmpty())
	{
		return getParserByType(parserType);
	}
	return 0;
}

/*!
 * \brief Get the parser from the type.
 *
 * This method create a parser for the type passed in parameter. To create
 * the rigth parser, this method search in all IContentViewParserPlugin the
 * rigth plugin and create the parser.
 */
Parser * ParserFactory::getParserByType(const QString & type)
{
	Parser * parser = 0;

	foreach(XinxPluginElement * e, XinxPluginsLoader::self()->plugins())
	{
		if (e->isActivated())
		{
			IContentViewParserPlugin * plugin = qobject_cast<IContentViewParserPlugin*>(e->plugin());
			if (plugin)
			{
				parser = plugin->createContentParser(type);
				if (parser)
				{
					break;
				}
			}
		}
	}

	return parser;
}

}
