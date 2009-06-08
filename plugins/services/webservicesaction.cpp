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

// Xinx Action
#include "webservicesaction.h"
#include <core/xinxproject.h>
#include "webservices.h"
#include "webserviceseditor.h"
#include <editors/editormanager.h>

/* WebServicesRefreshAction */

WebServicesRefreshAction::WebServicesRefreshAction( QAction * a, QObject * parent ) : XinxAction::Action( a, parent ) {
}

WebServicesRefreshAction::WebServicesRefreshAction( const QString & text, const QKeySequence & shortcut, QObject * parent ) : XinxAction::Action( text, shortcut, parent ) {
}

WebServicesRefreshAction::WebServicesRefreshAction( const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent ) : XinxAction::Action( icon, text, shortcut, parent ) {
}

bool WebServicesRefreshAction::isActionVisible() const {
	return XINXProjectManager::self()->project() && XINXProjectManager::self()->project()->options().testFlag( XinxProject::hasWebServices );
}

bool WebServicesRefreshAction::isActionEnabled() const {
	return XINXProjectManager::self()->project() && XINXProjectManager::self()->project()->options().testFlag( XinxProject::hasWebServices );
}

bool WebServicesRefreshAction::isInToolBar() const {
	return false;
}

void WebServicesRefreshAction::actionTriggered() {
	WebServicesManager::self()->updateWebServicesList();
}

/* WebServicesRunAction */

WebServicesRunAction::WebServicesRunAction( QAction * a, QObject * parent ) : XinxAction::Action( a, parent ) {
}

WebServicesRunAction::WebServicesRunAction( const QString & text, const QKeySequence & shortcut, QObject * parent ) : XinxAction::Action( text, shortcut, parent ) {
}

WebServicesRunAction::WebServicesRunAction( const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent ) : XinxAction::Action( icon, text, shortcut, parent ) {
}

bool WebServicesRunAction::isActionVisible() const {
	return XINXProjectManager::self()->project() && XINXProjectManager::self()->project()->options().testFlag( XinxProject::hasWebServices );
}

bool WebServicesRunAction::isActionEnabled() const {
	if( qobject_cast<WebServicesEditor*>( EditorManager::self()->currentEditor() ) ) {
		WebServicesEditor * editor = qobject_cast<WebServicesEditor*>( EditorManager::self()->currentEditor() );
		return editor->service() && editor->operation() && XINXProjectManager::self()->project() && XINXProjectManager::self()->project()->options().testFlag( XinxProject::hasWebServices );
	}
	return false;
}

bool WebServicesRunAction::isInToolBar() const {
	return true;
}

void WebServicesRunAction::actionTriggered() {
	WebServicesEditor * editor = qobject_cast<WebServicesEditor*>( EditorManager::self()->currentEditor() );
	Q_ASSERT( editor );

	editor->run();
}
