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
#include "customgeneralimpl.h"
#include <core/xinxconfig.h>
#include <directoryedit.h>
#include <translations/translationmanager.h>

// Qt header
#include <QStyleFactory>
#include <QDir>

/* CustomGeneralImpl */

CustomGeneralImpl::CustomGeneralImpl(QWidget * parent) : QWidget(parent)
{
	setupUi(this);

	m_traceLogWidget->lineEdit()->setFileMustExist(false);

	// Style
	m_styleComboBox->addItem(QString());
	foreach(const QString & style, QStyleFactory::keys())
	{
		m_styleComboBox->addItem(style);
	}
}

CustomGeneralImpl::~CustomGeneralImpl()
{
}

QPixmap CustomGeneralImpl::image()
{
	return QPixmap(":/images/preferences-general.png");
}

QString CustomGeneralImpl::name()
{
	return windowTitle();
}

QWidget * CustomGeneralImpl::settingsDialog()
{
	return this;
}

bool CustomGeneralImpl::loadSettingsDialog()
{
	// Language
	m_langComboBox->clear();
	m_langComboBox->addItem(QIcon(":/images/unknown.png"), "(C) ANSI", "C");
	m_langComboBox->setCurrentIndex(0);
	foreach(TranslationManager::Language l, TranslationManager::self()->languages())
	{
		m_langComboBox->addItem(QIcon(l.image), QString("(%1) %2").arg(l.code).arg(l.name), l.code);
	}

	for (int i = 0; i < m_langComboBox->count(); i++)
	{
		if (m_langComboBox->itemData(i).toString() == XINXConfig::self()->config().language)
		{
			m_langComboBox->setCurrentIndex(i);
			break;
		}
	}

	// Trace Log File
	m_traceLogWidget->lineEdit()->setText(QDir::toNativeSeparators(XINXConfig::self()->config().xinxTrace));

	// Style
	int currentStyle = m_styleComboBox->findText(XINXConfig::self()->config().style, Qt::MatchFixedString);
	m_styleComboBox->setCurrentIndex(currentStyle);

	// Show Shortcut near dock title
	m_shortcutDockTitle->setCheckState(XINXConfig::self()->config().dock.showShortcut ? Qt::Checked : Qt::Unchecked);

	return true;
}

bool CustomGeneralImpl::saveSettingsDialog()
{
	// Language
	XINXConfig::self()->config().language = m_langComboBox->itemData(m_langComboBox->currentIndex()).toString();

	// Trace Log File
	XINXConfig::self()->config().xinxTrace = QDir::fromNativeSeparators(m_traceLogWidget->lineEdit()->text());

	// Style
	XINXConfig::self()->config().style = m_styleComboBox->currentText();

	// Show Shortcut near dock title
	XINXConfig::self()->config().dock.showShortcut = m_shortcutDockTitle->isChecked();

	return true;
}

bool CustomGeneralImpl::cancelSettingsDialog()
{
	return true;
}

bool CustomGeneralImpl::isSettingsValid(QString& message)
{
	return true;
}

bool CustomGeneralImpl::isVisible()
{
	return true;
}


