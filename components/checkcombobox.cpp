/****************************************************************************
 **
 ** Copyright (C) Qxt Foundation. Some rights reserved.
 **
 ** This file is part of the QxtGui module of the Qxt library.
 **
 ** This library is free software; you can redistribute it and/or modify it
 ** under the terms of the Common Public License, version 1.0, as published
 ** by IBM, and/or under the terms of the GNU Lesser General Public License,
 ** version 2.1, as published by the Free Software Foundation.
 **
 ** This file is provided "AS IS", without WARRANTIES OR CONDITIONS OF ANY
 ** KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT LIMITATION, ANY
 ** WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY OR
 ** FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** You should have received a copy of the CPL and the LGPL along with this
 ** file. See the LICENSE file and the cpl1.0.txt/lgpl-2.1.txt files
 ** included with the source distribution for more information.
 ** If you did not receive a copy of the licenses, contact the Qxt Foundation.
 **
 ** <http://libqxt.org>  <foundation@libqxt.org>
 **
 ** 02/03/2011 : Mise en forme pour XINX
 **
 ****************************************************************************/
#include "checkcombobox.h"
#include "checkcombobox_p.h"
#include <QLineEdit>
#include <QKeyEvent>

PrivateCheckComboBox::PrivateCheckComboBox(CheckComboBox * parent) : _containerMousePress(false), _separator(QLatin1String(",")), _parent(parent)
{

}

bool PrivateCheckComboBox::eventFilter(QObject* receiver, QEvent* event)
{
	switch (event->type())
	{
	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		if (receiver == _parent && (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down))
		{
			_parent->showPopup();
			return true;
		}
		else if (keyEvent->key() == Qt::Key_Enter ||
				 keyEvent->key() == Qt::Key_Return ||
				 keyEvent->key() == Qt::Key_Escape)
		{
			// it is important to call QComboBox implementation
			_parent->QComboBox::hidePopup();
			if (keyEvent->key() != Qt::Key_Escape)
				return true;
		}
	}
	case QEvent::MouseButtonPress:
		_containerMousePress = (receiver == _parent->view()->window());
		break;
	case QEvent::MouseButtonRelease:
		_containerMousePress = false;;
		break;
	default:
		break;
	}
	return false;
}

void PrivateCheckComboBox::updateCheckedItems()
{
	QStringList items = _parent->checkedItems();
	if (items.isEmpty())
		_parent->setEditText(_defaultText);
	else
		_parent->setEditText(items.join(_separator));

	// TODO: find a way to recalculate a meaningful size hint

	emit _parent->checkedItemsChanged(items);
}

void PrivateCheckComboBox::toggleCheckState(int index)
{
	QVariant value = _parent->itemData(index, Qt::CheckStateRole);
	if (value.isValid())
	{
		Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
		_parent->setItemData(index, (state == Qt::Unchecked ? Qt::Checked : Qt::Unchecked), Qt::CheckStateRole);
	}
}

CheckComboModel::CheckComboModel(QObject* parent)
		: QStandardItemModel(0, 1, parent) // rows,cols
{
}

Qt::ItemFlags CheckComboModel::flags(const QModelIndex& index) const
{
	return QStandardItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

QVariant CheckComboModel::data(const QModelIndex& index, int role) const
{
	QVariant value = QStandardItemModel::data(index, role);
	if (index.isValid() && role == Qt::CheckStateRole && !value.isValid())
		value = Qt::Unchecked;
	return value;
}

bool CheckComboModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	bool ok = QStandardItemModel::setData(index, value, role);
	if (ok && role == Qt::CheckStateRole)
	{
		emit dataChanged(index, index);
		emit checkStateChanged();
	}
	return ok;
}

