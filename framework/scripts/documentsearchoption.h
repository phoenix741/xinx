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

#pragma once
#ifndef _DOCUMENTSEARCHOPTION_H_
#define _DOCUMENTSEARCHOPTION_H_

// Xinx header
#include <core/lib-config.h>

// QCodeEdit header
#include <qdocumentsearch.h>

/* DocumentSearchOption */

class LIBEXPORT DocumentSearchOption : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool wholeWords READ isWholeWords WRITE setWholeWords)
	Q_PROPERTY(bool caseSensitive READ isCaseSensitive WRITE setCaseSensitive)
	Q_PROPERTY(bool regExp READ isRegExp WRITE setRegExp)
public:
	DocumentSearchOption(QDocumentSearch * search, QObject * parent = 0);
	virtual ~DocumentSearchOption();
public slots:
	void setWholeWords(bool value);
	bool isWholeWords() const;

	void setCaseSensitive(bool value);
	bool isCaseSensitive() const;

	void setRegExp(bool value);
	bool isRegExp() const;
private:
	bool m_wholeWords, m_caseSensitive, m_regexp;
	QDocumentSearch * m_search;
};

#endif /* _DOCUMENTSEARCHOPTION_H_ */
