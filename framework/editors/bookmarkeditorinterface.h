/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

#pragma once
#ifndef BOOKMARKEDITORINTERFACE_H
#define BOOKMARKEDITORINTERFACE_H

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QObject>

class LIBEXPORT BookmarkEditorInterface : public QObject
{
	Q_OBJECT
public:
	BookmarkEditorInterface(QObject * parent);

	virtual QString bookmarkAt(int i) = 0;
	virtual int bookmarkCount() = 0;

public slots :
	virtual void toogledBookmark() = 0;
	virtual void firstBookmark();
	virtual bool previousBookmark() = 0;
	virtual bool nextBookmark() = 0;
	virtual void lastBookmark();
	virtual void gotoBookmarkAt(int i) = 0;
	virtual void clearAllBookmark() = 0;

signals:
	void bookmarkModified(int minValue, int maxValue);
};

#endif // BOOKMARKEDITORINTERFACE_H
