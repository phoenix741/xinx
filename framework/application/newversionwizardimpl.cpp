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
#include "newversionwizardimpl.h"
#include <core/xinxconfig.h>
#include "customdialog/toolsmodelindex.h"
#include "customdialog/directoryeditdelegate.h"
#include "customdialog/customdialogimpl.h"
#include "ui_newversion.h"
#include <directoryedit.h>

// Qt header
#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>

/* PrivateNewVersionWizardImpl */

class PrivateNewVersionWizardImpl
{
public:
	PrivateNewVersionWizardImpl(NewVersionWizardImpl * wizard);
	~PrivateNewVersionWizardImpl();

	Ui::NewVersionWizard * _ui;
	QHash<QString,QString> _tools;

	void init();
};

PrivateNewVersionWizardImpl::PrivateNewVersionWizardImpl(NewVersionWizardImpl * wizard)
{
	_ui = new Ui::NewVersionWizard;
	_ui->setupUi(wizard);

	init();
}

PrivateNewVersionWizardImpl::~PrivateNewVersionWizardImpl()
{
	delete _ui;
}

void PrivateNewVersionWizardImpl::init()
{
	// Licence
	QFile licenceFile(":/document/licence.html");
	if (licenceFile.open(QIODevice::ReadOnly))
	{
		QTextStream textStream(&licenceFile);

		_ui->_licenceWidget->setHtml(textStream.readAll());
	}

	// Font encodings
	QList<QByteArray> encodings = QTextCodec::availableCodecs();
	qSort(encodings);
	foreach(const QByteArray & encoding, encodings)
	{
		_ui->m_encodingComboBox->addItem(encoding);
	}
}



/* NewVersionWizardImpl */

NewVersionWizardImpl::NewVersionWizardImpl()
{
	d = new PrivateNewVersionWizardImpl(this);
	connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(slotCurrentIdChanged(int)));
}

NewVersionWizardImpl::NewVersionWizardImpl(QWidget * parent) : QWizard(parent)
{
	d = new PrivateNewVersionWizardImpl(this);
	connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(slotCurrentIdChanged(int)));
}

NewVersionWizardImpl::~NewVersionWizardImpl()
{
	delete d;
}

bool NewVersionWizardImpl::validateCurrentPage()
{
	if (currentPage() == d->_ui->applicationPage)
	{
		if (! QDir(QDir::fromNativeSeparators(d->_ui->m_projectPathLineEdit->lineEdit()->text())).exists())
		{
			QMessageBox::critical(this, tr("Application page"), tr("The default open location for project must be a valid path."));
			return false;
		}
	}
    return QWizard::validateCurrentPage();
}

void NewVersionWizardImpl::loadFromConfig()
{
	// Language
	d->_ui->m_langComboBox->setCurrentIndex(0);
	for (int i = 0; i <d->_ui-> m_langComboBox->count(); i++)
	{
		if (d->_ui->m_langComboBox->itemText(i).contains(QString("(%1)").arg(XINXConfig::self()->config().language)))
		{
			d->_ui->m_langComboBox->setCurrentIndex(i);
			break;
		}
	}

	// Default project directory
	d->_ui->m_projectPathLineEdit->lineEdit()->setText(QDir::toNativeSeparators(XINXConfig::self()->config().project.defaultPath));

	// Refresh Directory Timeout
	d->_ui->m_autoRefreshDirectoryChk->setChecked(XINXConfig::self()->config().project.automaticProjectDirectoryRefreshTimeout > 0);

	// Open the last project at start
	d->_ui->m_openLastSessionCheckBox->setChecked(XINXConfig::self()->config().project.openTheLastSessionAtStart);

	// Default text encoding.
	d->_ui->m_encodingComboBox->setCurrentIndex(d->_ui->m_encodingComboBox->findText(XINXConfig::self()->config().editor.defaultTextCodec));

	// Tools
	d->_tools = XINXConfig::self()->config().tools;

	// Tools
	ToolsModelIndex * toolsModel = new ToolsModelIndex(&(d->_tools), d->_ui->m_toolsTable);
	d->_ui->m_toolsTable->setModel(toolsModel);
	d->_ui->m_toolsTable->setItemDelegate(new DirectoryEditDelegate(d->_ui->m_toolsTable));
	d->_ui->m_toolsTable->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
	d->_ui->m_toolsTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

	// Create backup file when saving
	d->_ui->m_createBakCheckBox->setChecked(XINXConfig::self()->config().editor.createBackupFile);

	// Popup when file modified
	d->_ui->m_popupWhenFileModifiedCheckBox->setChecked(XINXConfig::self()->config().editor.popupWhenFileModified);

	int cnt = 0;
	if (XINXConfig::self()->config().editor.showTabulationAndSpace) cnt++;
	if (XINXConfig::self()->config().editor.highlightCurrentLine) cnt++;
	if (XINXConfig::self()->config().editor.autoHighlight) cnt++;
	if (XINXConfig::self()->config().editor.autoindentOnSaving == "true") cnt++;

	d->_ui->m_listSlider->setValue(cnt);

	// Show tabulation and space in the editor
	d->_ui->m_listEffect->item(0)->setCheckState(XINXConfig::self()->config().editor.showTabulationAndSpace ? Qt::Checked : Qt::Unchecked);

	// Show current line
	d->_ui->m_listEffect->item(1)->setCheckState(XINXConfig::self()->config().editor.highlightCurrentLine ? Qt::Checked : Qt::Unchecked);

	// Auto highlight text
	d->_ui->m_listEffect->item(2)->setCheckState(XINXConfig::self()->config().editor.autoHighlight ? Qt::Checked : Qt::Unchecked);

	// Pretty print on saving
	d->_ui->m_listEffect->item(3)->setCheckState(XINXConfig::self()->config().editor.autoindentOnSaving == "true" ? Qt::Checked : Qt::Unchecked);
}

