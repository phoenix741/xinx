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

// Xinx header
#include "errordockimpl.h"
#include <core/exceptions.h>
#include <plugins/xinxpluginsloader.h>
#include <core/xinxconfig.h>
#include <plugins/interfaces/files.h>
#include <dtoolview.h>

// Qt header
#include <QFileInfo>
#include <QPainter>
#include <QMenu>

/* ErrorDockWidgetImpl */

ErrorDockWidgetImpl::ErrorDockWidgetImpl(QWidget * parent) : AbstractMessageDockWidget(parent)
{
	setWindowTitle(tr("Warnings/Errors"));
	setWindowIcon(QIcon(":/images/warning.png"));

	_widget = new Ui::ErrorDockWidget();
	_widget->setupUi(this);

	QAction * clearAction = new QAction(tr("Clear the errors"), this);

	_widget->m_messagesWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
	_widget->m_messagesWidget->addAction(clearAction);

	connect(clearAction, SIGNAL(triggered()), this, SLOT(clearAction()));
	connect(_widget->m_messagesWidget,   SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
}

ErrorDockWidgetImpl::~ErrorDockWidgetImpl()
{

}

bool ErrorDockWidgetImpl::automatcallyShow() const
{
	return XINXConfig::self()->config().editor.closeErrorDockAutomatically;
}

bool ErrorDockWidgetImpl::automaticallyClose() const
{
	return XINXConfig::self()->config().editor.closeErrorDockAutomatically;
}

void ErrorDockWidgetImpl::clearAction()
{
	QList<QTreeWidgetItem*> items = _widget->m_messagesWidget->selectedItems();
	foreach(QTreeWidgetItem* item, items)
	{
		const QString context = item->data(0, Qt::UserRole).isValid() ? item->data(0, Qt::UserRole).toString() : item->text(0);

		ErrorManager::self()->clearMessages(context);
	}
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
				msgItem->setText(0, QString("%1: %2").arg(err.line, 4, 10, QChar('0')).arg(message));
			else
				msgItem->setText(0, message);
			msgItem->setData(0, Qt::UserRole, context);
			msgItem->setData(0, Qt::UserRole + 1, err.line);
			switch (err.type)
			{
			case QtFatalMsg:
			case QtCriticalMsg:
				msgItem->setIcon(0, style()->standardIcon(QStyle::SP_MessageBoxCritical));
				break;
			case QtWarningMsg:
				msgItem->setIcon(0, style()->standardIcon(QStyle::SP_MessageBoxWarning));
				break;
			case QtDebugMsg:
				msgItem->setIcon(0, style()->standardIcon(QStyle::SP_MessageBoxInformation));
				break;
			}
		}

		_widget->m_messagesWidget->expandItem(fileItem);
	}

	if (dock())
	{
		setNotifyCount(errors.values().count());

		if (_widget->m_messagesWidget->invisibleRootItem()->childCount() && automatcallyShow())
		{
			dock()->setVisible(true);
		}
		else
		{
			if (automaticallyClose())
				dock()->setVisible(false);
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
