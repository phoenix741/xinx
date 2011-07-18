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

#ifndef _STYLESHEETACTION_H_
#define _STYLESHEETACTION_H_
#pragma once

// Xinx header
#include <actions/actioninterface.h>

class XmlPresentationDockWidget;

class StyleSheetAction : public XinxAction::Action
{
	Q_OBJECT
public:
	StyleSheetAction();

	virtual bool isVisible() const;
	virtual bool isEnabled() const;
protected slots:
	virtual void actionTriggered();
private:
	XmlPresentationDockWidget * getXmlPresentationDockWidget() const;

	mutable XmlPresentationDockWidget * m_dock;
};

class RunXQueryAction : public XinxAction::Action
{
	Q_OBJECT
public:
	RunXQueryAction();

	virtual bool isVisible() const;
	virtual bool isEnabled() const;
protected slots:
	virtual void actionTriggered();
private:
	XmlPresentationDockWidget * getXmlPresentationDockWidget() const;

	mutable XmlPresentationDockWidget * m_dock;
};

class DataStreamAction : public XinxAction::ProjectAction
{
	Q_OBJECT
public:
	DataStreamAction();

	virtual bool isVisible() const;
	virtual bool isEnabled() const;
protected slots:
	virtual void actionTriggered();
private:
};

#endif // _STYLESHEETACTION_H_
