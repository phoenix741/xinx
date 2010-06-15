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
#include "webserviceseditor.h"

struct WebServicesEditorPtr
{
	WebServicesEditorPtr(WebServicesEditor * e) : editor(e) { } ;

	WebServicesEditor * editor;

	void run()
	{
		editor->runBatch();
	}
};

ServicesBatchDialogImpl::ServicesBatchDialogImpl(QWidget *parent) : QDialog(parent)
{
	setupUi(this);

	progressBar->hide();

	m_launchButton = new QPushButton(tr("&Launch"));
	m_launchButton->setDefault(true);

	buttonBox->addButton(m_launchButton, QDialogButtonBox::ActionRole);

	connect(m_launchButton, SIGNAL(clicked()), this, SLOT(launchWebServices()));

	connect(&m_watcher, SIGNAL(progressRangeChanged(int,int)), progressBar, SLOT(setRange(int,int)));
	connect(&m_watcher, SIGNAL(progressValueChanged(int)), progressBar, SLOT(setValue(int)));
	connect(&m_watcher, SIGNAL(resultReadyAt(int)), this, SLOT(resultReadyAt(int)));
}

void ServicesBatchDialogImpl::resultReadyAt(int index)
{
	WebServicesEditor * editor = static_cast<ServicesBatchRow*>(editorWidget->verticalHeaderItem(index))->editor;

	editorWidget->setItem(index, 0, new QTableWidgetItem(QString("%1").arg(editor->executionTime())));
	editorWidget->setItem(index, 2, new QTableWidgetItem(editor->faultString()));
	QTableWidgetItem * backgroundItem = new QTableWidgetItem;
	if (editor->faultString().isEmpty())
		backgroundItem->setBackgroundColor(Qt::green);
	else
		backgroundItem->setBackgroundColor(Qt::red);
	editorWidget->setItem(index, 1, backgroundItem);
}

void ServicesBatchDialogImpl::launchWebServices()
{
	progressBar->show();

	QList<WebServicesEditorPtr> editors;
	for(int i = 0; i < editorWidget->rowCount(); i++)
	{
		WebServicesEditor * editor = static_cast<ServicesBatchRow*>(editorWidget->verticalHeaderItem(i))->editor;
		editors << WebServicesEditorPtr(editor);
	}

	m_watcher.setFuture(QtConcurrent::map(editors, &WebServicesEditorPtr::run));
}
