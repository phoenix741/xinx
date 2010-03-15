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

DocumentSearchOption::DocumentSearchOption(QDocumentSearch * search, QObject * parent) : QObject(parent), m_search(search)
{
}

DocumentSearchOption::~DocumentSearchOption()
{
}

void DocumentSearchOption::setWholeWords(bool value)
{
	m_wholeWords = value;
	m_search->setOption(QDocumentSearch::WholeWords, value);
}

bool DocumentSearchOption::isWholeWords() const
{
	return m_wholeWords;
}

void DocumentSearchOption::setCaseSensitive(bool value)
{
	m_caseSensitive = value;
	m_search->setOption(QDocumentSearch::CaseSensitive, value);
}

bool DocumentSearchOption::isCaseSensitive() const
{
	return m_caseSensitive;
}

void DocumentSearchOption::setRegExp(bool value)
{
	m_regexp = value;
	m_search->setOption(QDocumentSearch::RegExp, value);
}

bool DocumentSearchOption::isRegExp() const
{
	return m_regexp;
}

/* DocumentSearch */

DocumentSearch::DocumentSearch(XinxCodeEdit * editor)
{
	m_search  = new QDocumentSearch(editor->editor(), QString(), QDocumentSearch::Silent);
	m_options = new DocumentSearchOption(m_search, this);
	m_search->setCursor(QDocumentCursor(editor->document()));
}

DocumentSearch::~DocumentSearch()
{
	delete m_search;
}

DocumentSearchOption * DocumentSearch::getOptions() const
{
	return m_options;
}

void DocumentSearch::setOptions(DocumentSearchOption * value)
{
	m_options = value;
}

void DocumentSearch::setSearchText(const QString & text)
{
	m_search->setSearchText(text);
}

QString DocumentSearch::getSearchText() const
{
	return m_search->searchText();
}

void DocumentSearch::setReplaceText(const QString & text)
{
	m_search->setReplaceText(text);
	m_search->setOption(QDocumentSearch::Replace, ! text.isEmpty());
}

QString DocumentSearch::getReplaceText() const
{
	return m_search->replaceText();
}

bool DocumentSearch::next()
{
	m_search->next(false);

	QDocumentCursor cursor = m_search->cursor();

	return cursor.isValid();
}
