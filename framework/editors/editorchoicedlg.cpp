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
#include "editorchoicedlg.h"

// Qt header
#include <QFileInfo>
#include <QVBoxLayout>

EditorChoiceDlg::EditorChoiceDlg(QWidget *parent) : QDialog(parent, Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint)
{
	QVBoxLayout * verticalLayout = new QVBoxLayout(this);

	_selector = new FileTypeSelector(this);
	_button_box = new QDialogButtonBox(this);
	_button_box->setOrientation(Qt::Horizontal);
	_button_box->setStandardButtons(QDialogButtonBox::Ok);

	verticalLayout->addWidget(_selector);
	verticalLayout->addWidget(_button_box);

	connect(_button_box, SIGNAL(accepted()), this, SLOT(accept()));
	connect(_button_box, SIGNAL(rejected()), this, SLOT(reject()));
	connect(_selector, SIGNAL(doubleClicked()), this, SLOT(accept()));
}

void EditorChoiceDlg::setFileName(const QString & value)
{
	_selector->setFileName(value);
}

IFileTypePluginPtr EditorChoiceDlg::selectedType() const
{
	return _selector->selectedType();
}

int EditorChoiceDlg::count() const
{
	return _selector->count();
}


