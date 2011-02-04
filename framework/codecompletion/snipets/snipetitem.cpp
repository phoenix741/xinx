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

#include "snipetitem.h"
#include <snipets/snipetmanager.h>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>

namespace CodeCompletion
{

/* PrivateSnipetItem */

class PrivateSnipetItem
{
public:
    QString _snipet;
};

/* SnipetItem */

SnipetItem::SnipetItem(const QString& snipet, const QString& name) : ActionItem(tr("Add the snipet \"%1\" to the editor").arg(snipet), snipet)
{
	d = new PrivateSnipetItem;
	d->_snipet = snipet;

	setCompletionType (tr("Snipet"));
}

SnipetItem::~SnipetItem()
{

}

void SnipetItem::execute(Context context, TextFileEditor * editor)
{
	QString result;
	if (SnipetManager::self()->callSnipet(d->_snipet, &result, editor->lastFileName(), qApp->activeWindow()))
	{
		editor->textEdit()->insertText(result);
	}
}

}
