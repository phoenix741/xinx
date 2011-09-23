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
#include "documentsearch.h"
#include "documentsearchoption.h"

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
