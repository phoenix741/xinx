/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2010 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "newversionwizardimpl.h"
#include <core/xinxconfig.h>
#include "customdialog/toolsmodelindex.h"
#include "customdialog/directoryeditdelegate.h"
#include "customdialog/customdialogimpl.h"

// Qt header
#include <QTextCodec>

NewVersionWizardImpl::NewVersionWizardImpl()
{
	setupUi(this);

	init();
}

NewVersionWizardImpl::NewVersionWizardImpl(QWidget * parent) : QWizard(parent)
{
	setupUi(this);

	init();
}

NewVersionWizardImpl::~NewVersionWizardImpl()
{
}

void NewVersionWizardImpl::init()
{
	// Font encodings
	QList<QByteArray> encodings = QTextCodec::availableCodecs();
	qSort(encodings);
	foreach(const QByteArray & encoding, encodings)
	{
		m_encodingComboBox->addItem(encoding);
	}
}

void NewVersionWizardImpl::loadFromConfig()
{
	// Language
	m_langComboBox->setCurrentIndex(0);
	for (int i = 0; i < m_langComboBox->count(); i++)
	{
		if (m_langComboBox->itemText(i).contains(QString("(%1)").arg(XINXConfig::self()->config().language)))
		{
			m_langComboBox->setCurrentIndex(i);
			break;
		}
	}

	// Default project directory
	m_projectPathLineEdit->lineEdit()->setText(QDir::toNativeSeparators(XINXConfig::self()->config().project.defaultPath));

	// Refresh Directory Timeout
	m_autoRefreshDirectoryChk->setChecked(XINXConfig::self()->config().project.automaticProjectDirectoryRefreshTimeout > 0);

	// Open the last project at start
	m_openLastProjectCheckBox->setChecked(XINXConfig::self()->config().project.openTheLastProjectAtStart);

	// Default text encoding.
	m_encodingComboBox->setCurrentIndex(m_encodingComboBox->findText(XINXConfig::self()->config().editor.defaultTextCodec));

	// Tools
	m_tools = XINXConfig::self()->config().tools;

	// Tools
	ToolsModelIndex * toolsModel = new ToolsModelIndex(&(m_tools), m_toolsTable);
	m_toolsTable->setModel(toolsModel);
	m_toolsTable->setItemDelegate(new DirectoryEditDelegate(m_toolsTable));
	m_toolsTable->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
	m_toolsTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

	// Create backup file when saving
	m_createBakCheckBox->setChecked(XINXConfig::self()->config().editor.createBackupFile);

	// Popup when file modified
	m_popupWhenFileModifiedCheckBox->setChecked(XINXConfig::self()->config().editor.popupWhenFileModified);

	int cnt = 0;
	if (XINXConfig::self()->config().editor.showTabulationAndSpace) cnt++;
	if (XINXConfig::self()->config().editor.highlightCurrentLine) cnt++;
	if (XINXConfig::self()->config().editor.autoHighlight) cnt++;
	if (XINXConfig::self()->config().editor.autoindentOnSaving == "true") cnt++;

	m_listSlider->setValue(cnt);

	// Show tabulation and space in the editor
	m_listEffect->item(0)->setCheckState(XINXConfig::self()->config().editor.showTabulationAndSpace ? Qt::Checked : Qt::Unchecked);

	// Show current line
	m_listEffect->item(1)->setCheckState(XINXConfig::self()->config().editor.highlightCurrentLine ? Qt::Checked : Qt::Unchecked);

	// Auto highlight text
	m_listEffect->item(2)->setCheckState(XINXConfig::self()->config().editor.autoHighlight ? Qt::Checked : Qt::Unchecked);

	// Pretty print on saving
	m_listEffect->item(3)->setCheckState(XINXConfig::self()->config().editor.autoindentOnSaving == "true" ? Qt::Checked : Qt::Unchecked);
}

void NewVersionWizardImpl::saveToConfig()
{
	// Language
	QRegExp exp("^\\((.*)\\).*$");
	if (exp.indexIn(m_langComboBox->currentText()) >= 0)
		XINXConfig::self()->config().language = exp.cap(1);

	// Default project directory
	XINXConfig::self()->config().project.defaultPath = QDir::fromNativeSeparators(m_projectPathLineEdit->lineEdit()->text());

	// Refresh Directory Timeout
	XINXConfig::self()->config().project.automaticProjectDirectoryRefreshTimeout = m_autoRefreshDirectoryChk->isChecked() ? 500 : 0;

	// Open the last project at start
	XINXConfig::self()->config().project.openTheLastProjectAtStart = m_openLastProjectCheckBox->isChecked();

	// Default text encoding.
	XINXConfig::self()->config().editor.defaultTextCodec = m_encodingComboBox->itemText(m_encodingComboBox->currentIndex());

	// Tools
	XINXConfig::self()->config().tools = m_tools;

	// Create backup file when saving
	XINXConfig::self()->config().editor.createBackupFile = m_createBakCheckBox->isChecked();

	// Popup when file modified
	XINXConfig::self()->config().editor.popupWhenFileModified = m_popupWhenFileModifiedCheckBox->isChecked();

	// Show tabulation and space in the editor
	XINXConfig::self()->config().editor.showTabulationAndSpace = m_listEffect->item(0)->checkState() == Qt::Checked;

	// Show current line
	XINXConfig::self()->config().editor.highlightCurrentLine = m_listEffect->item(1)->checkState() == Qt::Checked;

	// Auto highlight text
	XINXConfig::self()->config().editor.autoHighlight = m_listEffect->item(2)->checkState() == Qt::Checked;

	// Pretty print on saving
	if (m_listEffect->item(3)->checkState() == Qt::Checked)
		XINXConfig::self()->config().editor.autoindentOnSaving = "true";
	else
		XINXConfig::self()->config().editor.autoindentOnSaving = "false";
}

void NewVersionWizardImpl::on_m_listSlider_valueChanged(int value)
{
	for (int i = 0; i < m_listEffect->count(); i++)
	{
		m_listEffect->item(i)->setCheckState(i < value ? Qt::Checked : Qt::Unchecked);
	}
}

void NewVersionWizardImpl::accept()
{
	XINXConfig::self()->save();

	if (m_customizeChk->isChecked())
	{
		CustomDialogImpl custom(this);
		custom.loadConfig();

		custom.exec();
	}

	QWizard::accept();
}
