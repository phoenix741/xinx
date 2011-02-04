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
#include "xinxlistwidgetimpl.h"

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
XinxListWidgetImpl::XinxListWidgetImpl(QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	setupUi(this);

	connect(m_list, SIGNAL(currentRowChanged(int)), this, SIGNAL(currentRowChanged(int)));

	m_btnDel->setEnabled(m_list->count() > 0);
	m_btnDef->setEnabled(m_list->count() > 0);
	m_btnUp->setEnabled(m_list->currentRow() > 0);
	m_btnDown->setEnabled((m_list->currentRow() >= 0) && (m_list->currentRow() < m_list->count() - 1));
}

//! Destroys the widget
XinxListWidgetImpl::~XinxListWidgetImpl()
{

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

void XinxListWidgetImpl::updateDefault(const QString & def)
{
	m_defaultValue = -1;
	bool hasDefault = def.isEmpty() ? true : false;
	for (int i = 0; i < m_list->count() ; i++)
	{
		QListWidgetItem * item = m_list->item(i);
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
		QListWidgetItem * item = new QListWidgetItem(def, m_list);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		QFont font = item->font();
		font.setBold(true);
		item->setFont(font);
		m_list->addItem(item);
		m_defaultValue = m_list->count() - 1;
	}
}

/*!
 * \brief Retrieve the default value selected by the user.
 * \sa setDefaultValue()
 */
QString XinxListWidgetImpl::defaultValue() const
{
	if (m_list->item(m_defaultValue))
		return m_list->item(m_defaultValue)->text();
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
	updateDefault(value);
}

/*!
 * \brief Retrieve the visibility of the default button.
 * \sa setDefaultVisible()
 */
bool XinxListWidgetImpl::defaultVisible() const
{
	return m_btnDef->isVisible();
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
	m_btnDef->setVisible(visible);
}

/*!
 * \brief Return the list of value used by the widget.
 * \sa setValues()
 */
QStringList XinxListWidgetImpl::values() const
{
	QStringList result;
	for (int i = 0; i < m_list->count(); i++)
	{
		result.append(m_list->item(i)->text());
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
	QString def = m_list->item(m_defaultValue) ? m_list->item(m_defaultValue)->text() : QString();

	m_list->clear();
	foreach(const QString & value, values)
	{
		QListWidgetItem * item = new QListWidgetItem(value, m_list);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		m_list->addItem(item);
	}
	m_btnDel->setEnabled(m_list->count() > 0);
	m_btnDef->setEnabled(m_list->count() > 0);
	m_btnUp->setEnabled(m_list->currentRow() > 0);
	m_btnDown->setEnabled((m_list->currentRow() >= 0) && (m_list->currentRow() < m_list->count() - 1));

	updateDefault(def);
}

//! Add a value in the list
void XinxListWidgetImpl::add(const QString & value)
{
	QListWidgetItem * item = new QListWidgetItem(value, m_list);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	m_list->addItem(item);
	m_list->setCurrentItem(item);

	m_btnDel->setEnabled(m_list->count() > 0);
	m_btnDef->setEnabled(m_list->count() > 0);
	m_btnUp->setEnabled(m_list->currentRow() > 0);
	m_btnDown->setEnabled((m_list->currentRow() >= 0) && (m_list->currentRow() < m_list->count() - 1));
}

/*!
 * \brief Retrieve the default value proposed to the user, when he adds a value to the list.
 * \sa setDefaultProposedValue()
 */
QString XinxListWidgetImpl::defaultProposedValue() const
{
	return m_defaultProposedValue;
}

/*!
 * \brief Sets the default proposed value to the user to \p value.
 * \sa defaultProposedValue()
 */
void XinxListWidgetImpl::setDefaultProposedValue(const QString & value)
{
	m_defaultProposedValue = value;
}

void XinxListWidgetImpl::on_m_btnDef_clicked()
{
	QListWidgetItem * item = m_list->currentItem();
	if (item)
	{
		m_defaultValue = m_list->currentRow();
		updateDefault(item->text());
		emit defaultValueChanged(item->text());
	}
}

void XinxListWidgetImpl::on_m_btnAdd_clicked()
{
	QListWidgetItem * item = new QListWidgetItem(m_defaultProposedValue.isEmpty() ? "..." : m_defaultProposedValue, m_list);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	m_list->addItem(item);
	m_list->setCurrentItem(item);
	m_list->editItem(item);

	m_btnDel->setEnabled(m_list->count() > 0);
	m_btnDef->setEnabled(m_list->count() > 0);
	m_btnUp->setEnabled(m_list->currentRow() > 0);
	m_btnDown->setEnabled((m_list->currentRow() >= 0) && (m_list->currentRow() < m_list->count() - 1));
}

void XinxListWidgetImpl::on_m_btnDel_clicked()
{
	Q_ASSERT(m_list->currentRow() >= 0);

	delete m_list->currentItem();

	m_btnDel->setEnabled(m_list->count() > 0);
	m_btnDef->setEnabled(m_list->count() > 0);
	m_btnUp->setEnabled(m_list->currentRow() > 0);
	m_btnDown->setEnabled((m_list->currentRow() >= 0) && (m_list->currentRow() < m_list->count() - 1));
}

void XinxListWidgetImpl::on_m_btnUp_clicked()
{
	Q_ASSERT((m_list->currentRow() >= 0) && (m_list->currentRow() - 1 >= 0));

	int row = m_list->currentRow();
	QListWidgetItem * item = m_list->takeItem(m_list->currentRow());
	m_list->insertItem(row - 1, item);
	m_list->setCurrentItem(item);
}

void XinxListWidgetImpl::on_m_btnDown_clicked()
{
	Q_ASSERT((m_list->currentRow() >= 0) && (m_list->currentRow() + 1 < m_list->count()));

	int row = m_list->currentRow();
	QListWidgetItem * item = m_list->takeItem(m_list->currentRow());
	m_list->insertItem(row + 1, item);
	m_list->setCurrentItem(item);
}

void XinxListWidgetImpl::on_m_list_currentRowChanged(int row)
{
	m_btnUp->setEnabled(row > 0);
	m_btnDown->setEnabled(row < m_list->count() - 1);
}

void XinxListWidgetImpl::on_m_list_itemChanged(QListWidgetItem * item)
{
	if (item)
		emit itemChanged(item->text());
	else
		emit itemChanged(QString());
}