void NewVersionWizardImpl::saveToConfig()
{
	// Language
	QRegExp exp("^\\((.*)\\).*$");
	if (exp.indexIn(d->_ui->m_langComboBox->currentText()) >= 0)
		XINXConfig::self()->config().language = exp.cap(1);

	// Default project directory
	XINXConfig::self()->config().project.defaultPath = QDir::fromNativeSeparators(d->_ui->m_projectPathLineEdit->lineEdit()->text());

	// Refresh Directory Timeout
	XINXConfig::self()->config().project.automaticProjectDirectoryRefreshTimeout = d->_ui->m_autoRefreshDirectoryChk->isChecked() ? 500 : 0;

	// Open the last project at start
	XINXConfig::self()->config().project.openTheLastSessionAtStart = d->_ui->m_openLastSessionCheckBox->isChecked();

	// Default text encoding.
	XINXConfig::self()->config().editor.defaultTextCodec = d->_ui->m_encodingComboBox->itemText(d->_ui->m_encodingComboBox->currentIndex());

	// Tools
	XINXConfig::self()->config().tools = d->_tools;

	// Create backup file when saving
	XINXConfig::self()->config().editor.createBackupFile = d->_ui->m_createBakCheckBox->isChecked();

	// Popup when file modified
	XINXConfig::self()->config().editor.popupWhenFileModified = d->_ui->m_popupWhenFileModifiedCheckBox->isChecked();

	// Show tabulation and space in the editor
	XINXConfig::self()->config().editor.showTabulationAndSpace = d->_ui->m_listEffect->item(0)->checkState() == Qt::Checked;

	// Show current line
	XINXConfig::self()->config().editor.highlightCurrentLine = d->_ui->m_listEffect->item(1)->checkState() == Qt::Checked;

	// Auto highlight text
	XINXConfig::self()->config().editor.autoHighlight = d->_ui->m_listEffect->item(2)->checkState() == Qt::Checked;

	// Pretty print on saving
	if (d->_ui->m_listEffect->item(3)->checkState() == Qt::Checked)
		XINXConfig::self()->config().editor.autoindentOnSaving = "true";
	else
		XINXConfig::self()->config().editor.autoindentOnSaving = "false";
}

void NewVersionWizardImpl::on_m_listSlider_valueChanged(int value)
{
	for (int i = 0; i < d->_ui->m_listEffect->count(); i++)
	{
		d->_ui->m_listEffect->item(i)->setCheckState(i < value ? Qt::Checked : Qt::Unchecked);
	}
}

void NewVersionWizardImpl::slotCurrentIdChanged(int id)
{
	if (page(id) == d->_ui->licencePage)
	{
		d->_ui->_refuseRadioButton->setChecked(true);
		button(QWizard::NextButton)->setEnabled(false);
	}
}

void NewVersionWizardImpl::on__acceptRadioButton_toggled(bool value)
{
	button(QWizard::NextButton)->setEnabled(value);
}

void NewVersionWizardImpl::accept()
{
	XINXConfig::self()->save();

	if (d->_ui->m_customizeChk->isChecked())
	{
		CustomDialogImpl custom(this);
		custom.loadConfig();

		custom.exec();
	}

	QWizard::accept();
}
