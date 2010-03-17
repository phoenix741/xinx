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

	connect(m_logwidget->m_searchTreeWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_m_searchTreeWidget_doubleClicked(QModelIndex)));
	m_logwidget->m_searchTreeWidget->setItemDelegate(new SearchLogWidgetDelegate);

	m_logwidget->m_progressBar->hide();
}

LogDockWidget::LogDockWidget(QWidget * parent, Qt::WindowFlags flags) : QDockWidget(parent, flags)
{
	QWidget * contentWidget = new QWidget(this);
	m_logwidget = new Ui::LogWidget();
	m_logwidget->setupUi(contentWidget);
	setWidget(contentWidget);

	connect(m_logwidget->m_searchTreeWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_m_searchTreeWidget_doubleClicked(QModelIndex)));
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
	m_logwidget->m_searchTreeWidget->addTopLevelItem(item);
	m_logwidget->m_searchTreeWidget->scrollToItem(item);
	m_logwidget->m_tabWidget->setCurrentWidget(m_logwidget->m_searchTab);
}
/*
void LogDockWidget::clearMessages(const QString & file)
{
	QTreeWidgetItem * item = m_messageItemFile.value(file);
	if (item)
	{
		m_messageItemFile.remove(file);
		delete item;
	}
}

void LogDockWidget::addMessage(const QString & file, int line, const QString & message, AbstractEditor::LevelMessage level)
{
	QTreeWidgetItem * fileItem = m_messageItemFile.value(file);
	if (! fileItem)
	{
		fileItem = new QTreeWidgetItem(m_logwidget->m_messagesWidget);
		fileItem->setText(0, file);
		m_messageItemFile.insert(file, fileItem);
	}

	QTreeWidgetItem * msgItem = new QTreeWidgetItem(fileItem);
	msgItem->setText(0, tr("%1: %2").arg(line, 4, 10, QChar('0')).arg(message));
	switch (level)
	{
	case AbstractEditor::ERROR_MESSAGE:
		msgItem->setIcon(0, style()->standardIcon(QStyle::SP_MessageBoxCritical));
		break;
	case AbstractEditor::WARNING_MESSAGE:
		msgItem->setIcon(0, style()->standardIcon(QStyle::SP_MessageBoxWarning));
		break;
	case AbstractEditor::INFORMATION_MESSAGE:
		msgItem->setIcon(0, style()->standardIcon(QStyle::SP_MessageBoxInformation));
		break;
	}

	m_logwidget->m_messagesWidget->expandItem(fileItem);
	m_logwidget->m_tabWidget->setCurrentWidget(m_logwidget->m_messageTab);
	setVisible(true);
}
*/
void LogDockWidget::on_m_searchTreeWidget_doubleClicked(const QModelIndex & index)
{
	QTreeWidgetItem * item = static_cast<SearchLogWidget*>(m_logwidget->m_searchTreeWidget)->itemFromIndex(index);
	QString filename = item->data(0, Qt::UserRole).toString();
	int line = item->data(0, Qt::UserRole + 1).toInt();

	emit open(filename, line);
}

