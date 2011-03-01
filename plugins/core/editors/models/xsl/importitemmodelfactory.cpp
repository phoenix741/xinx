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

#include "importitemmodelfactory.h"
#include <editors/models/html_xsl_base/xmlcontexttype.h>
#include <codecompletion/model.h>
#include <contentview3/definitions.h>
#include <contentview3/file.h>
#include <contentview3/filenode.h>
#include <contentview3/cache.h>
#include <project/xinxprojectproject.h>
#include "stylesheet_templatenode.h"
#include "stylesheet_paramnode.h"
#include "stylesheet_variablenode.h"
#include "editors/models/html_xsl_base/xmlcontexttype.h"
#include <qdocumentcursor.h>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>

namespace Core
{

namespace Stylesheet
{

/* ImportItemModelFactory */

ImportItemModelFactory::ImportItemModelFactory()
{

}

ImportItemModelFactory::~ImportItemModelFactory()
{

}

void ItemModelFactory::addNode(const QString & path)
{

}

void ItemModelFactory::generate()
{
	Core::BaliseDefinition::XmlContextType * c = dynamic_cast<Core::BaliseDefinition::XmlContextType*>(context().context(XML_CONTEXT_TYPE));
	if (c && c->position() == Core::BaliseDefinition::XmlContextType::ATTRIBUTE_CONTENT)
	{
		ContentView3::FilePtr file = context().fileStrongRef();
		XinxProject::ProjectPtr project = file->project();

		addNode(QFileInfo(file->filename()).absoluteFilePath ());

		if (project)
		{
			addNode(project->projectPath());
		}
	}
}

}

}
