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
#include <project/xinxprojectproject.h>
#include "webservices.h"
#include "webserviceseditor.h"
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>
#include "servicesbatchdialogimpl.h"
#include <QApplication>
#include <QMessageBox>

inline static WebServicesManager * projectWebServicesManager(AbstractEditor * editor)
{
	return WebServicesManager::manager(editor->project());
}

inline static bool projectIsActivated(AbstractEditor * editor)
{
	if (!editor) return false;

	XinxProject::ProjectPtr project = editor->project();
	if (!(project && project->activatedPlugin().contains("ServicesPlugin") && project->containsObject("webservices"))) return false;

	Q_ASSERT_X(projectWebServicesManager(editor), "projectIsActivated", "The object webservices should be created");

	return true;
}

/* WebServicesRefreshAction */

WebServicesRefreshAction::WebServicesRefreshAction() : XinxAction::Action(QIcon(":/images/reload.png"), tr("Update WebServices List"), QString())
{

}

bool WebServicesRefreshAction::isVisible() const
{
	return projectIsActivated(currentEditor());
}

void WebServicesRefreshAction::actionTriggered()
{
	projectWebServicesManager(currentEditor())->updateWebServicesList();
}

/* WebServicesRunAction */

WebServicesRunAction::WebServicesRunAction() : XinxAction::Action(QIcon(":/services/images/action.png"), tr("Call the service"), QString("F9"))
{
}

bool WebServicesRunAction::isVisible() const
{
	return projectIsActivated(currentEditor());
}

bool WebServicesRunAction::isEnabled() const
{
	if (qobject_cast<WebServicesEditor*>(EditorManager::self()->currentEditor()))
	{
		WebServicesEditor * editor = qobject_cast<WebServicesEditor*>(EditorManager::self()->currentEditor());
		return editor->service() && editor->operation() && EditorManager::self()->currentEditor()->project() && EditorManager::self()->currentEditor()->project()->activatedPlugin().contains("ServicesPlugin");
	}
	return false;
}

void WebServicesRunAction::actionTriggered()
{
	WebServicesEditor * editor = qobject_cast<WebServicesEditor*>(EditorManager::self()->currentEditor());
	Q_ASSERT(editor);

	editor->run();
}

/* WebServicesRunAllAction */

WebServicesRunAllAction::WebServicesRunAllAction() : XinxAction::Action(tr("Launch multiple WebServices"), QString("Shift+F9"))
{
}

bool WebServicesRunAllAction::isVisible() const
{
	return projectIsActivated(currentEditor());
}

void WebServicesRunAllAction::actionTriggered()
{
	ServicesBatchDialogImpl dlg(qApp->activeWindow());

	int count = 0;
	for (int i = 0; i < EditorManager::self()->editorsCount(); i++)
	{
		WebServicesEditor * editor = qobject_cast<WebServicesEditor*>(EditorManager::self()->editor(i));
		if (editor)
		{
			dlg.editorWidget->setRowCount(++count);

			ServicesBatchRow * filenameItem = new ServicesBatchRow(QFileInfo(editor->getTitle()).fileName());
			filenameItem->editor = editor;

			dlg.editorWidget->setVerticalHeaderItem(count - 1, filenameItem);
		}
	}
	if (count == 0)
	{
		QMessageBox::information(qApp->activeWindow(), tr("No editor opened"), tr("Please open all stream you want launch in a editor."));
	}
	else
	{
		dlg.exec();
	}
}
