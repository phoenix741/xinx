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

#include "logdialogimpl_p.h"

/* LogDialogImplPrivate */

LogDialogImplPrivate::LogDialogImplPrivate() : _ui(new Ui::LogDialog)
{

}

LogDialogImplPrivate::~LogDialogImplPrivate()
{
}

void LogDialogImplPrivate::updateLogEntries()
{
	QRegExp filterRegExp(_ui->searchText->text());

	int index = 0;
	_ui->logList->clear();
	foreach(const LogEntry & entry, _logEntries)
	{
		if (_ui->searchText->text().isEmpty() || filterRegExp.exactMatch(entry.message))
		{
			QTreeWidgetItem * item = new QTreeWidgetItem(_ui->logList, QStringList() << entry.revision << entry.author << entry.dateTime.toString(Qt::SystemLocaleShortDate) << QString(entry.message).remove("\n"));
			item->setData(0, Qt::UserRole, QVariant(index));
		}

		index++;
	}
}

void LogDialogImplPrivate::updateLogEntry()
{
	QTreeWidgetItem * item = _ui->logList->currentItem();
	if (item)
	{
		int index = item->data(0, Qt::UserRole).toInt();
		LogEntry entry = _logEntries.at(index);

		_ui->logMessage->setPlainText(entry.message);

		_ui->logPath->clear();
		foreach(const LogPath & path, entry.changedPath)
		{
			new QTreeWidgetItem(_ui->logPath, QStringList() << QString() << path.path << path.informations);
		}
	}
}

/* LogDialogImpl */

LogDialogImpl::LogDialogImpl(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f), d(new LogDialogImplPrivate)
{
	d->_ui->setupUi(this);

	d->_ui->logPath->hide();
	d->_ui->label_5->hide();

	connect(d->_ui->searchText, SIGNAL(textChanged(QString)), d.data(), SLOT(updateLogEntries()));
	connect(d->_ui->logList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), d.data(), SLOT(updateLogEntry()));
}

LogDialogImpl::~LogDialogImpl()
{

}

void LogDialogImpl::setLogEntries(const LogEntries& log)
{
	d->_logEntries = log;
	d->updateLogEntries();
}

const LogEntries& LogDialogImpl::logEntries() const
{
	return d->_logEntries;
}
