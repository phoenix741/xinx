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
#include "commentactions.h"
#include <editors/editormanager.h>
#include <editors/xinxcodeedit.h>

#include "editors/widgeteditor/css/cssfileeditor.h"
#include "editors/widgeteditor/html/htmlfileeditor.h"
#include "editors/widgeteditor/js/jsfileeditor.h"
#include "editors/widgeteditor/xml/xmlfileeditor.h"
#include "editors/widgeteditor/xsl/xslfileeditor.h"

// Qt header
#include <QDebug>

/* CommentAction */

CommentAction::CommentAction( QAction * a, QObject * parent ) : XinxAction::Action( a, parent ) {
	connect( dynamic_cast<QObject*>( EditorManager::self() ), SIGNAL(currentChanged(int)), this, SLOT(updateSignals()) );
}

CommentAction::CommentAction( const QString & text, const QKeySequence & shortcut, QObject * parent ) : XinxAction::Action( text, shortcut, parent ) {
	connect( dynamic_cast<QObject*>( EditorManager::self() ), SIGNAL(currentChanged(int)), this, SLOT(updateSignals()) );
}

CommentAction::CommentAction( const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent ) : XinxAction::Action( icon, text, shortcut, parent ) {
	connect( dynamic_cast<QObject*>( EditorManager::self() ), SIGNAL(currentChanged(int)), this, SLOT(updateSignals()) );
}

bool CommentAction::isActionVisible() const {
	bool result =
		EditorManager::self() && (
		qobject_cast<CSSFileEditor*>( EditorManager::self()->currentEditor() )  ||
		qobject_cast<HtmlFileEditor*>( EditorManager::self()->currentEditor() ) ||
		qobject_cast<JSFileEditor*>( EditorManager::self()->currentEditor() )   ||
		qobject_cast<XmlFileEditor*>( EditorManager::self()->currentEditor() )  ||
		qobject_cast<StyleSheetEditor*>( EditorManager::self()->currentEditor() ) );

	return result;
}

bool CommentAction::isActionEnabled() const {
	if( ! EditorManager::self() ) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>( EditorManager::self()->currentEditor() );
	if( ! editor ) return false;
	return editor->textEdit()->textCursor().hasSelection();
}

void CommentAction::actionTriggered() {
}

void CommentAction::updateSignals() {
	if( ! m_editor.isNull() ) {
		m_editor->disconnect( action() );
	}

	TextFileEditor * editor = qobject_cast<TextFileEditor*>( EditorManager::self()->currentEditor() );
	if( editor ) {
		m_editor = editor;
		connect( m_editor->textEdit(), SIGNAL(copyAvailable(bool)), action(), SLOT(setEnabled(bool)));
	}
}

/* UncommentAction */

UncommentAction::UncommentAction( QAction * a, QObject * parent ) : CommentAction( a, parent ) {
}

UncommentAction::UncommentAction( const QString & text, const QKeySequence & shortcut, QObject * parent ) : CommentAction( text, shortcut, parent ) {
}

UncommentAction::UncommentAction( const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent ) : CommentAction( icon, text, shortcut, parent ) {
}

void UncommentAction::actionTriggered() {
}

