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
#ifndef _DOCUMENTSEARCH_H_
#define _DOCUMENTSEARCH_H_

// Xinx header
#include <core/lib-config.h>
#include <editors/xinxcodeedit.h>

// QCodeEdit header
#include <qdocumentsearch.h>

class DocumentSearchOption;

/* DocumentSearch */

class LIBEXPORT DocumentSearch : public QObject
{
	Q_OBJECT
	Q_PROPERTY(DocumentSearchOption * options READ getOptions WRITE setOptions)
	Q_PROPERTY(QString searchText READ getSearchText WRITE setSearchText)
	Q_PROPERTY(QString replaceText READ getReplaceText WRITE setReplaceText)
public:
	DocumentSearch(XinxCodeEdit * editor);
	virtual ~DocumentSearch();

public slots:
	DocumentSearchOption * getOptions() const;
	void setOptions(DocumentSearchOption * value);

	void setSearchText(const QString & text);
	QString getSearchText() const;

	void setReplaceText(const QString & text);
	QString getReplaceText() const;

	bool next();
private:
	DocumentSearchOption * m_options;
	QDocumentSearch * m_search;
};

#endif /* _DOCUMENTSEARCH_H_ */
