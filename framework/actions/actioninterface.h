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
#ifndef ACTIONINTERFACE_H
#define ACTIONINTERFACE_H

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QObject>
#include <QAction>
#include <QKeySequence>
#include <QIcon>
#include <QPointer>

class AbstractEditor;

namespace XinxAction
{

class ActionManager;

class LIBEXPORT MenuItem : public QObject
{
	Q_OBJECT
public:
	MenuItem(QAction * a = 0);
	virtual ~MenuItem();

	QAction * action() const;

	virtual bool isEnabled() const;
	virtual bool isVisible() const;

public slots:
	void updateActionState();

protected:
	QAction * m_action;
};

class LIBEXPORT Separator : public MenuItem
{
	Q_OBJECT
public:
	Separator();
	virtual ~Separator();

	virtual bool isVisible() const;
	void setVisible(bool value);

private:
	bool _visible;
};

class LIBEXPORT Action : public MenuItem
{
	Q_OBJECT
public:
	Action(QAction * a);
	Action(const QString & text);
	Action(const QString & text, const QKeySequence & shortcut);
	Action(const QIcon & icon, const QString & text, const QKeySequence & shortcut);
	virtual ~Action();

protected:
	void addEditorSignals(const char * signal);

	void setCurrentEditor(AbstractEditor * editor);
	AbstractEditor * currentEditor() const;

protected slots:
	virtual void actionTriggered();

private:
	QPointer<AbstractEditor> _editor;
	QList<const char *> _signals;

	friend class ActionManager;
};

}

#endif // ACTIONINTERFACE_H
