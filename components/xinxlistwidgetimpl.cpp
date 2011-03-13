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
#include "xinxlistwidgetimpl_p.h"

/* XinxListWidgetImplPrivate */

XinxListWidgetImplPrivate::XinxListWidgetImplPrivate(XinxListWidgetImpl* parent) : _parent(parent), _ui(new Ui::XinxListWidget)
{
	_ui->setupUi(parent);

	connect(_ui->m_btnDef, SIGNAL(clicked()), this, SLOT(on_m_btnDef_clicked()));
	connect(_ui->m_btnAdd, SIGNAL(clicked()), this, SLOT(on_m_btnAdd_clicked()));
	connect(_ui->m_btnDel, SIGNAL(clicked()), this, SLOT(on_m_btnDel_clicked()));

	connect(_ui->m_btnUp, SIGNAL(clicked()), this, SLOT(on_m_btnUp_clicked()));
	connect(_ui->m_btnDown, SIGNAL(clicked()), this, SLOT(on_m_btnDown_clicked()));

	connect(_ui->m_list, SIGNAL(currentRowChanged(int)), this, SLOT(on_m_list_currentRowChanged(int)));
	connect(_ui->m_list, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(on_m_list_itemChanged(QListWidgetItem*)));
}

void XinxListWidgetImplPrivate::updateDefault(const QString & def)
{
	m_defaultValue = -1;
	bool hasDefault = def.isEmpty() ? true : false;
	for (int i = 0; i < _ui->m_list->count() ; i++)
	{
		QListWidgetItem * item = _ui->m_list->item(i);
		bool isDefault = item->text() == def;
		if (isDefault)
		{
			hasDefault = true;
			m_defaultValue = i;
		}
		QFont font = item->font();
		font.setBold(isDefault);
		item->setFont(font);
	}
	if (! hasDefault)
	{
		QListWidgetItem * item = new QListWidgetItem(def, _ui->m_list);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		QFont font = item->font();
		font.setBold(true);
		item->setFont(font);
		_ui->m_list->addItem(item);
		m_defaultValue = _ui->m_list->count() - 1;
	}
}


void XinxListWidgetImplPrivate::on_m_btnDef_clicked()
{
	QListWidgetItem * item = _ui->m_list->currentItem();
	if (item)
	{
		m_defaultValue = _ui->m_list->currentRow();
		updateDefault(item->text());
		emit _parent->defaultValueChanged(item->text());
	}
}

void XinxListWidgetImplPrivate::on_m_btnAdd_clicked()
{
	QString textValue = _parent->addNewValue();
	if (!textValue.isEmpty())
	{
		QListWidgetItem * item = new QListWidgetItem(textValue, _ui->m_list);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		_ui->m_list->addItem(item);
		_ui->m_list->setCurrentItem(item);
		_ui->m_list->editItem(item);

		_ui->m_btnDel->setEnabled(_ui->m_list->count() > 0);
		_ui->m_btnDef->setEnabled(_ui->m_list->count() > 0);
		_ui->m_btnUp->setEnabled(_ui->m_list->currentRow() > 0);
		_ui->m_btnDown->setEnabled((_ui->m_list->currentRow() >= 0) && (_ui->m_list->currentRow() < _ui->m_list->count() - 1));
	}
}

void XinxListWidgetImplPrivate::on_m_btnDel_clicked()
{
	Q_ASSERT(_ui->m_list->currentRow() >= 0);

	delete _ui->m_list->currentItem();

	_ui->m_btnDel->setEnabled(_ui->m_list->count() > 0);
	_ui->m_btnDef->setEnabled(_ui->m_list->count() > 0);
	_ui->m_btnUp->setEnabled(_ui->m_list->currentRow() > 0);
	_ui->m_btnDown->setEnabled((_ui->m_list->currentRow() >= 0) && (_ui->m_list->currentRow() < _ui->m_list->count() - 1));
}

