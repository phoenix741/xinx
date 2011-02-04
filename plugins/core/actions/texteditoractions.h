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

#ifndef _TEXTEDITORACTIONS_H_
#define _TEXTEDITORACTIONS_H_
#pragma once

// Xinx header
#include <actions/actioninterface.h>

class GotoLineAction : public XinxAction::Action
{
	Q_OBJECT
public:
	GotoLineAction();
	virtual bool isVisible() const;
protected slots:
	virtual void actionTriggered();
};

class DuplicateLinesAction : public XinxAction::Action
{
	Q_OBJECT
public:
	DuplicateLinesAction();
	virtual bool isVisible() const;
protected slots:
	virtual void actionTriggered();
};

class MoveLineUpAction : public XinxAction::Action
{
	Q_OBJECT
public:
	MoveLineUpAction();
	virtual bool isVisible() const;
protected slots:
	virtual void actionTriggered();
};

class MoveLineDownAction : public XinxAction::Action
{
	Q_OBJECT
public:
	MoveLineDownAction();
	virtual bool isVisible() const;
protected slots:
	virtual void actionTriggered();
};

class UpperSelectedTextAction : public XinxAction::Action
{
	Q_OBJECT
public:
	UpperSelectedTextAction();
	virtual bool isVisible() const;
	virtual bool isEnabled() const;
protected slots:
	virtual void actionTriggered();
};

class LowerSelectedTextAction : public XinxAction::Action
{
	Q_OBJECT
public:
	LowerSelectedTextAction();
	virtual bool isVisible() const;
	virtual bool isEnabled() const;
protected slots:
	virtual void actionTriggered();
};

class ShowSpecialCharsAction : public XinxAction::Action
{
	Q_OBJECT
public:
	ShowSpecialCharsAction();
	virtual bool isVisible() const;
protected slots:
	virtual void actionTriggered();
};

class IndentAction : public XinxAction::Action
{
	Q_OBJECT
public:
	IndentAction();
	virtual bool isVisible() const;
protected slots:
	virtual void actionTriggered();
};


class UnindentAction : public XinxAction::Action
{
	Q_OBJECT
public:
	UnindentAction();
	virtual bool isVisible() const;
protected slots:
	virtual void actionTriggered();
};


class AutoIndentAction : public XinxAction::Action
{
	Q_OBJECT
public:
	AutoIndentAction();
	virtual bool isVisible() const;
protected slots:
	virtual void actionTriggered();
};

class CompleteAction : public XinxAction::Action
{
	Q_OBJECT
public:
	CompleteAction();
	virtual bool isVisible() const;
protected slots:
	virtual void actionTriggered();
};


class HighlightWordAction : public XinxAction::Action
{
	Q_OBJECT
public:
	HighlightWordAction();
	virtual bool isVisible() const;
protected slots:
	virtual void actionTriggered();
};

#endif // _TEXTEDITORACTIONS_H_
