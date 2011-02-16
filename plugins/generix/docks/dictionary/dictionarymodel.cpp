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

#include "dictionarymodel.h"
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>
#include <configuration/configurationmanager.h>
#include <contentview3/filenode.h>

/* DictionaryModel */
#include "dictionary_labelnode.h"

DictionaryModel::DictionaryModel(QObject* parent): QStandardItemModel(parent)
{
	connect(EditorManager::self(), SIGNAL(currentChanged(int)), this, SLOT(textEditorChanged(int)));
}

DictionaryModel::~DictionaryModel()
{

}

void DictionaryModel::textEditorChanged(int index)
{
	XinxProject::ProjectPtr project;
	if ((index >= 0) && (index < EditorManager::self()->editorsCount()))
	{
		project = EditorManager::self()->editor(index)->project();
	}

	if (project.data() != _project.data())
	{
		_project = project.toWeakRef();
		loadDictionaries(_prefix);
	}
}

void DictionaryModel::loadDictionaries(const QString & prefix)
{
	XinxProject::ProjectPtr project = _project.toStrongRef();

	clear();

	emit changed();

	if (! project) return;
	if (! ConfigurationManager::manager(project)) return;
	if (! ConfigurationManager::manager(project)->getInterface()) return;
	if (prefix.isEmpty())
	{
		appendRow(new QStandardItem(tr("Please type a prefix to list labels.")));
		return;
	}

	QRegExp regExpPrefix(prefix);
	regExpPrefix.setCaseSensitivity(Qt::CaseInsensitive);

	QHash<QString,QStandardItem*> _items;

	QStringList dictionaries = ConfigurationManager::manager(project)->getInterface()->dictionnaries();
	foreach(const QString & dictionary, dictionaries)
	{
		ContentView3::NodePtr nodePtr = project->cache()->cachedFile(dictionary)->rootNode();

		foreach(ContentView3::NodePtr childNodePtr, nodePtr->childs())
		{
			QStandardItem * label;
			bool forceChild = false;
			if (_items.contains(childNodePtr->name()))
			{
				label = _items.value(childNodePtr->name());
				forceChild = true;
			}
			else
			{
				bool skipLabel = false;
				if (! childNodePtr->name().contains(regExpPrefix))
				{
					skipLabel = true;
					foreach(ContentView3::NodePtr childChildPtr, childNodePtr->childs())
					{
						QSharedPointer<Generix::Dictionary::LabelNode> labelPtr = childChildPtr.dynamicCast<Generix::Dictionary::LabelNode>();
						if (labelPtr->value().contains(regExpPrefix))
						{
							skipLabel = false;
						}
					}
					if (skipLabel)
					{
						continue;
					}
				}

				label = new QStandardItem;
				label->setText(childNodePtr->name());
				label->setIcon(QIcon(childNodePtr->icon()));

				appendRow(label);
				_items.insert(childNodePtr->name(), label);
			}

			QStandardItem * langue = 0;
			QStandardItem * context = 0;

			foreach(ContentView3::NodePtr childChildPtr, childNodePtr->childs())
			{
				QSharedPointer<Generix::Dictionary::LabelNode> labelPtr = childChildPtr.dynamicCast<Generix::Dictionary::LabelNode>();

				if (!labelPtr->value().contains(regExpPrefix) && !forceChild)
				{
					continue;
				}

				if ((langue == 0) || (langue->text() != labelPtr->lang()))
				{
					if (labelPtr->lang().isEmpty())
					{
						langue = label;
					}
					else
					{
						langue = new QStandardItem;
						langue->setText(labelPtr->lang());
						if (labelPtr->lang() == "FRA")
						{
							langue->setIcon(QIcon(":/generix/images/france.png"));
						}
						else if (labelPtr->lang() == "ENG")
						{
							langue->setIcon(QIcon(":/generix/images/usa.png"));
						}
						else if (labelPtr->lang() == "ESP")
						{
							langue->setIcon(QIcon(":/generix/images/spain.png"));
						}
						else if (labelPtr->lang() == "POR")
						{
							langue->setIcon(QIcon(":/generix/images/portugal.png"));
						}
						else if (labelPtr->lang() == "DEU")
						{
							langue->setIcon(QIcon(":/generix/images/germany.png"));
						}
						else if (labelPtr->lang() == "ITA")
						{
							langue->setIcon(QIcon(":/generix/images/italy.png"));
						}
						label->appendRow(langue);
					}
				}

				if ((context == 0) || (context->text() != labelPtr->context()))
				{
					if (labelPtr->context().isEmpty())
					{
						context = langue;
					}
					else
					{
						context = new QStandardItem;
						context->setText(labelPtr->context());
						context->setIcon(QIcon(":/generix/images/unknown.png"));
						langue->appendRow(context);
					}
				}

				QStandardItem * item = new QStandardItem;
				item->setText(labelPtr->name());
				item->setIcon(QIcon(labelPtr->icon()));
				context->appendRow(item);
			}
		}
	}
	emit changed();
}
