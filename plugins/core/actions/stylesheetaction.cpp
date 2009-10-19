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
#include "stylesheetaction.h"
#include "editors/widgeteditor/html/htmlfileeditor.h"
#include "editors/widgeteditor/xsl/xslfileeditor.h"
#include <editors/editormanager.h>
#include "docks/datastream/xmlpresentationdockwidget.h"
#include "config/selfwebpluginsettings.h"

// Qt header
#include <QMessageBox>

/* StyleSheetAction */

StyleSheetAction::StyleSheetAction( QAction * a, QObject * parent ) : XinxAction::Action( a, parent ), m_dock( 0 ) {
}

StyleSheetAction::StyleSheetAction( const QString & text, const QKeySequence & shortcut, QObject * parent ) : XinxAction::Action( text, shortcut, parent ), m_dock( 0 ) {
}

StyleSheetAction::StyleSheetAction( const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent ) : XinxAction::Action( icon, text, shortcut, parent ), m_dock( 0 ) {
}

void StyleSheetAction::setXmlPresentationDockWidget( XmlPresentationDockWidget * value ) const {
	if( m_dock != value ) {
		if( m_dock != 0 )
			m_dock->disconnect( this );
		if( value  != 0 )
			connect( value, SIGNAL(filenameChanged(QString)), this, SLOT(updateActionState()) );
		m_dock = value;
	}
}

bool StyleSheetAction::isActionVisible() const {
	return ( SelfWebPluginSettings::self()->config().stylesheetParsing.viewer.type != "none" );
}

bool StyleSheetAction::isActionEnabled() const {
	if( ( SelfWebPluginSettings::self()->config().stylesheetParsing.viewer.type != "none" ) && EditorManager::self() ) {
		if( qobject_cast<StyleSheetEditor*>( EditorManager::self()->currentEditor() ) && ( SelfWebPluginSettings::self()->config().stylesheetParsing.parser.type != "none" ) ) {
			if( ! m_dock ) {
				setXmlPresentationDockWidget( StyleSheetEditor::xmlPresentationDockWidget() );
			}
			return m_dock && !m_dock->filename().isEmpty();
		} else if( qobject_cast<HtmlFileEditor*>( EditorManager::self()->currentEditor() ) ) {
			return true;
		}
	}
	return false;
}

bool StyleSheetAction::isInToolBar() const {
	return true;
}

void StyleSheetAction::actionTriggered() {
	Q_ASSERT( qobject_cast<StyleSheetEditor*>( EditorManager::self()->currentEditor() ) || qobject_cast<HtmlFileEditor*>( EditorManager::self()->currentEditor() ) );

	if( qobject_cast<StyleSheetEditor*>( EditorManager::self()->currentEditor() ) ) {
		Q_ASSERT( m_dock );
		qobject_cast<StyleSheetEditor*>( EditorManager::self()->currentEditor() )->launchStylesheetParsing( m_dock->filename() );
	}
	if( qobject_cast<HtmlFileEditor*>( EditorManager::self()->currentEditor() ) )
		qobject_cast<HtmlFileEditor*>( EditorManager::self()->currentEditor() )->showHtml();
}
