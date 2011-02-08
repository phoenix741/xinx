/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2010 by Ulrich Van Den Hekke                              *
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
#include "dictionarywidget.h"
#include <configuration/configurationmanager.h>
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>
#include <contentview3/file.h>
#include <contentview3/filenode.h>

/* DictionaryWidget */

DictionaryWidget::DictionaryWidget(QWidget * parent) : QTreeWidget(parent), _project(0)
{
	setSortingEnabled(true);

	connect(EditorManager::self(), SIGNAL(currentChanged(int)), this, SLOT(textEditorChanged(int)));
}

void DictionaryWidget::textEditorChanged(int index)
{
	if ((index >= 0) && (index < EditorManager::self()->editorsCount()))
	{
		_project = EditorManager::self()->editor(index)->project();
	}
	else
	{
		_project = 0;
	}
	loadDictionaries(_prefix);
}

void DictionaryWidget::loadDictionaries(const QString & prefix)
{
	_prefix = prefix;

	clear();
	if (!_project) return;

	QStringList dictionaries = ConfigurationManager::manager(_project)->getInterface()->dictionnaries();
	foreach(const QString & dictionary, dictionaries)
	{
		ContentView3::NodePtr nodePtr = _project->cache()->cachedFile(dictionary)->rootNode();

		foreach(ContentView3::NodePtr childNodePtr, nodePtr->childs())
		{
			QTreeWidgetItem * label = new QTreeWidgetItem(this);
			label->setText(0, childNodePtr->name());
			label->setIcon(0, QIcon(childNodePtr->icon()));
		}
	}

	/* FIXME: Use the new method
	try
	{
		ContentView2::Project project(XinxProject::Manager::self()->project());

		QString whereClause =   "SELECT distinct display_name, name, icon "

		clear();
		while (query.next())
		{
			QTreeWidgetItem * label = new QTreeWidgetItem(this);
			label->setText(0, query.value(0).toString());
			label->setIcon(0, QIcon(query.value(2).toString()));

			QSqlQuery childQuery = ContentView2::Manager::self()->database()->prepare(
								"SELECT distinct cv_node2.property1, cv_node2.property2, cv_node2.name, cv_node2.icon "
								"FROM cv_file, cv_node, cv_node cv_node2 "
								"WHERE cv_file.project_id=:project_id "
								"  AND cv_node.file_id=cv_file.id "
								"  AND cv_node.type = 'XslVariable' "
								"  AND cv_node.name = :node_name "
								"  AND cv_file.type = 'GNX_DICO' "
								"  AND cv_node2.parent_id = cv_node.id "
								"  AND cv_node2.type = 'DICTIONARY_LABEL' "
								"ORDER BY cv_node2.property1, cv_node2.property2, lower(cv_node2.name) "
								);
			childQuery.bindValue(":project_id", project.projectId());
			childQuery.bindValue(":node_name", label->text(0));
			bool result = childQuery.exec();
			Q_ASSERT_X(result, "DictionaryWidget::exec", qPrintable(childQuery.lastError().text()));

			QTreeWidgetItem * langue = 0;
			QTreeWidgetItem * context = 0;

			while (childQuery.next())
			{
				const QString langueStr  = childQuery.value(0).toString();
				const QString contextStr = childQuery.value(1).toString();
				const QString nameStr    = childQuery.value(2).toString();
				const QString iconStr    = childQuery.value(3).toString();

				if ((langue == 0) || (langue->text(0) != langueStr))
				{
					if (langueStr.isEmpty())
					{
						langue = label;
					}
					else
					{
						langue = new QTreeWidgetItem(label);
						langue->setText(0, langueStr);
						if (langueStr == "FRA")
						{
							langue->setIcon(0, QIcon(":/generix/images/france.png"));
						}
						else if (langueStr == "ENG")
						{
							langue->setIcon(0, QIcon(":/generix/images/usa.png"));
						}
						else if (langueStr == "ESP")
						{
							langue->setIcon(0, QIcon(":/generix/images/spain.png"));
						}
						else if (langueStr == "POR")
						{
							langue->setIcon(0, QIcon(":/generix/images/portugal.png"));
						}
						else if (langueStr == "DEU")
						{
							langue->setIcon(0, QIcon(":/generix/images/germany.png"));
						}
						else if (langueStr == "ITA")
						{
							langue->setIcon(0, QIcon(":/generix/images/italy.png"));
						}
					}
				}

				if ((context == 0) || (context->text(0) != contextStr))
				{
					if (contextStr.isEmpty())
					{
						context = langue;
					}
					else
					{
						context = new QTreeWidgetItem(langue);
						context->setText(0, contextStr);
						context->setIcon(0, QIcon(":/generix/images/unknown.png"));
					}
				}

				QTreeWidgetItem * item = new QTreeWidgetItem(context);
				item->setText(0, nameStr);
				item->setIcon(0, QIcon(iconStr));
			}

			childQuery.finish ();
		}

		query.finish ();
	}
	catch (ContentView2::ProjectException e)
	{

	}
	*/
}
