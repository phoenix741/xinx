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

#ifndef ACTIONMANAGER_P_H
#define ACTIONMANAGER_P_H

#include "actionmanager.h"
#include "actioninterface.h"
#include <plugins/xinxpluginsloader.h>
#include <plugins/xinxpluginelement.h>
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>

// Qt header
#include <QMultiHash>
#include <QSet>

namespace XinxAction
{

//! \private
class ActionManagerPrivate : public QObject
{
	Q_OBJECT
public:
	ActionManagerPrivate();
	~ActionManagerPrivate();

	ActionManager * _parent;

	QHash<QString,QString> _libelles;
	QMultiHash<QString,MenuItem*> _menus;
	QMultiHash<QString,MenuItem*> _toolBars;
	QList<MenuItem*> _popups;
	QList<MenuItem*> _directoryProjectPopup;

	QSet<MenuItem*> _items;

public slots:
	void updateSeparatorState(const QList<MenuItem*> & items);
	void currentEditorChanged(int index);
};

}

#endif // ACTIONMANAGER_P_H