void XinxListWidgetImplPrivate::on_m_btnUp_clicked()
{
	Q_ASSERT((_ui->m_list->currentRow() >= 0) && (_ui->m_list->currentRow() - 1 >= 0));

	int row = _ui->m_list->currentRow();
	QListWidgetItem * item = _ui->m_list->takeItem(_ui->m_list->currentRow());
	_ui->m_list->insertItem(row - 1, item);
	_ui->m_list->setCurrentItem(item);
}

void XinxListWidgetImplPrivate::on_m_btnDown_clicked()
{
	Q_ASSERT((_ui->m_list->currentRow() >= 0) && (_ui->m_list->currentRow() + 1 < _ui->m_list->count()));

	int row = _ui->m_list->currentRow();
	QListWidgetItem * item = _ui->m_list->takeItem(_ui->m_list->currentRow());
	_ui->m_list->insertItem(row + 1, item);
	_ui->m_list->setCurrentItem(item);
}

void XinxListWidgetImplPrivate::on_m_list_currentRowChanged(int row)
{
	_ui->m_btnUp->setEnabled(row > 0);
	_ui->m_btnDown->setEnabled(row < _ui->m_list->count() - 1);
}

void XinxListWidgetImplPrivate::on_m_list_itemChanged(QListWidgetItem * item)
{
	if (item)
		emit _parent->itemChanged(item->text());
	else
		emit _parent->itemChanged(QString());
}

/* XinxListWidgetImpl */

/*!
 * \ingroup Components
 * \class XinxListWidgetImpl
 * \since 0.7.1.0
 *
 * \brief The class XinxListWidgetImpl provides a widget to create, and sort a list.
 *
 * This widget is a list of values (exemple: list of path, list of urls, list of prefix) with
 * a button to add an value, a button to delete a value, and a button to make a value the
 * default (this button can be cached the property defaultVisible()).
 *
 * In this list you have two button too, to move a ligne up, and move a line down to sort
 * your list.
 *
 * \image html xinxlistwidget1.png
 * \image latex xinxlistwidget1.png
 *
 */

/*!
 * \brief Construct the widget with a default, add and remove button.
 * \param parent The parent widget
 * \param f Flags for the widget
 */
XinxListWidgetImpl::XinxListWidgetImpl(QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f), d(new XinxListWidgetImplPrivate(this))
{
	connect(d->_ui->m_list, SIGNAL(currentRowChanged(int)), this, SIGNAL(currentRowChanged(int)));

	d->_ui->m_btnDel->setEnabled(d->_ui->m_list->count() > 0);
	d->_ui->m_btnDef->setEnabled(d->_ui->m_list->count() > 0);
	d->_ui->m_btnUp->setEnabled(d->_ui->m_list->currentRow() > 0);
	d->_ui->m_btnDown->setEnabled((d->_ui->m_list->currentRow() >= 0) && (d->_ui->m_list->currentRow() < d->_ui->m_list->count() - 1));
}

//! Destroys the widget
XinxListWidgetImpl::~XinxListWidgetImpl()
{

}

/*!
 * \brief This method is called when a new value is added into the list.
 * \since 0.10.0.0
 *
 * The default implementation of this method will add the result of defaultProposedValue() and if empty
 * will add \e ....
 *
 * \return The value to add in the list.
 */
QString XinxListWidgetImpl::addNewValue()
{
	return d->m_defaultProposedValue.isEmpty() ? "..." : d->m_defaultProposedValue;
}

/*!
 * \fn void XinxListWidgetImpl::defaultValueChanged(QString value)
 * \brief This signal is emited when the user changes the value \p value.
 * \param value Contains the default value, chosen by the user.
 */

/*!
 * \fn void XinxListWidgetImpl::currentRowChanged(int value)
 * \brief The signal is emited when the user changes the current row in the list.
 * \param value the index of the new row
 * \sa itemChanged()
 */

/*!
 * \fn void XinxListWidgetImpl::itemChanged(const QString & text)
 * \brief The signal is emited when the user changes the current row in the list.
 * \param text The text of the new selected item.
 * \sa currentRowChanged()
 */

/*!
 * \brief Retrieve the default value selected by the user.
 * \sa setDefaultValue()
 */
