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
#include "searchdockimpl.h"
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
	friend class SearchDockWidgetImpl;
};

/* SearchLogWidgetDelegate */

// FIXME: Make it a column delegate
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
	if (index.column() == 1)    // Filename
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

/* SearchDockWidgetImpl */

SearchDockWidgetImpl::SearchDockWidgetImpl(QWidget * parent) : AbstractMessageDockWidget(parent), _widget(new Ui::SearchDockWidget)
{
	setWindowTitle(tr("Search Result"));
	setWindowIcon(QIcon(":/images/find.png"));

	_widget->setupUi(this);

	_widget->m_searchTreeWidget->setItemDelegate(new SearchLogWidgetDelegate);
	connect(_widget->m_searchTreeWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));

	connect(_widget->_search_button, SIGNAL(clicked()), this, SIGNAL(abort()));
	_widget->_search_button->hide();
	_widget->m_progressBar->hide();
}

SearchDockWidgetImpl::~SearchDockWidgetImpl()
{

}

bool SearchDockWidgetImpl::automatcallyShow() const
{
    return true;
}

bool SearchDockWidgetImpl::automaticallyClose() const
{
	return false;
}

void SearchDockWidgetImpl::init()
{
	setNotifyCount(0);
	_widget->m_searchTreeWidget->clear();
	_widget->m_searchTreeWidget->setSortingEnabled(false);
	_widget->_search_button->show();
	_widget->m_progressBar->show();
}

void SearchDockWidgetImpl::end()
{
	_widget->_search_button->hide();
	_widget->m_progressBar->hide();
	_widget->m_searchTreeWidget->setSortingEnabled(true);
}

void SearchDockWidgetImpl::find(const QString & filename, const QString & text, int line)
{
	QString path        = QDir::toNativeSeparators(QFileInfo(filename).absolutePath());
	QString emplacement = QFileInfo(filename).fileName() + ":" + QString::number(line);
	QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << path << emplacement << text);
	item->setData(0, Qt::UserRole, filename);
	item->setData(0, Qt::UserRole + 1, line);
	item->setData(1, Qt::UserRole, filename);
	item->setData(1, Qt::UserRole + 1, line);
	item->setData(2, Qt::UserRole, filename);
	item->setData(2, Qt::UserRole + 1, line);
	_widget->m_searchTreeWidget->addTopLevelItem(item);
	_widget->m_searchTreeWidget->scrollToItem(item);
	setNotifyCount(_widget->m_searchTreeWidget->invisibleRootItem()->childCount());
}

void SearchDockWidgetImpl::doubleClicked(const QModelIndex & index)
{
	QString filename = index.data(Qt::UserRole).toString();
	int line         = index.data(Qt::UserRole + 1).toInt();

	if (!filename.isEmpty())
	{
		emit open(filename, line);
	}
}
