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

#include "itemmodelfactory.h"
#include <editors/models/html_xsl_base/xmlcontexttype.h>
#include <editors/models/xsl/itemmodelfactory.h>
#include <QStringList>
#include "configuration/configurationmanager.h"
#include <contentview3/file.h>
#include <contentview3/filenode.h>

namespace Generix
{

namespace Dictionary
{

/* ItemModelFactory */

ItemModelFactory::ItemModelFactory()
{

}

ItemModelFactory::~ItemModelFactory()
{

}

void ItemModelFactory::addNode(const QString & contextType, ContentView3::NodePtr node)
{
	foreach(ContentView3::NodePtr child, node->childs())
	{
		if (child->name().startsWith(context().prefix (), Qt::CaseInsensitive))
		{
			Core::Stylesheet::XslNodeItem * item = new Core::Stylesheet::XslNodeItem(child);
			item->setContextType(contextType);
			itemInterface()->addItem(item);
		}
	}
}

void ItemModelFactory::generate()
{
	// Do nothing if the prefix is not LBL. + 1 car.
	if (context ().prefix ().length () < 5) return;

	Core::BaliseDefinition::XmlContextType * c = dynamic_cast<Core::BaliseDefinition::XmlContextType*> (context().context(XML_CONTEXT_TYPE));
	if (c && c->position () == Core::BaliseDefinition::XmlContextType::ATTRIBUTE_CONTENT)
	{
		ContentView3::FilePtr file = context().fileStrongRef ();
		XinxProject::Project * project = file->project();

		if (project)
		{
			GceInterface* configurationFile = ConfigurationManager::self()->getInterfaceOfProject(project);
			if (configurationFile)
			{
				QStringList dictionaries =  configurationFile->dictionnaries();

				foreach(const QString & dictionarie, dictionaries)
				{
					ContentView3::FilePtr dicoFilePtr = project->cache ()->cachedFile (dictionarie);
					ContentView3::FileNodePtr rootNode = dicoFilePtr->rootNode();

					addNode (QDir(project->projectPath ()).relativeFilePath (dicoFilePtr->filename()), rootNode);
				}
			}
		}
	}
}

}

}
