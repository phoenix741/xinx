/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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
#include "snipetcompletionparser.h"
#include "snipetmanager.h"
#include "contentview2/contentview2node.h"
#include "contentview2/contentview2cache.h"
#include "contentview2/contentview2manager.h"
#include "core/xinxconfig.h"
#include "project/xinxproject.h"

// Qt header
#include <QSqlQuery>
#include <QtConcurrentRun>
#include <QSqlError>
#include <QVariant>

/* SnipetCompletionParser */

SnipetCompletionParser::SnipetCompletionParser() : ContentView2::Parser()
{

}

SnipetCompletionParser::~SnipetCompletionParser()
{

}

void SnipetCompletionParser::setFilename(const QString & )
{

}

void SnipetCompletionParser::load()
{
	clearNodes(rootNode());

	QSqlQuery selectQuery("SELECT id, icon, shortcut, name FROM snipets WHERE auto>=:auto", SnipetManager::self()->database());
	selectQuery.bindValue(":auto", true);

	bool result = selectQuery.exec();
	Q_ASSERT(result);
	if (! result)
	{
		qWarning(qPrintable(selectQuery.lastError().text()));
		return;
	}

	while (selectQuery.next())
	{
		const QString icon = selectQuery.value(1).toString();
		const QString shortcut = selectQuery.value(2).toString();
		const QString name = selectQuery.value(3).toString();

		ContentView2::Node node;
		node.setLine(-1);
		node.setFileId(rootNode().fileId());
		node.setData(shortcut, ContentView2::Node::NODE_NAME);
		node.setData("Snipet",  ContentView2::Node::NODE_TYPE);
		node.setData(icon.isEmpty() ? ":/images/help-hint.png" : icon,  ContentView2::Node::NODE_ICON);
		node.setData(shortcut,  ContentView2::Node::NODE_DISPLAY_NAME);

		attachNode(rootNode(), node);
	}

}

