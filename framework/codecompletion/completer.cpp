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

#include "completer.h"
#include <QAbstractItemView>
#include <QAbstractProxyModel>
#include <QKeyEvent>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>
#include <codecompletion/model.h>
#include <document/qdocumentcursor.h>
#include <QStyledItemDelegate>
#include <QTreeView>
#include <QHeaderView>

namespace CodeCompletion
{

/* PrivateCompleter */

class PrivateCompleter
{
public:
	PrivateCompleter();

	QModelIndex findNextSelectableIndex(QModelIndex index, bool up);

	Completer * _completer;
	TextFileEditor * _editor;
	Model* _model;
	Context _context;
    QTreeView* _treeView;
};

PrivateCompleter::PrivateCompleter() : _editor(0), _model(0), _treeView(0)
{

}

QModelIndex PrivateCompleter::findNextSelectableIndex(QModelIndex index, bool up)
{
	QModelIndex curIndex = index;

	do
	{
		if (up)
		{
			if (curIndex.row() == 0)
			{
				curIndex = _completer->completionModel()->index(_completer->completionModel()->rowCount() - 1, curIndex.column());
			}
			else
			{
				curIndex = _completer->completionModel()->index(curIndex.row() - 1, curIndex.column());
			}
		}
		else
		{
			if (curIndex.row() == _completer->completionModel()->rowCount() - 1)
			{
				curIndex = _completer->completionModel()->index(0, curIndex.column());
			}
			else
			{
				curIndex = _completer->completionModel()->index(curIndex.row() + 1, curIndex.column());
			}
		}

		if (curIndex == index)
		{
			break;
		}
	} while (! curIndex.flags().testFlag(Qt::ItemIsSelectable));

	return curIndex;
}

/* Completer */

Completer::Completer(TextFileEditor* parent): QCompleter(parent), d(new PrivateCompleter)
{
	d->_completer = this;

	d->_treeView = new QTreeView;
	setPopup(d->_treeView);

	d->_treeView->setHeaderHidden(true);

	d->_treeView->setAllColumnsShowFocus (true);
	d->_treeView->setRootIsDecorated(false);
	d->_treeView->setSelectionMode (QAbstractItemView::SingleSelection);
	d->_treeView->setSelectionBehavior (QAbstractItemView::SelectRows);
	d->_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	d->_treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	connect(d->_treeView->header (), SIGNAL(sectionCountChanged(int,int)), this, SLOT(resizeColumns(int,int)));

	setEditor(parent);
	setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	setCaseSensitivity(Qt::CaseInsensitive);
	setCompletionRole (Qt::UserRole + 1);
	setCompletionColumn (1);
}

Completer::~Completer()
{

}

void Completer::resizeColumns(int oldSize, int newSize)
{
	if (newSize == 2)
	{
		d->_treeView->header()->setStretchLastSection(false);
		d->_treeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
		d->_treeView->header()->setResizeMode(1, QHeaderView::Stretch);
	}
}

void Completer::complete(bool automaticCall, const QString & prefix)
{
	if (d->_model)
	{
		if (! prefix.isEmpty ())
		{
			d->_context.setPrefix (prefix);
			setCompletionPrefix(prefix);
		}
		else
		{
			setCompletionPrefix(d->_context.prefix());
		}

		d->_model->updateModel ();

		// Call made by the user (using shortcut or not)
		if ((!automaticCall) && (completionModel()->rowCount() == 1))
		{
			complete(0);
			popup()->hide();
			return;
		}

		int x, y, h, w;
		QPoint pos = d->_editor->textEdit()->editor()->mapFromContents(d->_editor->textEdit()->textCursor().documentPosition());
		d->_editor->textEdit()->editor()->getPanelMargins(&x, &y, &h, &w);

		QRect cr(pos.x() + x - popup()->sizeHintForColumn(0), pos.y() + d->_editor->textEdit()->document()->fontMetrics().height(), 1, 1);
		cr.setWidth(popup()->sizeHintForColumn(0) + popup()->sizeHintForColumn(1) + popup()->verticalScrollBar()->sizeHint().width());

		QCompleter::complete(cr);

		QModelIndex index = d->findNextSelectableIndex(d->_model->index(0, completionColumn ()), false);
		setCurrentRow (index.row ());
		popup()->setCurrentIndex (index);
	}
}

void Completer::complete(int row)
{
	if (d->_model)
	{
		QAbstractProxyModel* proxyModel = qobject_cast<QAbstractProxyModel*>(completionModel());
		Q_ASSERT_X(proxyModel != 0, "Completer::complete", "completionModel() must be a proxy model");

		QModelIndex sourceIndex = proxyModel->mapToSource(proxyModel->index(row, 0));

		QStandardItem * standardItem = d->_model->itemFromIndex(sourceIndex);
		CodeCompletion::Item * item = dynamic_cast<CodeCompletion::Item*>(standardItem);
		complete(item);
	}
}

void Completer::complete(const QModelIndex& index)
{
	if (index.isValid())
	{
		QAbstractProxyModel* proxyModel = qobject_cast<QAbstractProxyModel*>(completionModel());
		Q_ASSERT_X(proxyModel != 0, "Completer::complete", "completionModel() must be a proxy model");

		QModelIndex sourceIndex = proxyModel->mapToSource(index);

		QStandardItem * standardItem = d->_model->itemFromIndex(sourceIndex);
		CodeCompletion::Item * completionItem = dynamic_cast<CodeCompletion::Item*>(standardItem);
		complete(completionItem);
	}
}

void Completer::complete(Item* item)
{
	if (item)
	{
		QDocumentCursor tc   = d->_editor->textEdit()->textCursor();
		const QString prefix = completionPrefix();

		d->_editor->textEdit()->textUnderCursor(tc, true);

		item->execute (d->_context, editor());
	}
}

void Completer::setContext(Context context)
{
	d->_context = context;
	d->_model->setContext (context);
}

Context Completer::context() const
{
	return d->_context;
}

void Completer::setEditor(TextFileEditor* editor)
{
	if (d->_editor != editor)
	{
		if (d->_editor)
		{
			d->_editor->textEdit()->setCompleter(0);
		}
		d->_editor = editor;
		d->_editor->textEdit()->setCompleter(this);
	}
}

TextFileEditor* Completer::editor() const
{
	return d->_editor;
}

void Completer::setModel(Model* model)
{
	d->_model = model;
	QCompleter::setModel (model);
}

Model* Completer::model() const
{
	return d->_model;
}

bool Completer::eventFilter(QObject *o, QEvent *e)
{
	if ((e->type() == QEvent::KeyPress) && popup()->isVisible())
	{
		QKeyEvent *ke = static_cast<QKeyEvent *>(e);

		QModelIndex curIndex = popup()->currentIndex();
		QModelIndexList selList = popup()->selectionModel()->selectedIndexes();
		if (!curIndex.isValid () && !selList.isEmpty ())
		{
			// Apparently on QTreeView, currentIndex is not correctly filled.
			curIndex = selList.at (0);
		}

		const int key = ke->key();

		switch (key) {
		case Qt::Key_Up:
			{
				QModelIndex newIndex = d->findNextSelectableIndex(curIndex, true);
				popup()->setCurrentIndex(newIndex);
				return true;
			}
			break;
		case Qt::Key_Down:
			{
				QModelIndex newIndex = d->findNextSelectableIndex(curIndex, false);
				popup()->setCurrentIndex(newIndex);
				return true;
			}
			break;
		}
	}

	return QCompleter::eventFilter(o, e);

}

}
