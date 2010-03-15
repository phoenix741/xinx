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

#ifndef _COMMENTACTIONS_H_
#define _COMMENTACTIONS_H_
#pragma once

// Xinx header
#include <actions/actioninterface.h>

// Qt header
#include <QPointer>

class TextFileEditor;

class CommentAction : public XinxAction::Action
{
	Q_OBJECT
public:
	CommentAction(QAction * a, QObject * parent);
	CommentAction(const QString & text, const QKeySequence & shortcut, QObject * parent);
	CommentAction(const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent);

	virtual bool isInToolBar() const;
	virtual bool isActionVisible() const;
	virtual bool isActionEnabled() const;
protected slots:
	virtual void actionTriggered();
private slots:
	void updateSignals();
private:
	QPointer<TextFileEditor> m_editor;
};

class UncommentAction : public CommentAction
{
	Q_OBJECT
public:
	UncommentAction(QAction * a, QObject * parent);
	UncommentAction(const QString & text, const QKeySequence & shortcut, QObject * parent);
	UncommentAction(const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent);
protected slots:
	virtual void actionTriggered();
};

#endif // _COMMENTACTIONS_H_
