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
#include <core/xinxconfig.h>

// Qt header
#include <QFileInfo>
#include <QItemDelegate>
#include <QPainter>

/* SearchLogWidget */

/*
class SearchLogWidget : public QTreeWidget
{
public:
	friend class SearchDockWidgetImpl;
};
*/

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
	if ((index.column() == 0) && (qobject_cast<const FindedModel*>(index.model())->isIndexLine(index)))
	{
		int line = index.data(FindedModel::LineRole).toInt();
		QString content = index.data(FindedModel::ContentRole).toString();

		QStyleOptionViewItem myOpt = option;

		drawBackground(painter, option, index);

		QRectF boundingRect;
		painter->setFont(index.data(Qt::FontRole).value<QFont>());
		painter->setBackground(Qt::lightGray);
		painter->setBackgroundMode(Qt::OpaqueMode);
		painter->drawText(myOpt.rect, Qt::AlignLeft, QString("%1:").arg(line, 5), &boundingRect);
		myOpt.rect.setLeft((int)(myOpt.rect.left() + boundingRect.width()));
		painter->setBackgroundMode(Qt::TransparentMode);
		painter->drawText(myOpt.rect, Qt::AlignLeft, content, &boundingRect);

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

	_model = new FindedModel(this);
	_widget->m_searchTreeView->setModel(_model);

	_widget->m_searchTreeView->setItemDelegate(new SearchLogWidgetDelegate(_widget->m_searchTreeView));
	connect(_widget->m_searchTreeView, SIGNAL(activated(QModelIndex)), this, SLOT(activated(QModelIndex)));

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
	_model->clear();
	_widget->_search_button->show();
	_widget->m_progressBar->show();
}

void SearchDockWidgetImpl::end()
{
	_widget->_search_button->hide();
	_widget->m_progressBar->hide();
}

QModelIndex SearchDockWidgetImpl::selectedIndex() const
{
	return _widget->m_searchTreeView->currentIndex();
}

QModelIndex SearchDockWidgetImpl::selectNextFinded()
{
	QModelIndex idx = selectedIndex();
	QModelIndex nextIndex;

	if (! idx.isValid())
	{
		nextIndex = _model->index(0, 0, _model->index(0, 0));
	}
	else if (_model->isIndexFile(idx))
	{
		nextIndex = idx.child(0, 0);
	}
	else if (_model->isIndexLine(idx) && (idx.row() + 1 < _model->rowCount(idx.parent())))
	{
		nextIndex = _model->index(idx.row() + 1, idx.column(), idx.parent());
	}
	else if (_model->isIndexLine(idx) && (idx.row() + 1 >= _model->rowCount(idx.parent())))
	{
		QModelIndex parentIndex = idx.parent();
		nextIndex = _model->index(parentIndex.row() + 1, parentIndex.column(), parentIndex.parent());
		nextIndex = nextIndex.child(0, 0);
	}

	_widget->m_searchTreeView->setCurrentIndex(nextIndex);
	activated(nextIndex);
	return nextIndex;
}

QModelIndex SearchDockWidgetImpl::selectPreviousFinded()
{
	QModelIndex idx = selectedIndex();
	QModelIndex previousIndex;

	if (! idx.isValid())
	{
		QModelIndex parentIndex = _model->index(_model->rowCount() - 1, 0);
		previousIndex = _model->index(_model->rowCount(parentIndex) - 1, 0, parentIndex);
	}
	else if (_model->isIndexFile(idx))
	{
		QModelIndex parentIndex = idx;
		previousIndex = _model->index(parentIndex.row() - 1, parentIndex.column(), parentIndex.parent());
		previousIndex = previousIndex.child(_model->rowCount(previousIndex) - 1, 0);
	}
	else if (_model->isIndexLine(idx) && (idx.row() > 0))
	{
		previousIndex = _model->index(idx.row() - 1, idx.column(), idx.parent());
	}
	else if (_model->isIndexLine(idx) && (idx.row() == 0))
	{
		QModelIndex parentIndex = idx.parent();
		previousIndex = _model->index(parentIndex.row() - 1, parentIndex.column(), parentIndex.parent());
		previousIndex = previousIndex.child(_model->rowCount(previousIndex) - 1, 0);
	}

	_widget->m_searchTreeView->setCurrentIndex(previousIndex);
	activated(previousIndex);
	return previousIndex;
}

void SearchDockWidgetImpl::find(const QString & filename, const QString & text, int line)
{
	_model->append(filename, line, text);
	setNotifyCount(notifyCount() + 1);
}

void SearchDockWidgetImpl::activated(const QModelIndex & index)
{
	if (index.isValid())
	{
		QString filename = index.data(FindedModel::FilenameRole).toString();
		int line         = index.data(FindedModel::LineRole).toInt();

		if (!filename.isEmpty())
		{
			emit open(filename, line);
		}
	}
}
