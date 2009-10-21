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

Action::Action( QAction * a, QObject * parent ) : QObject( parent ), m_action( a ) {
	updateActionState();
	connect( m_action, SIGNAL(triggered()), this, SLOT(actionTriggered()) );
}

Action::Action( const QString & text, const QKeySequence & shortcut, QObject * parent ) : QObject( parent ) {
	m_action = new QAction( text, this );
	m_action->setShortcut( shortcut );
	updateActionState();
	connect( m_action, SIGNAL(triggered()), this, SLOT(actionTriggered()) );
}

Action::Action( const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent ) : QObject( parent ) {
	m_action = new QAction( icon, text, this );
	m_action->setShortcut( shortcut );
	updateActionState();
	connect( m_action, SIGNAL(triggered()), this, SLOT(actionTriggered()) );
}

Action::~Action() {
}

QAction * Action::action() const {
	return m_action;
}

bool Action::isActionVisible() const {
	return true;
}

bool Action::isActionEnabled() const {
	return true;
}

bool Action::isInToolBar() const {
	return true;
}

bool Action::isInPopupMenu() const {
	return true;
}

void Action::updateActionState() {
	m_action->setVisible( isActionVisible() );
	m_action->setEnabled( isActionEnabled() );
}

void Action::actionTriggered() {
}

/* ActionList */

ActionList::ActionList( const QString & menu ) : m_menu( menu ), m_menuId( menu ) {
}

ActionList::ActionList( const QString & menu, const QString & id ) : m_menu( menu ), m_menuId( id ) {
}

const QString & ActionList::menu() const {
	return m_menu;
}

const QString & ActionList::menuId() const {
	return m_menuId;
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

