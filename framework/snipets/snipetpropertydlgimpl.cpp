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
#include "snipets/snipetmanager.h"
#include "snipets/snipetpropertydlgimpl.h"
#include <plugins/xinxpluginsloader.h>
#include <plugins/interfaces/files.h>
#include <editors/filetypepool.h>

// Qt header
#include <QRegExp>
#include <QSqlDatabase>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QItemDelegate>
#include <QPainter>

/* ParametersDelegate */

// FIXME: Make it a column delegate
class ParametersDelegate : public QItemDelegate
{
public:
	ParametersDelegate(QObject * parent = 0);

	virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

	void setCountParameter(int parameter);
	int countParameter() const;
private:
	int m_countParameter;
};

ParametersDelegate::ParametersDelegate(QObject * parent) : QItemDelegate(parent)
{
}

void ParametersDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	if (index.row() >= m_countParameter)
	{
		QStyleOptionViewItem myOption = option;
		myOption.palette.setColor(QPalette::Normal, QPalette::Window, QColor(0xff, 0xc0, 0xc0));

		if (option.showDecorationSelected && (option.state & QStyle::State_Selected))
		{
			painter->fillRect(myOption.rect, myOption.palette.brush(QPalette::Normal, QPalette::Highlight));
		}
		else
		{
			painter->fillRect(myOption.rect, myOption.palette.brush(QPalette::Normal, QPalette::Window));
		}

		QItemDelegate::paint(painter, myOption, index);
	}
	else
		QItemDelegate::paint(painter, option, index);
}

int ParametersDelegate::countParameter() const
{
	return m_countParameter;
}

void ParametersDelegate::setCountParameter(int parameter)
{
	if (m_countParameter != parameter)
	{
		m_countParameter = parameter;
	}
}

Q_DECLARE_METATYPE(QSqlRecord);

/* SnipetPropertyDlgImpl */

SnipetPropertyDlgImpl::SnipetPropertyDlgImpl(int snipetId, QSqlDatabase db, QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f), m_db(db), m_id(snipetId)
{
	setupUi();

	// Snipet
	m_snipetModel = new QSqlTableModel(this, db);
	m_snipetModel->setTable("snipets");
	m_snipetModel->setFilter(QString("id = %1").arg(snipetId));
	m_snipetModel->select();

	createMapper();

	// Parameters
	m_paramsModel = new QSqlTableModel(this, db);
	m_paramsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
	m_paramsModel->setTable("snipets_params");
	m_paramsModel->setFilter(QString("snipet_id = %1").arg(snipetId));
	m_paramsModel->setSort(snipet_params_id, Qt::AscendingOrder);
	m_paramsModel->select();
	m_paramsModel->setHeaderData(snipet_params_name, Qt::Horizontal, tr("Name"));
	m_paramsModel->setHeaderData(snipet_params_default_value, Qt::Horizontal, tr("Default Value"));
	connect(m_paramsModel, SIGNAL(beforeInsert(QSqlRecord&)), this, SLOT(m_paramsModel_beforeInsert(QSqlRecord&)));
	connect(m_paramsModel, SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(m_paramsModel_primeInsert(int,QSqlRecord&)));

	m_parameterTable->setModel(m_paramsModel);
	m_parameterTable->setColumnHidden(snipet_params_id, true);
	m_parameterTable->setColumnHidden(snipet_params_snipet_id, true);
	m_parameterTable->resizeColumnToContents(snipet_params_name);

	m_delegate = new ParametersDelegate(m_parameterTable);
	m_parameterTable->setItemDelegate(m_delegate);

	// Go to snipet
	m_mapper->toFirst();
}

SnipetPropertyDlgImpl::SnipetPropertyDlgImpl(QSqlDatabase db, QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f), m_db(db), m_id(-1)
{
	setupUi();

	// Snipet
	m_snipetModel = new QSqlTableModel(this, db);
	m_snipetModel->setTable("snipets");
	m_snipetModel->setFilter("id is null");
	m_snipetModel->select();

	int field = m_snipetModel->fieldIndex("category_id");
	int row = m_snipetModel->rowCount();
	m_snipetModel->insertRow(row);
	QModelIndex index = m_snipetModel->index(row, field);
	m_snipetModel->setData(index, 0);

	createMapper();

	// Parameters
	m_paramsModel = new QSqlTableModel(this, db);
	m_paramsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
	m_paramsModel->setTable("snipets_params");
	m_paramsModel->setFilter("snipet_id is null");
	m_paramsModel->setSort(snipet_params_id, Qt::AscendingOrder);
	m_paramsModel->select();
	m_paramsModel->setHeaderData(snipet_params_name, Qt::Horizontal, tr("Name"));
	m_paramsModel->setHeaderData(snipet_params_default_value, Qt::Horizontal, tr("Default Value"));
	connect(m_paramsModel, SIGNAL(beforeInsert(QSqlRecord&)), this, SLOT(m_paramsModel_beforeInsert(QSqlRecord&)));
	connect(m_paramsModel, SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(m_paramsModel_primeInsert(int,QSqlRecord&)));

	m_parameterTable->setModel(m_paramsModel);
	m_parameterTable->setColumnHidden(snipet_params_id, true);
	m_parameterTable->setColumnHidden(snipet_params_snipet_id, true);
	m_parameterTable->resizeColumnToContents(snipet_params_name);

	m_delegate = new ParametersDelegate(m_parameterTable);
	m_parameterTable->setItemDelegate(m_delegate);

	// Go to snipet
	m_mapper->setCurrentIndex(row);
}

