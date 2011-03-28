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


/* LogDialogImpl */

LogDialogImpl::LogDialogImpl(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f), d(new LogDialogImplPrivate)
{
	d->_ui->setupUi(this);
}

LogDialogImpl::~LogDialogImpl()
{

}

void LogDialogImpl::setLogEntries(const LogEntries& log)
{
	d->_logEntries = log;

	d->_ui->logList->clear();
	foreach(const LogEntry & entry, log)
	{
		new QTreeWidgetItem(d->_ui->logList, QStringList() << entry.revision << QString() << entry.author << entry.dateTime.toString() << QString(entry.message).remove("\n"));
	}
}

const LogEntries& LogDialogImpl::logEntries() const
{
	return d->_logEntries;
}
