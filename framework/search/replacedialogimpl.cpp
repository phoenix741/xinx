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

// Qt header
#include <QLineEdit>
#include <QCompleter>

// Xinx header
#include "replacedialogimpl.h"
#include <project/xinxprojectmanager.h>
#include <plugins/xinxpluginsloader.h>

ReplaceDialogImpl::ReplaceDialogImpl(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(this);

	m_findButton = m_buttonBox->button(QDialogButtonBox::Ok);
	m_findButton->setText(tr("Find"));
	m_findButton->setIcon(QIcon(":/images/find.png"));

	connect(m_findButton, SIGNAL(clicked()), this, SLOT(m_findButton_clicked()));
}
//

void ReplaceDialogImpl::on_m_replaceCheckBox_toggled(bool checked)
{
	if (checked)
		m_findButton->setText(tr("Replace"));
	else
		m_findButton->setText(tr("Find"));
}

void ReplaceDialogImpl::initialize(bool hasEditor)
{
	m_searchAllRadioButton->setEnabled(hasEditor);
	m_searchSelectionRadioButton->setEnabled(hasEditor);
	m_searchAllRadioButton->setChecked(hasEditor);
	m_projectFilesRadioButton->setChecked((! hasEditor) && (XinxProject::Manager::self()->projects().count() > 0));
	m_customFilesRadioButton->setChecked((! hasEditor) && (XinxProject::Manager::self()->projects().count() == 0));
	on_m_extedButtonGroup_buttonClicked(m_extedButtonGroup->checkedButton());

	m_comboFind->lineEdit()->selectAll();
	m_comboFind->lineEdit()->setFocus(Qt::ActiveWindowFocusReason);

	XinxProject::ProjectPtr selectedProject = XinxProject::Manager::self()->selectedProject().toStrongRef();
	if (selectedProject)
	{
		m_directoryWidget->setDefaultValue(selectedProject->projectPath());
		m_directoryWidget->lineEdit()->setText(selectedProject->projectPath());
	}
	else
	{
		m_directoryWidget->setDefaultValue(QString());
		m_directoryWidget->lineEdit()->setText(QString());
	}

	QStringList filters = XinxPluginsLoader::self()->managedFilters();
	filters.removeDuplicates ();
	m_comboExt->clear ();
	m_comboExt->addItems(filters);
	m_comboExt->setCheckedItems(filters);

	m_findButton->setDefault(true);
}

void ReplaceDialogImpl::setText(const QString & str)
{
	m_comboFind->setEditText(str);
	m_comboFind->completer()->setCompletionPrefix(str);
}

void ReplaceDialogImpl::setReplace(bool value)
{
	m_replaceCheckBox->setChecked(value) ;
}

void ReplaceDialogImpl::on_m_extedButtonGroup_buttonClicked(QAbstractButton* button)
{
	if (button == m_searchAllRadioButton)
	{
		m_whereLabel->setVisible(false);
		m_directoryWidget->setVisible(false);
		m_extLabel->setVisible(false);
		m_comboExt->setVisible(false);
		m_directionGroupBox->setEnabled(true);
		m_fromStartCheckBox->setEnabled(true);
	}
	else if (button == m_searchSelectionRadioButton)
	{
		m_whereLabel->setVisible(false);
		m_directoryWidget->setVisible(false);
		m_extLabel->setVisible(false);
		m_comboExt->setVisible(false);
		m_directionGroupBox->setEnabled(true);
		m_fromStartCheckBox->setEnabled(true);
	}
	else if (button == m_projectFilesRadioButton)
	{
		m_whereLabel->setVisible(false);
		m_directoryWidget->setVisible(false);
		m_extLabel->setVisible(true);
		m_comboExt->setVisible(true);
		m_directionGroupBox->setEnabled(false);
		m_fromStartCheckBox->setEnabled(false);
	}
	else if (button == m_customFilesRadioButton)
	{
		m_whereLabel->setVisible(true);
		m_directoryWidget->setVisible(true);
		m_extLabel->setVisible(true);
		m_comboExt->setVisible(true);
		m_directionGroupBox->setEnabled(false);
		m_fromStartCheckBox->setEnabled(false);
	}
}

void ReplaceDialogImpl::m_findButton_clicked()
{
	if (! m_comboFind->lineEdit()->text().isEmpty() && (m_comboFind->findText(m_comboFind->lineEdit()->text()) == -1))
		m_comboFind->addItem(m_comboFind->lineEdit()->text());
	if (! m_comboReplace->lineEdit()->text().isEmpty() && (m_comboReplace->findText(m_comboFind->lineEdit()->text()) == -1))
		m_comboReplace->addItem(m_comboReplace->lineEdit()->text());

	AbstractEditor::SearchOptions options;
	if (m_searchSelectionRadioButton->isChecked())                  options |= AbstractEditor::ONLY_SELECTION;
	if (m_UpRadioButton->isChecked())                               options |= AbstractEditor::BACKWARD;
	if (m_caseCheckBox->checkState() == Qt::Checked)                options |= AbstractEditor::MATCH_CASE;
	if (m_fromStartCheckBox->checkState() == Qt::Checked)           options |= AbstractEditor::SEARCH_FROM_START;
	if (m_wholeWordsCheckBox->checkState() == Qt::Checked)          options |= AbstractEditor::WHOLE_WORDS;
	if (m_regularExpressionCheckBox->checkState() == Qt::Checked)   options |= AbstractEditor::REGULAR_EXPRESSION;

	if (!(m_projectFilesRadioButton->isChecked() || m_customFilesRadioButton->isChecked()))
	{
		if (m_replaceCheckBox->checkState() == Qt::Checked)
			emit find(m_comboFind->lineEdit()->text(), m_comboReplace->lineEdit()->text(), options);
		else
			emit find(m_comboFind->lineEdit()->text(), QString(), options);
	}
	else
	{
		QStringList directories;
		if (m_customFilesRadioButton->isChecked())
		{
			directories << QDir::fromNativeSeparators(m_directoryWidget->lineEdit()->text());
		}
		else
		{
			foreach(XinxProject::ProjectPtr project, XinxProject::Manager::self()->projects())
			{
				directories << project->projectPath();
			}
		}

		if (m_replaceCheckBox->checkState() == Qt::Checked)
			emit findInFiles(directories, m_comboExt->checkedItems (), m_comboFind->lineEdit()->text(), m_comboReplace->lineEdit()->text(), options);
		else
			emit findInFiles(directories, m_comboExt->checkedItems (), m_comboFind->lineEdit()->text(), QString(), options);
	}
}

