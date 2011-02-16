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
#include "customeditorimpl.h"
#include <core/xinxconfig.h>

// Qt header
#include <QTextCodec>

/* CustomEditorImpl */

CustomEditorImpl::CustomEditorImpl(QWidget * parent) : QWidget(parent)
{
	setupUi(this);

	// Font encodings
	QList<QByteArray> encodings = QTextCodec::availableCodecs();
	qSort(encodings);
	foreach(const QByteArray & encoding, encodings)
	{
		m_encodingComboBox->addItem(encoding);
	}

#if QT_VERSION <= 0x040500
	m_closeBtnGrp->setEnabled(false);
#endif
}

CustomEditorImpl::~CustomEditorImpl()
{
}

QPixmap CustomEditorImpl::image()
{
	return QPixmap(":/images/preferences-editor.png");
}

QString CustomEditorImpl::name()
{
	return windowTitle();
}

QWidget * CustomEditorImpl::settingsDialog()
{
	return this;
}

bool CustomEditorImpl::loadSettingsDialog()
{
	// Create backup file when saving
	m_createBakCheckBox->setChecked(XINXConfig::self()->config().editor.createBackupFile);

	// Popup when file modified
	m_popupWhenFileModifiedCheckBox->setChecked(XINXConfig::self()->config().editor.popupWhenFileModified);

	// Pretty print on saving
	m_prettyPrintNeverRadioButton->setChecked(XINXConfig::self()->config().editor.autoindentOnSaving == "false");
	m_prettyPrintOnSavingRadioButton->setChecked(XINXConfig::self()->config().editor.autoindentOnSaving == "true");
	m_prettyPrintOnClosingRadioButton->setChecked(XINXConfig::self()->config().editor.autoindentOnSaving == "closing");

	// Tab close button
	if (XINXConfig::self()->config().editor.closeButtonOnEachTab)
	{
		if (XINXConfig::self()->config().editor.hideCloseTab)
			m_closeBtnRadioButton->setChecked(true);
		else
			m_closeAndCornerBtnRadioButton->setChecked(true);
	}
	else
		m_cornerBtnRadioButton->setChecked(true);

	// Default text encoding.
	m_encodingComboBox->setCurrentIndex(m_encodingComboBox->findText(XINXConfig::self()->config().editor.defaultTextCodec));

	// Text content refresh
	m_refreshContentTimeoutSpinBox->setValue(XINXConfig::self()->config().editor.automaticModelRefreshTimeout);

	// Wrap Line
	m_wrapLineChk->setChecked(XINXConfig::self()->config().editor.wrapLine);

	// Autoclose error dock
	m_autoCloseError->setChecked(XINXConfig::self()->config().editor.closeErrorDockAutomatically);

	return true;
}

bool CustomEditorImpl::saveSettingsDialog()
{
	// Create backup file when saving
	XINXConfig::self()->config().editor.createBackupFile = m_createBakCheckBox->isChecked();

	// Popup when file modified
	XINXConfig::self()->config().editor.popupWhenFileModified = m_popupWhenFileModifiedCheckBox->isChecked();

	// Pretty print on saving
	if (m_prettyPrintNeverRadioButton->isChecked())
	{
		XINXConfig::self()->config().editor.autoindentOnSaving = "false";
	}
	else if (m_prettyPrintOnSavingRadioButton->isChecked())
	{
		XINXConfig::self()->config().editor.autoindentOnSaving = "true";
	}
	else if (m_prettyPrintOnClosingRadioButton->isChecked())
	{
		XINXConfig::self()->config().editor.autoindentOnSaving = "closing";
	}

	// Tab close button
	if (m_closeBtnRadioButton->isChecked())
	{
		XINXConfig::self()->config().editor.closeButtonOnEachTab = true;
		XINXConfig::self()->config().editor.hideCloseTab = true;
	}
	else if (m_cornerBtnRadioButton->isChecked())
	{
		XINXConfig::self()->config().editor.closeButtonOnEachTab = false;
		XINXConfig::self()->config().editor.hideCloseTab = false;
	}
	else
	{
		XINXConfig::self()->config().editor.closeButtonOnEachTab = true;
		XINXConfig::self()->config().editor.hideCloseTab = false;
	}

	// Default text encoding.
	XINXConfig::self()->config().editor.defaultTextCodec = m_encodingComboBox->itemText(m_encodingComboBox->currentIndex());

	// Text content refresh
	XINXConfig::self()->config().editor.automaticModelRefreshTimeout = m_refreshContentTimeoutSpinBox->value();

	// Wrap Line
	XINXConfig::self()->config().editor.wrapLine = m_wrapLineChk->isChecked();

	// Autoclose error dock
	XINXConfig::self()->config().editor.closeErrorDockAutomatically = m_autoCloseError->isChecked();

	return true;
}

bool CustomEditorImpl::cancelSettingsDialog()
{
	return true;
}

bool CustomEditorImpl::isSettingsValid()
{
	return true;
}

bool CustomEditorImpl::isVisible()
{
	return true;
}
