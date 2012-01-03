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
#include <QDirIterator>
#include <plugins/xinxpluginsloader.h>
#include <project/iconprojectprovider.h>
#include <project/externalfileresolver.h>
#include <editors/filetypepool.h>
#include <codecompletion/item.h>
#include <codecompletion/iteminterface.h>

namespace Core
{

namespace Stylesheet
{

/* ImportItemModelFactory */

ImportItemModelFactory::ImportItemModelFactory() : _provider(new IconProjectProvider)
{

}

ImportItemModelFactory::~ImportItemModelFactory()
{

}

void ImportItemModelFactory::addNode(const QString & path)
{
	const QFileInfo fileInfo = QFileInfo(path);
	const QString pathPart = fileInfo.isDir() ? fileInfo.absoluteFilePath() : fileInfo.absolutePath();
	const QString filePart = fileInfo.isDir() ? QString() : fileInfo.fileName();

	qDebug() << path;
	qDebug() << pathPart;
	qDebug() << filePart;

	QDirIterator iterator(pathPart, XinxPluginsLoader::self()->fileTypePool()->managedFilters(), QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Files);
	while (iterator.hasNext())
	{
		iterator.next();
		const QFileInfo information = iterator.fileInfo();
		const QString iteratorFilename = information.fileName();

		if (iteratorFilename.startsWith(filePart))
		{
			CodeCompletion::Item * item = new CodeCompletion::Item;
			if (information.isDir())
			{
				item->setIcon(QIcon(":/images/folder.png"));
				item->setCompletionType(tr("Directory"));
			}
			else
			{
				item->setIcon(_provider->icon(information.absoluteFilePath()));
				item->setCompletionType(tr("File"));
			}
			item->setText(iteratorFilename);
			item->setCompletionText(iteratorFilename);
			item->setKeyString(iteratorFilename);
			item->setContextType(tr("Import"));

			itemInterface()->addItem(item);
		}
	}
}

void ImportItemModelFactory::generate()
{
	Core::BaliseDefinition::XmlContextType * c = dynamic_cast<Core::BaliseDefinition::XmlContextType*>(context().context(XML_CONTEXT_TYPE));
	if (c && c->position() == Core::BaliseDefinition::XmlContextType::ATTRIBUTE_CONTENT)
	{
		if (c->balise().baliseName() == "import" && c->attributeName() == "href")
		{
			const QString prefix = c->balise().attributes().value(c->attributeName());

			ContentView3::FilePtr file = context().fileStrongRef();
			XinxProject::ProjectPtr project = file->project();

			if (project)
			{
				QStringList pathList = project->resolver()->resolvePath(prefix, project->resolver()->createContextInformation(file->filename()));
				foreach(QString path, pathList)
				{
					addNode(path);
				}
			}
		}
	}
}

}

}
