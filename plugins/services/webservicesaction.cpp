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

// Xinx Action
#include "webservicesaction.h"
#include <project/xinxproject.h>
#include "webservices.h"
#include "webserviceseditor.h"
#include <editors/editormanager.h>
#include "servicesbatchdialogimpl.h"
#include <QApplication>
#include <QMessageBox>

/* WebServicesRefreshAction */

WebServicesRefreshAction::WebServicesRefreshAction(QAction * a, QObject * parent) : XinxAction::Action(a, parent)
{
}

WebServicesRefreshAction::WebServicesRefreshAction(const QString & text, const QKeySequence & shortcut, QObject * parent) : XinxAction::Action(text, shortcut, parent)
{
}

WebServicesRefreshAction::WebServicesRefreshAction(const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent) : XinxAction::Action(icon, text, shortcut, parent)
{
}

bool WebServicesRefreshAction::isActionVisible() const
{
	return XINXProjectManager::self()->project() && XINXProjectManager::self()->project()->activatedPlugin().contains("ServicesPlugin");
}

bool WebServicesRefreshAction::isActionEnabled() const
{
	return XINXProjectManager::self()->project() && XINXProjectManager::self()->project()->activatedPlugin().contains("ServicesPlugin");
}

bool WebServicesRefreshAction::isInToolBar() const
{
	return false;
}

void WebServicesRefreshAction::actionTriggered()
{
	WebServicesManager::self()->updateWebServicesList();
}

/* WebServicesRunAction */

WebServicesRunAction::WebServicesRunAction(QAction * a, QObject * parent) : XinxAction::Action(a, parent)
{
}

WebServicesRunAction::WebServicesRunAction(const QString & text, const QKeySequence & shortcut, QObject * parent) : XinxAction::Action(text, shortcut, parent)
{
}

WebServicesRunAction::WebServicesRunAction(const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent) : XinxAction::Action(icon, text, shortcut, parent)
{
}

bool WebServicesRunAction::isActionVisible() const
{
	return XINXProjectManager::self()->project() && XINXProjectManager::self()->project()->activatedPlugin().contains("ServicesPlugin");
}

bool WebServicesRunAction::isActionEnabled() const
{
	if (qobject_cast<WebServicesEditor*>(EditorManager::self()->currentEditor()))
	{
		WebServicesEditor * editor = qobject_cast<WebServicesEditor*>(EditorManager::self()->currentEditor());
		return editor->service() && editor->operation() && XINXProjectManager::self()->project() && XINXProjectManager::self()->project()->activatedPlugin().contains("ServicesPlugin");
	}
	return false;
}

bool WebServicesRunAction::isInToolBar() const
{
	return true;
}

void WebServicesRunAction::actionTriggered()
{
	WebServicesEditor * editor = qobject_cast<WebServicesEditor*>(EditorManager::self()->currentEditor());
	Q_ASSERT(editor);

	editor->run();
}

/* WebServicesRunAllAction */

WebServicesRunAllAction::WebServicesRunAllAction(QAction * a, QObject * parent) : XinxAction::Action(a, parent)
{
}

WebServicesRunAllAction::WebServicesRunAllAction(const QString & text, const QKeySequence & shortcut, QObject * parent) : XinxAction::Action(text, shortcut, parent)
{
}

WebServicesRunAllAction::WebServicesRunAllAction(const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent) : XinxAction::Action(icon, text, shortcut, parent)
{
}

bool WebServicesRunAllAction::isActionVisible() const
{
	return XINXProjectManager::self()->project() && XINXProjectManager::self()->project()->activatedPlugin().contains("ServicesPlugin");
}

bool WebServicesRunAllAction::isActionEnabled() const
{
	return true;
}

bool WebServicesRunAllAction::isInToolBar() const
{
	return false;
}

void WebServicesRunAllAction::actionTriggered()
{
	ServicesBatchDialogImpl dlg(qApp->activeWindow());

	int count = 0;
	for(int i = 0; i < EditorManager::self()->editorsCount(); i++)
	{
		WebServicesEditor * editor = qobject_cast<WebServicesEditor*>(EditorManager::self()->editor(i));
		if(editor)
		{
			dlg.editorWidget->setRowCount(++count);

			ServicesBatchRow * filenameItem = new ServicesBatchRow(QFileInfo(editor->getTitle()).fileName());
			filenameItem->editor = editor;

			dlg.editorWidget->setVerticalHeaderItem(count - 1, filenameItem);
		}
	}
	if(count == 0)
	{
		QMessageBox::information(qApp->activeWindow(), tr("No editor opened"), tr("Please open all stream you want launch in a editor."));
	}
	else
	{
		dlg.exec();
	}
}
