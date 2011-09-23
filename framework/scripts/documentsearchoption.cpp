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
#include "documentsearchoption.h"

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
