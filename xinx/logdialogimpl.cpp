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
#include "logdialogimpl.h"
#include <core/exceptions.h>
#include <plugins/xinxpluginsloader.h>

// Qt header
#include <QFileInfo>
#include <QItemDelegate>
#include <QPainter>

/* SearchLogWidget */

class SearchLogWidget : public QTreeWidget
{
public:
	friend class LogDockWidget;
};

/* SearchLogWidgetDelegate */

class SearchLogWidgetDelegate : public QItemDelegate
{
public:
	SearchLogWidgetDelegate(QObject * parent = 0);

	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
private:

};

SearchLogWidgetDelegate::SearchLogWidgetDelegate(QObject * parent) : QItemDelegate(parent)
{
}

void SearchLogWidgetDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QString text = index.data().toString();
	QStyleOptionViewItem myOpt = option;
	if (index.column() == 0)    // Filename
	{
		drawBackground(painter, option, index);

		QRectF boundingRect;
		painter->setPen(Qt::magenta);
		painter->drawText(myOpt.rect, Qt::AlignLeft, text.left(text.indexOf(':')), &boundingRect);
		myOpt.rect.setLeft((int)(myOpt.rect.left() + boundingRect.width()));
		painter->setPen(Qt::black);
		painter->drawText(myOpt.rect, Qt::AlignLeft, ":", &boundingRect);
		myOpt.rect.setLeft((int)(myOpt.rect.left() + boundingRect.width()));
		painter->setPen(Qt::blue);
		painter->drawText(myOpt.rect, Qt::AlignLeft, text.mid(text.indexOf(':') + 1), &boundingRect);

		drawFocus(painter, option, option.rect);

		return;
	}

	QItemDelegate::paint(painter, option, index);
}

/* LogDockWidget */

LogDockWidget::LogDockWidget(const QString & title, QWidget * parent, Qt::WindowFlags flags) : QDockWidget(title, parent, flags)
{
	QWidget * contentWidget = new QWidget(this);
	m_logwidget = new Ui::LogWidget();
	m_logwidget->setupUi(contentWidget);
	setWidget(contentWidget);

	connect(m_logwidget->m_searchTreeWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
	connect(m_logwidget->m_messagesWidget,   SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
	m_logwidget->m_searchTreeWidget->setItemDelegate(new SearchLogWidgetDelegate);

	m_logwidget->m_progressBar->hide();
}

LogDockWidget::LogDockWidget(QWidget * parent, Qt::WindowFlags flags) : QDockWidget(parent, flags)
{
	QWidget * contentWidget = new QWidget(this);
	m_logwidget = new Ui::LogWidget();
	m_logwidget->setupUi(contentWidget);
	setWidget(contentWidget);

	connect(m_logwidget->m_searchTreeWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
	connect(m_logwidget->m_messagesWidget,   SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
	m_logwidget->m_searchTreeWidget->setItemDelegate(new SearchLogWidgetDelegate);

	m_logwidget->m_progressBar->hide();
}

LogDockWidget::~LogDockWidget()
{

}

void LogDockWidget::init()
{
	m_logwidget->m_rcsLogListWidget->clear();
	m_logwidget->m_searchTreeWidget->clear();
	m_logwidget->m_progressBar->show();
}

void LogDockWidget::end()
{
	m_logwidget->m_progressBar->hide();
}

void LogDockWidget::log(RCS::rcsLog niveau, const QString & info)
{
	QListWidgetItem * item = new QListWidgetItem(info, m_logwidget->m_rcsLogListWidget);
	switch (niveau)
	{
	case RCS::LogError :
	case RCS::LogConflict :
		item->setForeground(Qt::red);
		break;
	case RCS::LogLocallyModified :
		item->setForeground(Qt::darkYellow);
		break;
	case RCS::LogRemotlyModified :
		item->setForeground(Qt::darkGreen);
		break;
	case RCS::LogApplication :
		item->setForeground(Qt::lightGray);
		break;
	case RCS::LogNotManaged :
	case RCS::LogNormal :
	default:
		;
	}
	m_logwidget->m_rcsLogListWidget->scrollToItem(item);
	m_logwidget->m_tabWidget->setCurrentWidget(m_logwidget->m_rcsLogTab);
}

void LogDockWidget::find(const QString & filename, const QString & text, int line)
{
	QString emplacement = QFileInfo(filename).fileName() + ":" + QString::number(line);
	QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << emplacement << text);
	item->setData(0, Qt::UserRole, filename);
	item->setData(0, Qt::UserRole + 1, line);
	item->setData(1, Qt::UserRole, filename);
	item->setData(1, Qt::UserRole + 1, line);
	m_logwidget->m_searchTreeWidget->addTopLevelItem(item);
	m_logwidget->m_searchTreeWidget->scrollToItem(item);
	m_logwidget->m_tabWidget->setCurrentWidget(m_logwidget->m_searchTab);
}

void LogDockWidget::updateErrors()
{
	m_logwidget->m_messagesWidget->clear();

	QMap<QString, QList<ErrorManager::Error> > errors = ErrorManager::self()->errors();
	foreach(QString context, errors.keys())
	{
		QTreeWidgetItem * fileItem = new QTreeWidgetItem(m_logwidget->m_messagesWidget);
		fileItem->setText(0, context);
		IFileTypePlugin * fileType = XinxPluginsLoader::self()->matchedFileType(context);
		if( fileType )
		{
			fileItem->setIcon(0, QIcon(fileType->icon()));
		}

		foreach(ErrorManager::Error err, errors.value(context))
		{
			QString message = err.message;
			foreach(QString arg, err.parameters)
			{
				message = message.arg(arg);
			}

			QTreeWidgetItem * msgItem = new QTreeWidgetItem(fileItem);
			msgItem->setText(0, tr("%1: %2").arg(err.line, 4, 10, QChar('0')).arg(message));
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

		m_logwidget->m_messagesWidget->expandItem(fileItem);
	}

	if(m_logwidget->m_messagesWidget->invisibleRootItem()->childCount())
	{
		m_logwidget->m_tabWidget->setCurrentWidget(m_logwidget->m_messageTab);
		setVisible(true);
	}
	else
	{
		setVisible(false);
	}
}

void LogDockWidget::doubleClicked(const QModelIndex & index)
{
	QString filename = index.data(Qt::UserRole).toString();
	int line         = index.data(Qt::UserRole + 1).toInt();

	if(!filename.isEmpty())
	{
		emit open(filename, line);
	}
}

