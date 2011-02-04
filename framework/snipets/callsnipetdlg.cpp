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
#include "core/xinxconfig.h"
#include "snipets/callsnipetdlg.h"
#include "snipets/snipetmanager.h"
#include "snipets/snipet.h"

// Qt header
#include <QSqlQuery>
#include <QDebug>

/* SnipetParameterNameItem */

/*!
 * \class SnipetParameterNameItem
 * \internal
 */
class SnipetParameterNameItem : public QTableWidgetItem
{
public:
	SnipetParameterNameItem();
	SnipetParameterNameItem(const QString & name);
private:

};

//! \internal
SnipetParameterNameItem::SnipetParameterNameItem() : QTableWidgetItem()
{
}

//! \internal
SnipetParameterNameItem::SnipetParameterNameItem(const QString & name) : QTableWidgetItem()
{
	setData(Qt::DisplayRole, name);
}

/* SnipetParameterValueItem */

/*!
 * \class SnipetParameterValueItem
 * \internal
 */
class SnipetParameterValueItem : public QTableWidgetItem
{
public:
	SnipetParameterValueItem();
	SnipetParameterValueItem(const QString & defaultValue);

	void setDefault();
	void setDefaultValue(const QString & value);
	const QString & defaultValue() const;
private:
	QString m_defaultValue;
};

//! \internal
SnipetParameterValueItem::SnipetParameterValueItem() : QTableWidgetItem()
{
}

//! \internal
SnipetParameterValueItem::SnipetParameterValueItem(const QString & defaultValue) : QTableWidgetItem(), m_defaultValue(defaultValue)
{
	setData(Qt::DisplayRole, m_defaultValue);
}

//! \internal
void SnipetParameterValueItem::setDefault()
{
	setData(Qt::DisplayRole, m_defaultValue);
}

//! \internal
void SnipetParameterValueItem::setDefaultValue(const QString & value)
{
	if (m_defaultValue != value)
	{
		if (data(Qt::DisplayRole) == m_defaultValue)
		{
			setData(Qt::DisplayRole, m_defaultValue);
		}
		m_defaultValue = value;
	}
}

//! \internal
const QString & SnipetParameterValueItem::defaultValue() const
{
	return m_defaultValue;
}

/* CallSnipetDialogImpl */

/*!
 * \ingroup Snipets
 * \class CallSnipetDialogImpl
 * \since 0.9.0.0
 *
 * \brief Dialog to change parameters on snipet before add result to the text.
 *
 * This dialog permit to change parameters on snipet before replace it in the
 * editor.
 * This dialog have a part to change each parameters and another part to show a result.
 * The result contains the execution of the script.
 *
 * \image html callsnipetdialogimpl.png
 *
 */

/*!
 * \brief Constructor of the snipet dialog implementation.
 *
 * We defines a default windows flags. The windows can't be resize.
 * \param db A database
 * \param snipetId The id of the snipet
 * \param parent The parent of the dialog
 * \param f Flags to use on Windows. By default, the dialog have a fixed size.
 */
CallSnipetDialogImpl::CallSnipetDialogImpl(QSqlDatabase db, int snipetId, QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(db, snipetId, this);
}

/*!
 * \brief Destroy the dialog.
 */
CallSnipetDialogImpl::~CallSnipetDialogImpl()
{

}

/*!
 * \brief Return the result's text of the snipet.
 *
 * The result text has each parameters replaced and ECMAScript parsed.
 */
const QString & CallSnipetDialogImpl::snipetText() const
{
	return m_snipetText;
}

/*!
 * \brief Return the list of parameters modified and for each parameters the value.
 */
QStringList CallSnipetDialogImpl::values() const
{
	QStringList parameters;
	for (int row = 0 ; row < m_paramTableWidget->rowCount() ; row++)
	{
		parameters += m_paramTableWidget->item(row, 1)->data(Qt::DisplayRole).toString();
	}
	return parameters;
}

/*!
 * \brief Execute the dialog box.
 *
 * This method is show only if XINXConfig::self()->config().snipets.alwaysShowDialog or
 * if showDialog in the snipet is to true.
 */
int CallSnipetDialogImpl::exec()
{
	if (XINXConfig::self()->config().snipets.alwaysShowDialog || m_showDialog)
	{
		return QDialog::exec();
	}
	else
	{
		return QDialog::Accepted;
	}
}

void CallSnipetDialogImpl::on_m_tabWidget_currentChanged(int value)
{
	if (value == 1)    // Show result tab, so we calculate the result
	{
		QString result;
		QStringList parameters = values();
		if (SnipetManager::self()->executeSnipetScript(m_snipetText, parameters, &result))
			m_resultEdit->setPlainText(result);
		else
			m_resultEdit->setPlainText(tr("Cannot execute the snipet. Please check the script in the snipet in configuration dialog."));
	}
}

void CallSnipetDialogImpl::setupUi(QSqlDatabase db, int snipetId, QDialog * parent)
{
	Ui::CallSnipetDialog::setupUi(parent);

	/* Initialise the snipet description */
	QSqlQuery snipetQuery("SELECT name, description, text, auto, show_dialog FROM snipets WHERE id=:id", db);
	snipetQuery.bindValue(":id", snipetId);

	bool result = snipetQuery.exec();
	Q_ASSERT(result);

	if (! snipetQuery.next())
	{
		qCritical() << tr("CallSnipetDialogImpl: Can't find snipet id '%1'").arg(snipetId);
		return;
	}

	setWindowTitle(tr("Use the snipet \"%1\"").arg(snipetQuery.value(0).toString()));
	m_labelName->setText("<b>" + snipetQuery.value(0).toString() + "</b>");
	m_descriptionLabel->setText(snipetQuery.value(1).toString());
	m_snipetText = snipetQuery.value(2).toString();
	m_isAutomatic = snipetQuery.value(3).toBool();
	m_showDialog  = snipetQuery.value(4).toBool();

	/* Initialise the snipet parameter */
	QSqlQuery paramsQuery("SELECT name, default_value FROM snipets_params WHERE snipet_id=:id ORDER BY id", db);
	paramsQuery.bindValue(":id", snipetId);

	result = paramsQuery.exec();
	Q_ASSERT(result);

	int row = 0;
	while (paramsQuery.next())
	{
		m_paramTableWidget->setRowCount(row + 1);

		SnipetParameterNameItem * name = new SnipetParameterNameItem(paramsQuery.value(0).toString());
		SnipetParameterValueItem * defaultValue = new SnipetParameterValueItem(paramsQuery.value(1).toString());

		m_paramTableWidget->setVerticalHeaderItem(row, new QTableWidgetItem(tr("Parameter %1").arg(row)));
		m_paramTableWidget->setItem(row, 0, name);
		m_paramTableWidget->setItem(row, 1, defaultValue);

		row++;
	}
}

