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

#include "addmodifylabeldialog.h"
#include "ui_addmodifylabeldialog.h"
#include <translations/translationmanager.h>
#include <core/xinxconfig.h>

/* Static */

QString AddModifyLabelDialog::localLangToGenerixLang(const QString & lang)
{
	QLocale locale(lang);
	switch(locale.language())
	{
	case QLocale::C: /* GCE is a french product */
	case QLocale::French:
		return QString::fromAscii("FRA");
	case QLocale::German:
		return QString::fromAscii("DEU");
	case QLocale::English:
		return QString::fromAscii("ENG");
	case QLocale::Spanish:
		return QString::fromAscii("ESP");
	case QLocale::Italian:
		return QString::fromAscii("ITA");
	case QLocale::Portuguese:
		return QString::fromAscii("POR");
	default:
		return QString::fromAscii("UNK");
	}
}


/* AddModifyLabelDialog */

AddModifyLabelDialog::AddModifyLabelDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddModifyLabelDialog)
{
	ui->setupUi(this);

	foreach(TranslationManager::Language l, TranslationManager::self()->languages())
	{
		if (l.code == "C") continue; /* We don't process C */

		const QString localeLang = localLangToGenerixLang(l.code);
		ui->langueComboBox->addItem(QIcon(l.image), QString("(%1) %2").arg(localeLang).arg(l.name), localeLang);
	}

	const QString localeLang = localLangToGenerixLang(XINXConfig::self()->config().language);
	for (int i = 0; i < ui->langueComboBox->count(); i++)
	{
		if (ui->langueComboBox->itemData(i).toString() == localeLang)
		{
			ui->langueComboBox->setCurrentIndex(i);
			break;
		}
	}
}

AddModifyLabelDialog::~AddModifyLabelDialog()
{
	delete ui;
}

void AddModifyLabelDialog::setCode(const QString & value)
{
	ui->codeLineEdit->setText(value);
}

QString AddModifyLabelDialog::getCode() const
{
	return ui->codeLineEdit->text();
}

void AddModifyLabelDialog::setContext(const QString & value)
{
	ui->contextLineEdit->setText(value);
}

QString AddModifyLabelDialog::getContext() const
{
	return ui->contextLineEdit->text();
}

void AddModifyLabelDialog::setLang(const QString &value)
{
	int index = ui->langueComboBox->findData(value);
	ui->langueComboBox->setCurrentIndex(index);
}

QString AddModifyLabelDialog::getLang() const
{
	int index = ui->langueComboBox->currentIndex();
	if (index >= 0)
	{
		return ui->langueComboBox->itemData(index).toString();
	}
	else
	{
		return localLangToGenerixLang("C");
	}
}

void AddModifyLabelDialog::setDescription(const QString & value)
{
	ui->descriptionLineEdit->setText(value);
}

QString AddModifyLabelDialog::getDescription() const
{
	return ui->descriptionLineEdit->text();
}
