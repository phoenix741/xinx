/*
 XINX
 Copyright (C) 2007-2010 by Ulrich Van Den Hekke
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

#include "xmldefinitionmanager.h"
#include "definition_rootnode.h"
#include <project/xinxprojectproject.h>
#include "xmlcompletionparser.h"
#include <project/xinxprojectmanager.h>

namespace Core
{
namespace BaliseDefinition
{

XmlDefinitionManager::XmlDefinitionManager()
{
	if (_html_file.isNull())
	{
		_html_file = ContentView3::File::create("datas:core_html_definition.xml", XinxProject::Manager::self()->defaultProject());

		QSharedPointer<RootNode> rootNode = RootNode::create("HTML");
		_html_file->setRootNode(rootNode);

		if (QFileInfo(_html_file->filename()).exists())
		{
			XmlCompletionParser * parser = new XmlCompletionParser;
			parser->setFile(_html_file);
			static_cast<ContentView3::Parser*>(parser)->setDevice(new QFile(_html_file->filename()));
			XinxProject::Manager::self()->defaultProject()->cache()->addFileToCache(parser, false, ContentView3::Cache::PROJECT);
		}
	}
	if (_xsl_file.isNull())
	{
		_xsl_file = ContentView3::File::create("datas:core_xslt_definition.xml", XinxProject::Manager::self()->defaultProject());

		QSharedPointer<RootNode> rootNode = RootNode::create("XSLT");
		_xsl_file->setRootNode(rootNode);

		if (QFileInfo(_xsl_file->filename()).exists())
		{
			XmlCompletionParser * parser = new XmlCompletionParser;
			parser->setFile(_xsl_file);
			static_cast<ContentView3::Parser*>(parser)->setDevice(new QFile(_xsl_file->filename()));
			XinxProject::Manager::self()->defaultProject()->cache()->addFileToCache(parser, false, ContentView3::Cache::PROJECT);
		}
	}
}

ContentView3::FilePtr XmlDefinitionManager::getHtmlFile() const
{
	return _html_file;
}

ContentView3::FilePtr XmlDefinitionManager::getXslFile() const
{
	return _xsl_file;
}

QSharedPointer<Core::BaliseDefinition::RootNode> XmlDefinitionManager::getHtmlRootNode() const
{
	return _html_file->rootNode().staticCast<Core::BaliseDefinition::RootNode>();
}

QSharedPointer<Core::BaliseDefinition::RootNode> XmlDefinitionManager::getXslRootNode() const
{
	return _xsl_file->rootNode().staticCast<Core::BaliseDefinition::RootNode>();
}

} // namespace BaliseDefinition
} // namespace Core
