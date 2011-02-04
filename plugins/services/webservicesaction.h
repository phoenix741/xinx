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

#ifndef WEBSERVICESACTION_H
#define WEBSERVICESACTION_H
#pragma once

// Xinx header
#include <actions/actioninterface.h>

class WebServicesRefreshAction : public XinxAction::Action
{
	Q_OBJECT
public:
	WebServicesRefreshAction();

	virtual bool isVisible() const;
protected slots:
	virtual void actionTriggered();
};

class WebServicesRunAction : public XinxAction::Action
{
	Q_OBJECT
public:
	WebServicesRunAction();

	virtual bool isVisible() const;
	virtual bool isEnabled() const;
protected slots:
	virtual void actionTriggered();
};

class WebServicesRunAllAction : public XinxAction::Action
{
	Q_OBJECT
public:
	WebServicesRunAllAction();

	virtual bool isVisible() const;
protected slots:
	virtual void actionTriggered();
};

#endif // WEBSERVICESACTION_H
