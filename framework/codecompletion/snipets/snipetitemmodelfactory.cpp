/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
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

// Xinx header
#include "snipetitemmodelfactory.h"
#include <snipets/snipetmanager.h>
#include <codecompletion/model.h>
#include "snipetitem.h"
#include "snipetcontexttype.h"
#include <codecompletion/iteminterface.h>

// Qt header
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

namespace CodeCompletion
{

/* SnipetItemModelFactory */

SnipetItemModelFactory::SnipetItemModelFactory()
{

}

SnipetItemModelFactory::~SnipetItemModelFactory()
{

}

void SnipetItemModelFactory::generate()
{
	const QString context_type = tr("Snipet");

	SnipetContextType * c = dynamic_cast<SnipetContextType*>(context().context(SNIPET_CONTEXT_TYPE));
	if (c && c->loadSnipet())
	{
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

			if (shortcut.startsWith(context().prefix(), Qt::CaseInsensitive))
			{
				SnipetItem * item = new SnipetItem(shortcut, name);
				if (icon.isEmpty())
				{
					item->setIcon(QIcon(":/images/help-hint.png"));
				}
				else
				{
					item->setIcon(QIcon(icon));
				}

				item->setContextType(context_type);

				itemInterface()->addItem(item);
			}
		}
	}
}

}
