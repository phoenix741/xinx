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
#include "documentsearch.h"

/* DocumentSearchOption */

DocumentSearchOption::DocumentSearchOption( QDocumentSearch::Options options, QObject * parent ) : QObject( parent ) {
	m_wholeWords = options.testFlag( QDocumentSearch::WholeWords );
	m_caseSensitive = options.testFlag( QDocumentSearch::CaseSensitive );
	m_regexp = options.testFlag( QDocumentSearch::RegExp );
}

DocumentSearchOption::~DocumentSearchOption() {
}

void DocumentSearchOption::setWholeWords( bool value ) {
	m_wholeWords = value;
}

bool DocumentSearchOption::isWholeWords() const {
	return m_wholeWords;
}

void DocumentSearchOption::setCaseSensitive( bool value ) {
	m_caseSensitive = value;
}

bool DocumentSearchOption::isCaseSensitive() const {
	return m_caseSensitive;
}

void DocumentSearchOption::setRegExp( bool value ) {
	m_regexp = value;
}

bool DocumentSearchOption::isRegExp() const {
	return m_regexp;
}

/* DocumentSearch */

DocumentSearch::DocumentSearch( XinxCodeEdit * editor ) {
	m_search  = new QDocumentSearch( editor->editor(), QString(), QDocumentSearch::Silent );
	m_options = new DocumentSearchOption( m_search->options(), this );
	m_cursor  = QDocumentCursor( editor->document() );
}

DocumentSearch::~DocumentSearch() {
	delete m_search;
}

DocumentSearchOption * DocumentSearch::getOptions() const {
	return m_options;
}

void DocumentSearch::setOptions( DocumentSearchOption * value ) {
	m_options = value;
}

QString DocumentSearch::searchText( const QString & text ) {
	m_search->setCursor( m_cursor );
	m_search->setOption( QDocumentSearch::WholeWords, m_options->isWholeWords() );
	m_search->setOption( QDocumentSearch::CaseSensitive, m_options->isCaseSensitive() );
	m_search->setOption( QDocumentSearch::RegExp, m_options->isRegExp() );

	m_search->setSearchText( text );
	m_search->next( false );

	m_cursor = m_search->cursor();
	if( m_cursor.isValid() )
		return m_cursor.selectedText();

	return QString();
}

void DocumentSearch::replaceSelection( const QString & text ) {
	if( ! m_cursor.isValid() ) return;
	m_search->setCursor( m_cursor.selectionStart() );
	m_search->setReplaceText( text );
	m_search->setOption( QDocumentSearch::Replace, true );
	m_search->next( false );
	m_search->setOption( QDocumentSearch::Replace, false );
}