SnipetPropertyDlgImpl::~SnipetPropertyDlgImpl()
{
}

void SnipetPropertyDlgImpl::setupUi()
{
	Ui::SnipetPropertyDialog::setupUi(this);

	// List of icon
	QDir iconsDir = QDir(":/images");

	m_iconComboBox->addItem(QString());
	foreach(const QString & fileName, iconsDir.entryList(QDir::Files))
	{
		const QString completeFileName = iconsDir.absoluteFilePath(fileName);
		m_iconComboBox->addItem(QIcon(completeFileName), completeFileName);
	}

	// Extentions
	foreach(IFileTypePluginPtr fileType, XinxPluginsLoader::self()->fileTypePool()->fileTypes())
	{
		QListWidgetItem * item = new QListWidgetItem(XinxPluginsLoader::self()->fileTypePool()->fileTypeFilter(fileType), m_extentionListWidget);
		item->setData(Qt::UserRole, fileType->match());
	}

	// Snipet shortcut
	m_keyLineEdit->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9\\:\\-]*"), this));

	// List of category
	m_categoryModel = SnipetManager::self()->createCategoryItemModel(m_categoryTreeView);
	m_categoryModel->select();
	m_categoryTreeView->setModel(m_categoryModel);
	m_categoryTreeView->expandAll();

	// Text Edit
	connect(m_textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(m_textEdit_textChanged()));
}

void SnipetPropertyDlgImpl::createMapper()
{
	m_mapper = new QDataWidgetMapper(this);
	connect(m_mapper, SIGNAL(currentIndexChanged(int)), this, SLOT(m_mapper_currentIndexChanged(int)));
	m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
	m_mapper->setModel(m_snipetModel);
	m_mapper->addMapping(m_nameLineEdit, m_snipetModel->fieldIndex("name"));
	m_mapper->addMapping(m_iconComboBox, m_snipetModel->fieldIndex("icon"), "value");
	m_mapper->addMapping(m_keyLineEdit, m_snipetModel->fieldIndex("shortcut"));
	m_mapper->addMapping(m_autoChk, m_snipetModel->fieldIndex("auto"));
	m_mapper->addMapping(m_dialogChk, m_snipetModel->fieldIndex("show_dialog"));
	m_mapper->addMapping(m_descriptionTextEdit, m_snipetModel->fieldIndex("description"));
	m_mapper->addMapping(m_categoryTreeView, m_snipetModel->fieldIndex("category_id"));
	m_mapper->addMapping(m_textEdit, m_snipetModel->fieldIndex("text"), "plainText");
	m_mapper->addMapping(m_availablePlainTextEdit, m_snipetModel->fieldIndex("available_script"));
}

void SnipetPropertyDlgImpl::duplicate(int id)
{
	QSqlTableModel duplicateModel(this, m_db);
	duplicateModel.setTable("snipets");
	duplicateModel.setFilter(QString("id = %1").arg(id));
	duplicateModel.select();

	QSqlRecord duplicateRecord = duplicateModel.record(0);
	duplicateRecord.setValue(m_snipetModel->fieldIndex("id"), 0);

	m_snipetModel->setRecord(0, duplicateRecord);
	m_mapper->toFirst();

	initialiseExtentions(id);

	QSqlTableModel duplicateParamsModel(this, m_db);
	duplicateParamsModel.setTable("snipets_params");
	duplicateParamsModel.setFilter(QString("snipet_id = %1").arg(id));
	duplicateParamsModel.setSort(snipet_params_id, Qt::AscendingOrder);
	duplicateParamsModel.select();

	for (int i = 0 ; i < duplicateParamsModel.rowCount() ; i++)
	{
		QSqlRecord duplicateParamsRecord = duplicateParamsModel.record(i);
		duplicateParamsRecord.setValue(snipet_params_id, 0);
		m_paramsModel->setRecord(i, duplicateParamsRecord);
	}
}

void SnipetPropertyDlgImpl::setParentId(int id)
{
	m_categoryTreeView->setCategoryId(id);
}

void SnipetPropertyDlgImpl::m_paramsModel_beforeInsert(QSqlRecord & record)
{
	// The id is normally created
	record.setValue(snipet_params_snipet_id, m_id);
}

void SnipetPropertyDlgImpl::m_paramsModel_primeInsert(int row, QSqlRecord & record)
{
	record.setValue(snipet_params_name, tr("Parameter %1").arg(row + 1));
}