/*!
	\class CheckComboBox
	\inmodule Gui
	\brief The CheckComboBox widget is an extended QComboBox with checkable items.

	ComboBox is a specialized combo box with checkable items.
    Checked items are collected together in the line edit.

    \code
	CheckComboBox* comboBox = new CheckComboBox(this);
    comboBox->addItems(...);
    comboBox->setItemCheckState(2, Qt::Checked);
    comboBox->setItemCheckState(4, Qt::Checked);
    // OR
    comboBox->setCheckedItems(QStringList() << "dolor" << "amet");
    \endcode

	\image checkcombobox.png "CheckComboBox in Plastique style."
 */

/*!
	\fn CheckComboBox::checkedItemsChanged(const QStringList& items)

    This signal is emitted whenever the checked \a items have been changed.
 */

/*!
	Constructs a new CheckComboBox with \a parent.
 */
CheckComboBox::CheckComboBox(QWidget* parent) : QComboBox(parent), d(new PrivateCheckComboBox(this))
{
	setModel(new CheckComboModel(this));
	connect(this, SIGNAL(activated(int)), d.data(), SLOT(toggleCheckState(int)));
	connect(model(), SIGNAL(checkStateChanged()), d.data(), SLOT(updateCheckedItems()));
	connect(model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), d.data(), SLOT(updateCheckedItems()));
	connect(model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), d.data(), SLOT(updateCheckedItems()));

	// read-only contents
	QLineEdit* lineEdit = new QLineEdit(this);
	lineEdit->setReadOnly(true);
	setLineEdit(lineEdit);
	lineEdit->disconnect(this);
	setInsertPolicy(QComboBox::NoInsert);

	view()->installEventFilter(d.data());
	view()->window()->installEventFilter(d.data());
	view()->viewport()->installEventFilter(d.data());
	this->installEventFilter(d.data());
}

/*!
    Destructs the combo box.
 */
CheckComboBox::~CheckComboBox()
{
}

/*!
    \reimp
 */
void CheckComboBox::hidePopup()
{
	if (d->_containerMousePress)
		QComboBox::hidePopup();
}

/*!
    Returns the check state of the item at \a index.
 */
Qt::CheckState CheckComboBox::itemCheckState(int index) const
{
	return static_cast<Qt::CheckState>(itemData(index, Qt::CheckStateRole).toInt());
}

/*!
    Sets the check \a state of the item at \a index.
 */
void CheckComboBox::setItemCheckState(int index, Qt::CheckState state)
{
	setItemData(index, state, Qt::CheckStateRole);
}

/*!
	\property CheckComboBox::checkedItems
    \brief the checked items.
 */
QStringList CheckComboBox::checkedItems() const
{
	QStringList items;
	if (model())
	{
		QModelIndex index = model()->index(0, modelColumn(), rootModelIndex());
		QModelIndexList indexes = model()->match(index, Qt::CheckStateRole, Qt::Checked, -1, Qt::MatchExactly);
		foreach(const QModelIndex& index, indexes)
		items += index.data().toString();
	}
	return items;
}

void CheckComboBox::setCheckedItems(const QStringList& items)
{
	// not the most efficient solution but most likely nobody
	// will put too many items into a combo box anyway so...
	foreach(const QString& text, items)
	{
		const int index = findText(text);
		setItemCheckState(index, index != -1 ? Qt::Checked : Qt::Unchecked);
	}
}

/*!
	\property CheckComboBox::defaultText
    \brief the default text.

    The default text is shown when there are no checked items.
    The default value is an empty string.
 */
QString CheckComboBox::defaultText() const
{
	return d->_defaultText;
}

void CheckComboBox::setDefaultText(const QString& text)
{
	if (d->_defaultText != text)
	{
		d->_defaultText = text;
		d->updateCheckedItems();
	}
}

/*!
	\property CheckComboBox::separator
    \brief the default separator.

    The checked items are joined together with the separator string.
    The default value is a comma (",").
 */
QString CheckComboBox::separator() const
{
	return d->_separator;
}

void CheckComboBox::setSeparator(const QString& separator)
{
	if (d->_separator != separator)
	{
		d->_separator = separator;
		d->updateCheckedItems();
	}
}
