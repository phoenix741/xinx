/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2006-2010 by Ulrich Van Den Hekke                         *
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

#ifndef BOOKMARKACTIONS_H
#define BOOKMARKACTIONS_H
#pragma once

// Xinx header
#include <actions/actioninterface.h>

class BookmarkAction : public XinxAction::Action
{
	Q_OBJECT
public:
	BookmarkAction();

	virtual bool isEnabled() const;
protected slots:
	virtual void actionTriggered();
private:
};

class NextBookmarkAction : public XinxAction::Action
{
	Q_OBJECT
public:
	NextBookmarkAction();

	virtual bool isEnabled() const;
protected slots:
	virtual void actionTriggered();
private:
};

class PreviousBookmarkAction : public XinxAction::Action
{
	Q_OBJECT
public:
	PreviousBookmarkAction();

	virtual bool isEnabled() const;
protected slots:
	virtual void actionTriggered();
private:
};

class ClearAllBookmarkAction : public XinxAction::Action
{
	Q_OBJECT
public:
	ClearAllBookmarkAction();

	virtual bool isEnabled() const;
protected slots:
	virtual void actionTriggered();
private:
};

#endif // BOOKMARKACTIONS_H
