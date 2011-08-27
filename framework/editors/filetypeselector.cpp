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

#include "filetypeselector.h"
#include <QVBoxLayout>
#include <QFileInfo>
#include <plugins/xinxpluginsloader.h>
#include <editors/filetypepool.h>

FileTypeSelector::FileTypeSelector(QWidget *parent) : QWidget(parent)
{
	QVBoxLayout * verticalLayout = new QVBoxLayout(this);
	verticalLayout->setMargin(0);

	QLabel * label = new QLabel(this);
	label->setWordWrap(true);
	label->setText(tr("<b>XINX</b> can't determine the editor to use to open the file. Wich type of file do you want open ?"));

	_file_name_label = new QLabel(this);
	_file_type_list = new QListWidget(this);
	_show_all = new QCheckBox(tr("Show &all file type"), this);

	verticalLayout->addWidget(label);
	verticalLayout->addWidget(_file_name_label);
	verticalLayout->addWidget(_file_type_list);
	verticalLayout->addWidget(_show_all);

	connect(_file_type_list, SIGNAL(doubleClicked(QModelIndex)), this, SIGNAL(doubleClicked()));
	connect(_show_all, SIGNAL(toggled(bool)), this, SLOT(toggledShowAll()));
}

FileTypeSelector::~FileTypeSelector()
{

}

void FileTypeSelector::setShowFileName(bool value)
{
	_file_name_label->setVisible(value);
}

bool FileTypeSelector::isShowFileName() const
{
	return _file_name_label->isVisible();
}

void FileTypeSelector::setFileName(const QString & filename)
{
	_file_name_label->setText(QFileInfo(filename).fileName());
	QList<IFileTypePluginPtr> interfaces = XinxPluginsLoader::self()->fileTypePool()->matchedFileType(filename);

	_show_all->setVisible(interfaces.size());
	if (! interfaces.size() || _show_all->isChecked())
	{
		setFileTypes(XinxPluginsLoader::self()->fileTypePool()->fileTypes());
	}
	else
	{
		setFileTypes(interfaces);
	}
	_file_name = filename;
}

const QString & FileTypeSelector::filename() const
{
	return _file_name;
}

int FileTypeSelector::count() const
{
	return _file_type_list->count();
}

void FileTypeSelector::setFileTypes(const QList<IFileTypePluginPtr> & types)
{
	_file_type_list->clear();
	foreach(IFileTypePluginPtr plugin, types)
	{
		// FIXME: Le fait de ne pas utiliser QSharedPointer peut poser problème, mais ne devrait pas :D pour autant.
		QListWidgetItem * item = new QListWidgetItem(QIcon(plugin->icon()), plugin->description(), _file_type_list);
		item->setData(Qt::UserRole, QVariant::fromValue<IFileTypePluginPtr>(plugin));
	}
	_file_type_list->setCurrentRow(0);
}

IFileTypePluginPtr FileTypeSelector::selectedType() const
{
	QListWidgetItem * item = _file_type_list->currentItem();
	if (item)
	{
		return item->data(Qt::UserRole).value<IFileTypePluginPtr>();
	}
	return IFileTypePluginPtr();
}

void FileTypeSelector::toggledShowAll()
{
	setFileName(_file_name);
}
