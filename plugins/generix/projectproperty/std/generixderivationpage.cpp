/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
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
#include "generixderivationpage.h"
#include <core/xinxconfig.h>
#include <directoryedit.h>

/* DerivationDirectoryEditDelegate */

DerivationDirectoryEditDelegate::DerivationDirectoryEditDelegate(QObject *parent) : QItemDelegate(parent)
{
}

void DerivationDirectoryEditDelegate::setAbsolutePath(const QString & path)
{
	if (m_absolutePath != path)
	{

		m_absolutePath = path;
	}
}

QWidget * DerivationDirectoryEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() == 3)
	{
		DirectoryEditWidget * editor = new DirectoryEditWidget(parent);
		editor->setDefaultValue(m_absolutePath);
		editor->setDirectory(true);
		editor->layout()->setSpacing(0);
		return editor;
	}
	else
		return QItemDelegate::createEditor(parent, option, index);
}

void DerivationDirectoryEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	if (index.column() == 3)
	{
		QString value = QDir(m_absolutePath).absoluteFilePath(index.model()->data(index, Qt::EditRole).toString());
		DirectoryEdit * directoryEdit = qobject_cast<DirectoryEditWidget*>(editor)->lineEdit();
		directoryEdit->setText(QDir::toNativeSeparators(value));
		directoryEdit->setFocus();
	}
	else
		QItemDelegate::setEditorData(editor, index);
}

void DerivationDirectoryEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	if (index.column() == 3)
	{
		DirectoryEdit * directoryEdit = qobject_cast<DirectoryEditWidget*>(editor)->lineEdit();
		QString value = QDir(m_absolutePath).relativeFilePath(directoryEdit->text());

		model->setData(index, QDir::fromNativeSeparators(value), Qt::EditRole);
	}
	else
		QItemDelegate::setModelData(editor, model, index);
}

void DerivationDirectoryEditDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(index);
	editor->setGeometry(option.rect);
}

void DerivationDirectoryEditDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() == 3)
	{
		QString value = index.model()->data(index, Qt::DisplayRole).toString();

		painter->save();
		QStyleOptionViewItem o = option;
		if (! QFile(QDir(m_absolutePath).absoluteFilePath(value)).exists())
			o.palette.setColor(QPalette::Text, Qt::red);
		drawDisplay(painter, o, option.rect, value);
		painter->restore();
	}
	else
		QItemDelegate::paint(painter, option, index);
}


/* DerivationPathModel */

DerivationPathTableModel::DerivationPathTableModel(QObject * parent) : QAbstractTableModel(parent)
{
}

void DerivationPathTableModel::setDerivationPath(const QList<GenerixProject::DerivationPath> & values)
{
	m_paths = values;
	reset();
}

const QList<GenerixProject::DerivationPath> & DerivationPathTableModel::derivationPath() const
{
	return m_paths;
}

int DerivationPathTableModel::columnCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent);
	return 4;
}

int DerivationPathTableModel::rowCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent)
	return m_paths.size();
}

QVariant DerivationPathTableModel::data(const QModelIndex & index, int role) const
{
	if (index.isValid() && ((role == Qt::DisplayRole) || (role == Qt::EditRole)))
	{
		switch (index.column())
		{
		case 0:
			return m_paths.at(index.row()).name;
		case 1:
			return m_paths.at(index.row()).derivation;
		case 2:
			return m_paths.at(index.row()).visible;
		case 3:
			return m_paths.at(index.row()).path;
		}
	}

	return QVariant();
}

QVariant DerivationPathTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
	{
		switch (section)
		{
		case 0 :
			return tr("Name");
		case 1 :
			return tr("Derivable");
		case 2 :
			return tr("Visible");
		case 3 :
			return tr("Path");
		}
	}
	return QVariant();
}

