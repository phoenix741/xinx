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

// Xinx header
#include "stylesheetaction.h"
#include "editors/widgeteditor/html/htmlfileeditor.h"
#include "editors/widgeteditor/xsl/xslfileeditor.h"
#include <editors/editormanager.h>
#include "docks/datastream/xmlpresentationdockwidget.h"
#include "config/selfwebpluginsettings.h"

// Qt header
#include <QMessageBox>

/* StyleSheetAction */

StyleSheetAction::StyleSheetAction() : XinxAction::Action(QIcon(":/images/run.png"), tr("Process stylesheet"), QString("F9")), m_dock(0)
{
}

void StyleSheetAction::setXmlPresentationDockWidget(XmlPresentationDockWidget * value) const
{
	// FIXME:  N'est peut-être plus util
	if (m_dock != value)
	{
		if (m_dock != 0)
			m_dock->disconnect(this);
		if (value  != 0)
			connect(value, SIGNAL(filenameChanged(QString)), this, SLOT(updateActionState()));
		m_dock = value;
	}
}

bool StyleSheetAction::isVisible() const
{
	return true;
}

bool StyleSheetAction::isEnabled() const
{
	if (qobject_cast<StyleSheetEditor*>(EditorManager::self()->currentEditor()))
	{
		if (! m_dock)
		{
			setXmlPresentationDockWidget(StyleSheetEditor::xmlPresentationDockWidget());
		}
		return m_dock && !m_dock->filename().isEmpty();
	}
	else if (qobject_cast<HtmlFileEditor*>(EditorManager::self()->currentEditor()))
	{
		return true;
	}

	return false;
}

void StyleSheetAction::actionTriggered()
{
	Q_ASSERT(qobject_cast<StyleSheetEditor*>(EditorManager::self()->currentEditor()) || qobject_cast<HtmlFileEditor*>(EditorManager::self()->currentEditor()));

	if (qobject_cast<StyleSheetEditor*>(EditorManager::self()->currentEditor()))
	{
		Q_ASSERT(m_dock);
		qobject_cast<StyleSheetEditor*>(EditorManager::self()->currentEditor())->launchStylesheetParsing(m_dock->filename());
		qobject_cast<StyleSheetEditor*>(EditorManager::self()->currentEditor())->showHtml();
	}

	if (qobject_cast<HtmlFileEditor*>(EditorManager::self()->currentEditor()))
	{
		qobject_cast<HtmlFileEditor*>(EditorManager::self()->currentEditor())->showHtml();
	}
}
