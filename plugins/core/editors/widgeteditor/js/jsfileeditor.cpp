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
#include "jsfileeditor.h"
#include "editors/widgeteditor/js/jstexteditor.h"
#include "editors/models/js/javascriptmodelcompleter.h"
#include "editors/models/js/jscontentviewparser.h"

// Qt header
#include <QCompleter>

/* StyleSheetContainer */

JSFileEditor::JSFileEditor(QWidget *parent) : TextFileEditor(new JSTextEditor(), parent), m_completionModel(0)
{

}

JSFileEditor::~JSFileEditor()
{
	qobject_cast<JSTextEditor*>(textEdit())->setModel(0);
}

ContentView2::Parser * JSFileEditor::createParser()
{
	return new JsContentViewParser();
}

ContentView2::CompletionModel * JSFileEditor::createModel(QSqlDatabase db, QObject * parent)
{
	if (! m_completionModel)
	{
		m_completionModel = new JavascriptModelCompleter(db, fileContainer(), parent);

		qobject_cast<JSTextEditor*>(textEdit())->setModel(m_completionModel);
	}
	return m_completionModel;
}

QString JSFileEditor::defaultFileName() const
{
	return tr("noname.js");
}

QIcon JSFileEditor::icon() const
{
	return QIcon(":/images/typejs.png");
}

JavascriptModelCompleter * JSFileEditor::completionModel() const
{
	return m_completionModel;
}