bool DerivationPathTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (index.isValid() && (role == Qt::EditRole))
	{
		GenerixProject::DerivationPath dp = m_paths.at(index.row());
		switch (index.column())
		{
		case 0:
		{
			QString newName = value.toString();
			for (int row2 = 0; row2 < m_paths.size(); row2++)
			{
				const GenerixProject::DerivationPath & dp2 = m_paths.at(row2);
				if ((newName == dp2.name) && (row2!=index.row()))
					newName = newName + tr("Bis");
			}
			dp.name = newName;
		}
		break;
		case 1:
			dp.derivation = value.toBool();
			break;
		case 2:
			dp.visible = value.toBool();
			break;
		case 3:
			dp.path = value.toString();
			break;
		}
		m_paths.replace(index.row(), dp);
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

Qt::ItemFlags DerivationPathTableModel::flags(const QModelIndex & index) const
{
	if (index.isValid())
		return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
	return QAbstractTableModel::flags(index);
}

bool DerivationPathTableModel::insertRows(int row, int count, const QModelIndex & parent)
{
	if (parent.isValid()) return false;
	if (row > m_paths.count()) return false;
	if (row < 0) return false;

	beginInsertRows(QModelIndex(), row, row + count - 1);
	for (int i = 0; i < count; i++)
		m_paths.insert(row + i, GenerixProject::DerivationPath());
	endInsertRows();

	return true;
}

bool DerivationPathTableModel::removeRows(int row, int count, const QModelIndex & parent)
{
	if (parent.isValid()) return false;
	if (row < 0) return false;
	if ((row + count - 1) >= m_paths.count()) return false;

	beginRemoveRows(QModelIndex(), row, row + count - 1);
	for (int i = count - 1; i >= 0; i--)
		m_paths.removeAt(row - i);
	endRemoveRows();

	return true;
}


/* GenerixDerivationPathPageImpl */

GenerixDerivationPathPageImpl::GenerixDerivationPathPageImpl(QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f), m_project(0)
{
	setupUi(this);
	m_derivationPath->setModel(m_model = new DerivationPathTableModel(this));
	m_derivationPath->setItemDelegate(m_delegate = new DerivationDirectoryEditDelegate(this));

	connect(m_derivationPath->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(derivationLineChanged(QModelIndex)));
}

GenerixDerivationPathPageImpl::~GenerixDerivationPathPageImpl()
{
}

void GenerixDerivationPathPageImpl::setProject(XinxProject * project)
{
	if (m_project != project)
	{
		m_delegate->setAbsolutePath(project->projectPath());
		m_project = project;
	}
}

QPixmap GenerixDerivationPathPageImpl::image()
{
	return QPixmap();
}

QString GenerixDerivationPathPageImpl::name()
{
	return this->windowTitle();
}

bool GenerixDerivationPathPageImpl::loadSettingsDialog()
{
	GenerixProject * project = static_cast<GenerixProject*>(m_project);
	if (! project) return false;

	m_model->setDerivationPath(project->derivationsPath());

	return true;
}

bool GenerixDerivationPathPageImpl::saveSettingsDialog()
{
	GenerixProject * project = static_cast<GenerixProject*>(m_project);
	if (! project) return false;

	project->setDerivationsPath(m_model->derivationPath());

	return true;
}

void GenerixDerivationPathPageImpl::on_m_addBtn_clicked()
{
	m_model->insertRow(m_model->rowCount());
}

void GenerixDerivationPathPageImpl::on_m_delBtn_clicked()
{
	QModelIndex index = m_derivationPath->selectionModel()->currentIndex();
	Q_ASSERT(index.isValid());

	m_model->removeRow(index.row());
}

bool GenerixDerivationPathPageImpl::cancelSettingsDialog()
{
	return true;
}

QWidget * GenerixDerivationPathPageImpl::settingsDialog()
{
	return this;
}

bool GenerixDerivationPathPageImpl::isSettingsValid()
{
	return true;
}

bool GenerixDerivationPathPageImpl::isVisible()
{
	return true;
}

void GenerixDerivationPathPageImpl::derivationLineChanged(const QModelIndex & current)
{
	m_delBtn->setEnabled(current.isValid());
}

