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

#ifndef VALIDATIONACTION_H
#define VALIDATIONACTION_H
#pragma once

// Xinx header
#include <actions/actioninterface.h>

// Qt header
#include <QAbstractMessageHandler>

class XmlPresentationDockWidget;

class ValidationAction : public XinxAction::Action
{
	Q_OBJECT
public:
	ValidationAction(QAction * a, QObject * parent);
	ValidationAction(const QString & text, const QKeySequence & shortcut, QObject * parent);
	ValidationAction(const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent);

	virtual bool isActionVisible() const;
	virtual bool isActionEnabled() const;
	virtual bool isInToolBar() const;
protected slots:
	virtual void actionTriggered();
private:
};


#endif // VALIDATIONACTION_H
