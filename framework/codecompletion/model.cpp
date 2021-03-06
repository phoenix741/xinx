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
#include "model.h"
#include "context.h"
#include "itemmodelfactory.h"
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>
#include <plugins/xinxpluginsloader.h>
#include <codecompletion/iteminterface.h>
#include <codecompletion/item.h>

namespace CodeCompletion
{

class CategoryItem : public QHash<QString,Item*>
{
public:
	CategoryItem();
	~CategoryItem();

	QStandardItem * categoryItem();
	void setCategoryItem(QStandardItem * item);
private:
	QStandardItem * _category_item;
};

CategoryItem::CategoryItem() : _category_item(0)
{

}

CategoryItem::~CategoryItem()
{

}

void CategoryItem::setCategoryItem(QStandardItem * item)
{
	_category_item = item;
}

QStandardItem * CategoryItem::categoryItem()
{
	return _category_item;
}

/* PrivateModel */

class PrivateModel : public ItemInterface
{
public:
	PrivateModel(Model * model);

	Model * _model;
	Context _context;
	QHash<QString,CategoryItem> _itemsByType;
	QHash<QString,QStringList> _itemsToRemove;

	virtual void addItem(Item* item);
	virtual QStringList contextsType() const;
	virtual void prepareRemovingOldItems(const QString & type);
	virtual void removeOldItems(const QString & type);

	friend class PrivateItem;
};


/* PrivateModel */

PrivateModel::PrivateModel(Model * model) : _model(model)
{
}

void PrivateModel::addItem(Item* item)
{
	Q_ASSERT_X(item, "PrivateModel::addItem", "Item mustn't be null");
	Q_ASSERT_X(! item->keyString().isEmpty(), "PrivateModel::addItem", "KeyString musn't be empty");

	if (! _itemsByType[item->contextType()].categoryItem())
	{
		QFont font = item->data(Qt::FontRole).value<QFont>();
		font.setBold(true);

		QStandardItem * grayItem = new QStandardItem;
		grayItem->setBackground(Qt::gray);
		grayItem->setFlags(Qt::ItemIsEnabled);

		QStandardItem * ci = new QStandardItem(item->contextType());
		ci->setFont(font);
		ci->setBackground(Qt::gray);
		ci->setFlags(Qt::ItemIsEnabled);

		_model->appendRow(QList<QStandardItem*>() << grayItem << ci);

		_itemsByType[item->contextType()].setCategoryItem(ci);
	}

	if (! _itemsByType[item->contextType()].contains(item->keyString()))
	{
		QStandardItem * ci	= _itemsByType[item->contextType()].categoryItem();
		int count			= _itemsByType[item->contextType()].count();
		QString type		= item->completionType();

		_itemsByType[item->contextType()].insert(item->keyString(), item);

		_model->insertRow(ci->row() + count + 1, QList<QStandardItem*>() << new QStandardItem(type) << item);
	}
	else
	{
		QStringList::iterator i = qBinaryFind(_itemsToRemove[item->contextType()].begin(), _itemsToRemove[item->contextType()].end(), item->keyString());
		if (i != _itemsToRemove[item->contextType()].end())
		{
			_itemsToRemove[item->contextType()].erase(i);
		}
		delete item;
	}
}

QStringList PrivateModel::contextsType() const
{
	return _itemsByType.keys();
}

void PrivateModel::prepareRemovingOldItems(const QString & type)
{
	_itemsToRemove[type].clear();

	QHashIterator<QString,Item*> it(_itemsByType.value(type));

	while (it.hasNext())
	{
		it.next();

		_itemsToRemove[type].append(it.key());
	}

	_itemsToRemove[type].sort();
}

void PrivateModel::removeOldItems(const QString & type)
{
	QStringListIterator it(_itemsToRemove.value(type));

	while (it.hasNext())
	{
		const QString key = it.next();
		Item * item = _itemsByType[type].value(key);
		_itemsByType[type].remove(key);

		const int row = item->row();
		if (row)
		{
			_model->removeRow(row);
		}
	}

	_itemsToRemove[type].clear();

	if (! _itemsByType[type].size())
	{
		QStandardItem * ci = _itemsByType[type].categoryItem();
		if (ci)
		{
			_itemsByType[type].setCategoryItem(0);
			_model->removeRow(ci->row());
		}
	}
}

/* Model */

/*!
 * \class Model
 * \brief Model used for completion on Xinx text editor.
 * \since 0.10.0.0
 *
 * This classe is populated by one or more ItemModelFactory.
 *
 * \note This class does't manage yet the notion of title and the notion of delegate. This can be come
 * later on the classe Completer.
 */

Model::Model(QObject* parent): QStandardItemModel(parent), d(new PrivateModel(this))
{

}

Model::~Model()
{

}

void Model::setContext(Context context)
{
	d->_context = context;
}

Context Model::context() const
{
	return d->_context;
}

void Model::updateModel()
{
	XinxPluginsLoader::self()->codeCompletionPool()->generate(d.data(), d->_context);
}

}

