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
#include "scriptvalue.h"

// Qt header
#include <QApplication>

/* ScriptValue */

/*!
 * \class ScriptValue
 * \ingroup XinxScript
 * \brief Script object that can be used to execute the script.
 *
 * This method can be used to execute the the script, and provide access to methode
 * used before and after load and save.
 *
 */

/*!
 * \brief Create an empty script object.
 */
ScriptValue::ScriptValue()
{
}

/*!
 * \brief Create a script object from \e value (the ECMAScript object).
 */
ScriptValue::ScriptValue(QScriptValue value) : m_value(value)
{

}

//! Return the ECMAScript object
const QScriptValue & ScriptValue::value() const
{
	return m_value;
}

//! Return the description of the script object
QString ScriptValue::text() const
{
	return m_value.property("text").toString();
}

//! Return true if there is a methode to call before saving
bool ScriptValue::isCallBeforeSave() const
{
	return m_value.property("beforeSave").isFunction();
}

//! Call the script methode called "beforeSave"
void ScriptValue::callScriptBeforeSave()
{
	QScriptValue result = m_value.property("beforeSave").call(m_value);
	if (result.isError())
	{
		qWarning(qPrintable(qApp->translate("ScriptValue", "An error occure while run the script : %1").arg(result.toString())));
	}
}

//! Return true if there is a methode to call before load
bool ScriptValue::isCallBeforeLoad() const
{
	return m_value.property("beforeLoad").isFunction();
}

//! Call the script methode called "beforeLoad"
void ScriptValue::callScriptBeforeLoad()
{
	QScriptValue result = m_value.property("beforeLoad").call(m_value);
	if (result.isError())
	{
		qWarning(qPrintable(qApp->translate("ScriptValue", "An error occure while run the script : %1").arg(result.toString())));
	}
}

//! Return true if there is a methode to call after save
bool ScriptValue::isCallAfterSave() const
{
	return m_value.property("afterSave").isFunction();
}

//! Call the script methode called "afterSave"
void ScriptValue::callScriptAfterSave()
{
	QScriptValue result = m_value.property("afterSave").call(m_value);
	if (result.isError())
	{
		qWarning(qPrintable(qApp->translate("ScriptValue", "An error occure while run the script : %1").arg(result.toString())));
	}
}

//! Return true if there is a methode to call after load
bool ScriptValue::isCallAfterLoad() const
{
	return m_value.property("afterLoad").isFunction();
}

//! Call the script methode called "afterLoad"
void ScriptValue::callScriptAfterLoad()
{
	QScriptValue result = m_value.property("afterLoad").call(m_value);
	if (result.isError())
	{
		qWarning(qPrintable(qApp->translate("ScriptValue", "An error occure while run the script : %1").arg(result.toString())));
	}
}

//! Call the script (used for manual call)
void ScriptValue::callScript()
{
	QScriptValue result = m_value.property("run").call(m_value);
	if (result.isError())
	{
		qWarning(qPrintable(qApp->translate("ScriptValue", "An error occure while run the script : %1").arg(result.toString())));
	}
}
