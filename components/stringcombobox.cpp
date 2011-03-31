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
#include "stringcombobox.h"

/*!
 * \ingroup Components
 * \class StringComboBox
 * \since 0.9.0.0
 *
 * \brief The class StringComboBox provides a extension of QComboBox to select an item.
 *
 * \bc 0.10.0.0
 *
 * This special combo box adds a property value that can be used
 * to change dynamically the current index with a string value, instead
 * of change the value of the lineedit.
 *
 * This combobox can be used by QDataWidgetMapper who change the user property of the widget.
 */

/*!
 * \brief Creates the StringComboBox with parent \p parent.
 * \param parent The parent of the combobox.
 */
StringComboBox::StringComboBox(QWidget * parent) : QComboBox(parent)
{
}

//! Destroy the combobox
StringComboBox::~StringComboBox()
{
}

/*!
 * \property StringComboBox::value()
 * \brief Returns the current value of the combobox.
 *
 * The current value is equivalent to the currentText method.
 * \sa QComboBox::currentText(), setValue()
 */
QString StringComboBox::value() const
{
	return currentText();
}

/*!
 * \brief Sets the value of the combobox to \p value.
 *
 * If \p value can't be find in the combobox, there is no selection.
 * \param value The value to select.
 * \sa value()
 */
void StringComboBox::setValue(const QString & value)
{
	setCurrentIndex(findText(value));
}
