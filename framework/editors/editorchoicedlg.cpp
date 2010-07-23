/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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
#include "editorchoicedlg.h"

// Qt header
#include <QFileInfo>

EditorChoiceDlg::EditorChoiceDlg(QWidget *parent) :
    QDialog(parent){
    setupUi(this);
}

void EditorChoiceDlg::setFileName(const QString & value)
{
	m_fileName->setText(QFileInfo(value).fileName());
}

void EditorChoiceDlg::setFileTypes(const QList<IFileTypePlugin *> & types)
{
	m_typeFileList->clear();
	foreach(IFileTypePlugin* plugin, types)
	{
		QListWidgetItem * item = new QListWidgetItem(QIcon(plugin->icon()), plugin->description(), m_typeFileList);
		item->setData(Qt::UserRole, QVariant::fromValue<IFileTypePlugin*>(plugin));
	}
	m_typeFileList->setCurrentRow(0);
}

IFileTypePlugin * EditorChoiceDlg::selectedType() const
{
	QListWidgetItem * item = m_typeFileList->currentItem();
	if (item)
	{
		return item->data(Qt::UserRole).value<IFileTypePlugin*>();
	}
	return 0;
}

void EditorChoiceDlg::on_m_typeFileList_itemDoubleClicked(QListWidgetItem* item)
{
	Q_UNUSED(item);

	accept();
}
