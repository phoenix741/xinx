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

#include "xinxdirectorylistwidget_p.h"

#include <QApplication>
#include <QFileDialog>

/*!
* \ingroup Components
* \class XinxDirectoryListWidget
 * \brief This list ask to the user to choose the directory to add to the list.
 * \since 0.10.0.0
 *
 * \bc 0.10.0.0
 *
 */


/*!
 * \brief Create a XinxDirectoryListWidget based on the XinxListWidgetImpl
 */
XinxDirectoryListWidget::XinxDirectoryListWidget(QWidget* parent, Qt::WindowFlags f): XinxListWidgetImpl(parent, f), d(new XinxDirectoryListWidgetPrivate)
{

}

//! Destroy the XinxDirectoryListWidget
XinxDirectoryListWidget::~XinxDirectoryListWidget()
{

}

/*!
 * \brief This method is called when a new value is added into the list.
 * \since 0.10.0.0
 *
 * When a new value is added to the list, this method open a dialog box to the user to ask for an existing directory.
 * The entered value from the user is returned to be added to the list.
 */
QString XinxDirectoryListWidget::addNewValue()
{
	return QDir::toNativeSeparators(QFileDialog::getExistingDirectory(qApp->activeWindow(), tr("Get a directory"), defaultProposedValue()));
}

