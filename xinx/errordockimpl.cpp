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
#include "errordockimpl.h"
#include <core/exceptions.h>
#include <plugins/xinxpluginsloader.h>
#include <core/xinxconfig.h>

// Qt header
#include <QFileInfo>
#include <QItemDelegate>
#include <QPainter>

/* ErrorDockWidgetImpl */

ErrorDockWidgetImpl::ErrorDockWidgetImpl(QWidget * parent) : QWidget(parent), m_dock(0)
{
	setWindowTitle(tr("Warnings/Errors"));
	setWindowIcon(QIcon(":/images/warning.png"));

	_widget = new Ui::ErrorDockWidget();
	_widget->setupUi(this);

	connect(_widget->m_messagesWidget,   SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
}

ErrorDockWidgetImpl::~ErrorDockWidgetImpl()
{

}

void ErrorDockWidgetImpl::updateErrors()
{
	_widget->m_messagesWidget->clear();

	QMap<QString, QList<ErrorManager::Error> > errors = ErrorManager::self()->errors();
	foreach(QString context, errors.keys())
	{
		QTreeWidgetItem * fileItem = new QTreeWidgetItem(_widget->m_messagesWidget);
		fileItem->setText(0, context);
		QList<IFileTypePlugin *> fileTypes = XinxPluginsLoader::self()->matchedFileType(context);
		if (fileTypes.size())
		{
			fileItem->setIcon(0, QIcon(fileTypes.at(0)->icon()));
		}

		foreach(ErrorManager::Error err, errors.value(context))
		{
			QString message = err.message;
			foreach(QString arg, err.parameters)
			{
				message = message.arg(arg);
			}

			QTreeWidgetItem * msgItem = new QTreeWidgetItem(fileItem);
			if (err.line > 0)
				msgItem->setText(0, tr("%1: %2").arg(err.line, 4, 10, QChar('0')).arg(message));
			else
				msgItem->setText(0, tr("%1").arg(message));
			msgItem->setData(0, Qt::UserRole, context);
			msgItem->setData(0, Qt::UserRole + 1, err.line);
			switch (err.type)
			{
			case ErrorManager::MessageError:
				msgItem->setIcon(0, style()->standardIcon(QStyle::SP_MessageBoxCritical));
				break;
			case ErrorManager::MessageWarning:
				msgItem->setIcon(0, style()->standardIcon(QStyle::SP_MessageBoxWarning));
				break;
			case ErrorManager::MessageInformation:
				msgItem->setIcon(0, style()->standardIcon(QStyle::SP_MessageBoxInformation));
				break;
			}
		}

		_widget->m_messagesWidget->expandItem(fileItem);
	}

	if (m_dock)
	{
		if (_widget->m_messagesWidget->invisibleRootItem()->childCount())
		{
			m_dock->setVisible(true);
		}
		else
		{
			if (XINXConfig::self()->config().editor.closeErrorDockAutomatically)
				m_dock->setVisible(false);
		}
	}
}

void ErrorDockWidgetImpl::doubleClicked(const QModelIndex & index)
{
	QString filename = index.data(Qt::UserRole).toString();
	int line         = index.data(Qt::UserRole + 1).toInt();

	if (!filename.isEmpty())
	{
		emit open(filename, line);
	}
}

void ErrorDockWidgetImpl::setDock(QDockWidget * dock)
{
	m_dock = dock;
}
