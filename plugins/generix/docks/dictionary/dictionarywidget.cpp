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

/* DictionaryWidget */

DictionaryWidget::DictionaryWidget(QWidget * parent) : QTreeWidget(parent)
{
	setSortingEnabled(true);
}

void DictionaryWidget::loadDictionaries(const QString & prefix)
{
	 /* FIXME: Use the new method
	try
	{
		ContentView2::Project project(XinxProject::Manager::self()->project());

		QString whereClause =   "SELECT distinct display_name, name, icon "
								"FROM cv_node, cv_file "
								"WHERE cv_file.project_id=:project_id "
								"  AND cv_node.file_id=cv_file.id "
								"  AND cv_node.type = 'XslVariable' "
								"  AND cv_file.type = 'GNX_DICO' %1 "
								"ORDER BY lower(display_name) LIMIT 200";

		if (!prefix.isEmpty())
		{
			whereClause = whereClause.arg(
							  "  AND (cv_node.name like '%'||ifnull(:prefix1, '')||'%' "
							  "   OR EXISTS (SELECT 1 FROM cv_node cv_node2 "
							  " WHERE cv_node2.parent_id=cv_node.id "
							  "   AND cv_node2.name like '%'||ifnull(:prefix2, '')||'%')) "
						  );
		}
		else
		{
			whereClause = whereClause.arg("");
		}

		QSqlQuery query = ContentView2::Manager::self()->database()->prepare(whereClause);

		query.bindValue(":project_id", project.projectId());

		if (!prefix.isEmpty())
		{
			query.bindValue(":prefix1", prefix);
			query.bindValue(":prefix2", prefix);
		}
		bool result = query.exec();
		Q_ASSERT_X(result, "DictionaryWidget::exec", qPrintable(query.lastError().text()));

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
