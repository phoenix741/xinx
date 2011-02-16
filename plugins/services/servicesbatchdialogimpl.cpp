/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2010 by Ulrich Van Den Hekke                              *
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

#include "servicesbatchdialogimpl.h"

ServicesBatchDialogImpl::ServicesBatchDialogImpl(QWidget *parent) : QDialog(parent)
{
	setupUi(this);

	progressBar->hide();

	m_launchButton = new QPushButton(tr("&Launch"));
	m_launchButton->setDefault(true);

	buttonBox->addButton(m_launchButton, QDialogButtonBox::ActionRole);

	connect(m_launchButton, SIGNAL(clicked()), this, SLOT(launchWebServices()));

	m_signalMapper = new QSignalMapper(this);
	connect(m_signalMapper, SIGNAL(mapped(int)), this, SLOT(resultReadyAt(int)));
}

void ServicesBatchDialogImpl::resultReadyAt(int index)
{
	m_progression++;

	WebServicesEditorPtr editor = m_editors.at(index);

	m_signalMapper->removeMappings(editor);

	QTableWidgetItem * timingItem = new QTableWidgetItem(QString("%1 ms").arg(editor->executionTime()));
	QTableWidgetItem * backgroundItem = new QTableWidgetItem;
	if (editor->faultString().isEmpty())
		backgroundItem->setBackgroundColor(Qt::green);
	else
		backgroundItem->setBackgroundColor(Qt::red);
	QTableWidgetItem * faultStringItem = new QTableWidgetItem(editor->faultString());

	editorWidget->setItem(index, 0, timingItem);
	editorWidget->setItem(index, 1, backgroundItem);
	editorWidget->setItem(index, 2, faultStringItem);

	progressBar->setValue(m_progression);
	if (m_progression == editorWidget->rowCount())
	{
		progressBar->hide();
		buttonBox->setDisabled(false);
	}
}


void webServicesCall(WebServicesEditorPtr &editor)
{
	editor->runBatch();
}


void ServicesBatchDialogImpl::launchWebServices()
{
	m_progression = 0;
	buttonBox->setDisabled(true);

	progressBar->show();
	progressBar->setMinimum(0);
	progressBar->setMaximum(editorWidget->rowCount());
	progressBar->setValue(0);

	m_editors.clear();
	for (int i = 0; i < editorWidget->rowCount(); i++)
	{
		WebServicesEditorPtr editor = static_cast<ServicesBatchRow*>(editorWidget->verticalHeaderItem(i))->editor;
		connect(editor, SIGNAL(operationTerminated()), m_signalMapper, SLOT(map()));
		m_signalMapper->setMapping(editor, i);
		m_editors.append(editor);
	}

	QtConcurrent::map(m_editors, &webServicesCall);
}