QString XinxListWidgetImpl::defaultValue() const
{
	if (d->_ui->m_list->item(d->m_defaultValue))
		return d->_ui->m_list->item(d->m_defaultValue)->text();
	else
		return QString();
}

/*!
 * \brief Changes the default value.
 *
 * If \p value isn't in the list, the value is added.
 *
 * \sa defaultValue();
 */
void XinxListWidgetImpl::setDefaultValue(const QString & value)
{
	d->updateDefault(value);
}

/*!
 * \brief Retrieve the visibility of the default button.
 * \sa setDefaultVisible()
 */
bool XinxListWidgetImpl::defaultVisible() const
{
	return d->_ui->m_btnDef->isVisible();
}

/*!
 * \brief Sets the visibility of the default button.
 *
 * If \p visible is set to false, the default button is hidden and the user can't choose a default value.
 *
 * \sa defaultVisible()
 */
void XinxListWidgetImpl::setDefaultVisible(bool visible)
{
	d->_ui->m_btnDef->setVisible(visible);
}

/*!
 * \brief Return the list of value used by the widget.
 * \sa setValues()
 */
QStringList XinxListWidgetImpl::values() const
{
	QStringList result;
	for (int i = 0; i < d->_ui->m_list->count(); i++)
	{
		result.append(d->_ui->m_list->item(i)->text());
	}
	return result;
}

/*!
 * \brief Changes the list of values used by the widget.
 *
 * If the defaultValue isn't in the list, the default value is added.
 * \sa values()
 */
void XinxListWidgetImpl::setValues(const QStringList & values)
{
	QString def = d->_ui->m_list->item(d->m_defaultValue) ? d->_ui->m_list->item(d->m_defaultValue)->text() : QString();

	d->_ui->m_list->clear();
	foreach(const QString & value, values)
	{
		QListWidgetItem * item = new QListWidgetItem(value, d->_ui->m_list);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		d->_ui->m_list->addItem(item);
	}
	d->_ui->m_btnDel->setEnabled(d->_ui->m_list->count() > 0);
	d->_ui->m_btnDef->setEnabled(d->_ui->m_list->count() > 0);
	d->_ui->m_btnUp->setEnabled(d->_ui->m_list->currentRow() > 0);
	d->_ui->m_btnDown->setEnabled((d->_ui->m_list->currentRow() >= 0) && (d->_ui->m_list->currentRow() < d->_ui->m_list->count() - 1));

	d->updateDefault(def);
}

//! Add a value in the list
void XinxListWidgetImpl::add(const QString & value)
{
	QListWidgetItem * item = new QListWidgetItem(value, d->_ui->m_list);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	d->_ui->m_list->addItem(item);
	d->_ui->m_list->setCurrentItem(item);

	d->_ui->m_btnDel->setEnabled(d->_ui->m_list->count() > 0);
	d->_ui->m_btnDef->setEnabled(d->_ui->m_list->count() > 0);
	d->_ui->m_btnUp->setEnabled(d->_ui->m_list->currentRow() > 0);
	d->_ui->m_btnDown->setEnabled((d->_ui->m_list->currentRow() >= 0) && (d->_ui->m_list->currentRow() < d->_ui->m_list->count() - 1));
}

/*!
 * \brief Retrieve the default value proposed to the user, when he adds a value to the list.
 * \sa setDefaultProposedValue()
 */
QString XinxListWidgetImpl::defaultProposedValue() const
{
	return d->m_defaultProposedValue;
}

/*!
 * \brief Sets the default proposed value to the user to \p value.
 * \sa defaultProposedValue()
 */
void XinxListWidgetImpl::setDefaultProposedValue(const QString & value)
{
	d->m_defaultProposedValue = value;
}


/*!
 * \brief Retrieve the current item of the list.
 */
QString XinxListWidgetImpl::currentItem() const
{
	if (d->_ui->m_list->currentItem())
	{
		return d->_ui->m_list->currentItem()->text();
	}
	else
	{
		return QString();
	}
}
