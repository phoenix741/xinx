/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
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
#include "actioninterface.h"
#include <core/xinxcore.h>

using namespace XinxAction;

/* Action */

Action::Action( QAction * a ) : m_action( a ) {
}

const QAction * Action::action() {
	return m_action;
};

bool Action::isActionVisible() const {
	return true;
}

bool Action::isActionEnabled() const {
	return true;
}

bool Action::isInToolBar() const {
	return true;
}

void Action::updateActionState() {
	m_action->setEnabled( isActionEnabled() );
	m_action->setVisible( isActionVisible() );
}

void Action::actionTriggered() {
}

/* ActionList */

ActionList::ActionList( const QString & menu ) : m_menu( menu ) {
}

const QString & ActionList::menu() const {
	return m_menu;
}

void ActionList::updateActionsState() {
	foreach( MenuItem * item, *this ) {
		Action * action = dynamic_cast<Action*>( item );
		if( action )
			action->updateActionState();
	}
}

/* MenuList */

MenuList::MenuList() {
}

void MenuList::updateMenuState() {
	foreach( ActionList menu, *this ) {
		menu.updateActionsState();
	}
}