void SnipetPropertyDlgImpl::m_textEdit_textChanged()
{
	QString text = m_textEdit->toPlainText();
	int countParameter = 0, pos = 0;
	QRegExp regExpParameter("%(\\d+)");
	while ((pos = regExpParameter.indexIn(text, pos)) != -1)
	{
		int parameterNum = regExpParameter.cap(1).toInt();
		if (parameterNum > countParameter)
			countParameter = parameterNum;
		pos += regExpParameter.matchedLength();
	}

	int rc = m_paramsModel->rowCount();
	if (countParameter > rc)
	{
		for (int i = rc ; i < countParameter; i++)
		{
			m_paramsModel->insertRow(i);
		}
	}
	m_delegate->setCountParameter(countParameter);
}

void SnipetPropertyDlgImpl::initialiseExtentions(int id)
{
	m_extentionListWidget->selectionModel()->clearSelection();
	if (id == -1) return ;  // In creation, no row to read

	QSqlQuery extentionsQuery(m_db);
	extentionsQuery.prepare("SELECT distinct extention FROM snipets_extentions WHERE snipet_id=:id");
	extentionsQuery.bindValue(":id", id);

	bool result = extentionsQuery.exec();
	Q_ASSERT(result);

	while (extentionsQuery.next())
	{
		QString extention = extentionsQuery.value(0).toString();

		for (int i = 0 ; i < m_extentionListWidget->count() ; i++)
		{
			QListWidgetItem * item = m_extentionListWidget->item(i);
			if (item->data(Qt::UserRole).toString().contains(extention))
			{
				item->setSelected(true);
				break;
			}
		}
	}
}

void SnipetPropertyDlgImpl::m_mapper_currentIndexChanged(int index)
{
	Q_UNUSED(index);

	initialiseExtentions(m_id);
}

void SnipetPropertyDlgImpl::on_m_categoryTreeView_activated(const QModelIndex & index)
{
	int id = index.data(CategoryItemModel::CategoryIdRole).toInt();

	m_addCategoryButton->setEnabled(index != QModelIndex());
	m_removeCategoryButton->setEnabled((id != 0) && (index != QModelIndex()));
}

void SnipetPropertyDlgImpl::on_m_addCategoryButton_clicked()
{
	QModelIndexList list = m_categoryTreeView->selectionModel()->selectedIndexes();
	Q_ASSERT(list.size());

	int id = list.at(0).data(CategoryItemModel::CategoryIdRole).toInt();

	SnipetManager::self()->addCategory(id, false, this);

	m_categoryModel->select();
	m_categoryTreeView->expandAll();
}

void SnipetPropertyDlgImpl::on_m_removeCategoryButton_clicked()
{
	QModelIndexList list = m_categoryTreeView->selectionModel()->selectedIndexes();
	Q_ASSERT(list.size());

	int id = list.at(0).data(CategoryItemModel::CategoryIdRole).toInt();

	SnipetManager::self()->removeCategory(id, this);

	m_categoryModel->select();
	m_categoryTreeView->expandAll();
}

void SnipetPropertyDlgImpl::on_m_buttons_rejected()
{
	m_mapper->revert();
	m_paramsModel->revertAll();
}

void SnipetPropertyDlgImpl::on_m_buttons_accepted()
{
	// To create the Id if necessary
	m_mapper->submit();

	if (m_id == -1)
		m_id = m_snipetModel->query().lastInsertId().toInt();

	// Extentions
	for (int i = 0 ; i < m_extentionListWidget->count() ; i++)
	{
		const QListWidgetItem * item = m_extentionListWidget->item(i);
		const bool isSelected = item->isSelected();
		const QStringList extentions = item->data(Qt::UserRole).toString().split(" ");

		foreach(QString extention, extentions)
		{
			// A line with the extention exist ?
			QSqlQuery countQuery("SELECT count(1) FROM snipets_extentions WHERE snipet_id=:id AND extention=:ext", m_db);
			countQuery.bindValue(":id", m_id);
			countQuery.bindValue(":ext", extention);

			bool result = countQuery.exec();
			Q_ASSERT(result);

			result = countQuery.next();
			Q_ASSERT(result);

			int count = countQuery.value(0).toInt();
			if (isSelected)
			{
				if (count == 0)
				{
					QSqlQuery insertQuery("INSERT INTO snipets_extentions( snipet_id, extention ) VALUES (:id, :ext)", m_db);
					insertQuery.bindValue(":id", m_id);
					insertQuery.bindValue(":ext", extention);

					result = insertQuery.exec();
					Q_ASSERT(result);
				}
			}
			else
			{
				if (count > 0)
				{
					QSqlQuery deleteQuery("DELETE FROM snipets_extentions WHERE snipet_id=:id AND extention=:ext", m_db);
					deleteQuery.bindValue(":id", m_id);
					deleteQuery.bindValue(":ext", extention);

					result = deleteQuery.exec();
					Q_ASSERT(result);
				}
			}
		}
	}

	// Parameters
	int countParameter = m_delegate->countParameter();
	int rw = m_paramsModel->rowCount();
	if (countParameter < rw)
		m_paramsModel->removeRows(countParameter, rw - countParameter);

	m_paramsModel->submitAll();
}

