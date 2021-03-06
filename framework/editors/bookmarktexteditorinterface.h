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
#ifndef BOOKMARKTEXTEDITORINTERFACE_H
#define BOOKMARKTEXTEDITORINTERFACE_H

// Xinx header
#include <core/lib-config.h>

// Xinx header
#include <editors/bookmarkeditorinterface.h>

class XinxCodeEdit;
class TextFileEditor;

class LIBEXPORT BookmarkTextEditorInterface : public BookmarkEditorInterface
{
	Q_OBJECT
public:
	explicit BookmarkTextEditorInterface(TextFileEditor * parent);

	virtual QList<int> bookmarks() const;

	void setBookmark(int line, bool enabled);
	virtual void toogledBookmark();
	virtual void gotoBookmarkAt(int i);
	virtual QString bookmarkAt(int i);
	virtual int bookmarkCount();
	virtual bool previousBookmark();
	virtual bool nextBookmark();
	virtual void clearAllBookmark();

	void setTextEditor(XinxCodeEdit * textEdit);
public slots:
	void slotBookmarkToggled(int line, bool enabled);

private:
	XinxCodeEdit * m_view;
	TextFileEditor * m_textEdit;
};

#endif // BOOKMARKTEXTEDITORINTERFACE_H
