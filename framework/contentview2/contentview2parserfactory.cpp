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

// Xinx header
#include "contentview2/contentview2parserfactory.h"
#include "plugins/xinxpluginsloader.h"

namespace ContentView2 {

ParserFactory::ParserFactory()
{
}

QString ParserFactory::getParserTypeByFilename( const QString & filename )
{
	if (! QFileInfo(filename).exists()) return QString();

	IFileTypePlugin * fileType = XinxPluginsLoader::self()->matchedFileType(filename);
	if( fileType )
	{
		QString parserType = fileType->parserType();
		if( ! parserType.isEmpty() )
		{
			return parserType;
		}
	}
	return QString();
}

Parser * ParserFactory::getParserByFilename( const QString & filename )
{
	QString parserType = getParserTypeByFilename( filename );
	if( ! parserType.isEmpty() )
	{
		return getParserByType( parserType );
	}
	return 0;
}

Parser * ParserFactory::getParserByType( const QString & type )
{
	Parser * parser = 0;

	foreach( XinxPluginElement * e, XinxPluginsLoader::self()->plugins() )
	{
		if( e->isActivated() )
		{
			IContentViewParserPlugin * plugin = qobject_cast<IContentViewParserPlugin*>( e->plugin() );
			if( plugin )
			{
				parser = plugin->createParser( type );
				if( parser )
				{
					break;
				}
			}
		}
	}

	return parser;
}

}
