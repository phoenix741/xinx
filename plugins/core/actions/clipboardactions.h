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

#ifndef CLIPBOARDACTIONS_H
#define CLIPBOARDACTIONS_H
#pragma once

// Xinx header
#include <actions/actioninterface.h>

class CutAction : public XinxAction::Action
{
	Q_OBJECT
public:
	CutAction();

	virtual bool isEnabled() const;
protected slots:
	virtual void actionTriggered();
private:
};

class CopyAction : public XinxAction::Action
{
	Q_OBJECT
public:
	CopyAction();

	virtual bool isEnabled() const;
protected slots:
	virtual void actionTriggered();
private:
};

class PasteAction : public XinxAction::Action
{
	Q_OBJECT
public:
	PasteAction();

	virtual bool isEnabled() const;
protected slots:
	virtual void actionTriggered();
private:
};

#endif // CLIPBOARDACTIONS_H
