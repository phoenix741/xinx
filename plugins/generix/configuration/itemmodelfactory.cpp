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

namespace Configuration
{

/* AliasItem */

AliasItem::AliasItem(const QString & alias) : _alias(alias)
{
	setIcon(QIcon(":/images/folder.png"));
	setText(alias);
	setCompletionText(alias);
	setCompletionHelper("{%1}");
	setCompletionType(tr("Alias"));
	setKeyString(alias);
}

AliasItem::~AliasItem()
{

}

/* ItemModelFactory */

ItemModelFactory::ItemModelFactory()
{

}

ItemModelFactory::~ItemModelFactory()
{

}

void ItemModelFactory::generate()
{
	Core::BaliseDefinition::XmlContextType * c = dynamic_cast<Core::BaliseDefinition::XmlContextType*>(context().context(XML_CONTEXT_TYPE));
	if (c && c->position() == Core::BaliseDefinition::XmlContextType::ATTRIBUTE_CONTENT)
	{
		ContentView3::FilePtr file = context().fileStrongRef();
		XinxProject::ProjectPtr project = file->project();

		if (project)
		{
			QSharedPointer<GceConfiguration> configurationFile = ConfigurationManager::manager(project)->getInterface();
			if (configurationFile)
			{
				QStringList aliases =  configurationFile->aliasPolicy().keys();

				foreach(const QString & alias, aliases)
				{
					AliasItem * item = new AliasItem(alias);
					item->setContextType(tr("Aliases"));
					itemInterface()->addItem(item);
				}
			}
		}
	}
}

}

}
