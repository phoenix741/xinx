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
#include "directoryeditwidget_p.h"
#include <directoryedit.h>

// Qt header
#include <QFile>
#include <QPalette>
#include <QCompleter>
#include <QFileDialog>
#include <QToolButton>
#include <QHBoxLayout>
#include <QToolButton>
#include <QKeyEvent>

/* PrivateDirectoryEditWidget */

PrivateDirectoryEditWidget::PrivateDirectoryEditWidget(QWidget * o)
{
	QHBoxLayout * layout = new QHBoxLayout(o);

	m_lineEdit = new DirectoryEdit(o);
	m_button   = new QToolButton(o);
	m_button->setIcon(QIcon(":/images/folder.png"));

	layout->addWidget(m_lineEdit);
	layout->addWidget(m_button);

	layout->setMargin(0);
}

/* DirectoryEditWidget */

/*!
 * \ingroup Components
 * \class DirectoryEditWidget
 * \since 0.7.0.0
 *
 * \bc 0.10.0.0
 *
 * \brief A widget to edit a path. This widget has a button attached to open a dialog.
 *
 * This class is an association of a Directory Line editor and a button.
 * The button call the \p changePath slot from the \p DirectoryEdit class.
 *
 * \image html directoryeditwidget1.png
 * \image latex directoryeditwidget1.png
 */

/*!
 * \brief Constructs the directory edit widget and tells if the widget is used
 * to edit directory or file.
 * \param isDirectory If \p true, the widget edit directory, otherwise file.
 * \param parent The parent of the widget.
 */
DirectoryEditWidget::DirectoryEditWidget(bool isDirectory, QWidget * parent) : QWidget(parent)
{
	d = new PrivateDirectoryEditWidget(this);
	d->m_lineEdit->setDirectory(isDirectory);
	connect(d->m_button, SIGNAL(clicked()), this, SLOT(changePath()));
}

/*!
 * \brief Construct the directory edit widget.
 * \param parent The parent of the object.
 */
DirectoryEditWidget::DirectoryEditWidget(QWidget * parent) : QWidget(parent)
{
	d = new PrivateDirectoryEditWidget(this);
	connect(d->m_button, SIGNAL(clicked()), this, SLOT(changePath()));
}

//! Destroy the widget
DirectoryEditWidget::~DirectoryEditWidget()
{
	delete d;
}

/*!
 * \brief This property holds whether the input is a directory or a file
 *
 * Return true if the widget is used to edit directory, or false if the widget is used
 * to edit file.
 * \sa setDirectory()
 */
bool DirectoryEditWidget::isDirectory() const
{
	return d->m_lineEdit->isDirectory();
}

/*!
 * \brief Change the directory/files goal of the widget.
 * \param value If \p true, the widget is used for edit directory, otherwise files.
 * \sa isDirectory()
 */
void DirectoryEditWidget::setDirectory(bool value)
{
	d->m_lineEdit->setDirectory(value);
}

/*!
 * \brief Return the default value used by the widget when the user click on the open button
 * \sa setDefaultValue()
 */
QString DirectoryEditWidget::defaultValue() const
{
	return d->m_default;
}

/*!
 * \brief Used to change the default value to use, when no text is in the editor.
 * \param value The new default value to use when the user click on the button.
 * \sa defaultValue()
 */
void DirectoryEditWidget::setDefaultValue(const QString & value)
{
	d->m_default = value;
}

/*!
 * \brief This slot is called to open a dialog to edit the new path.
 */
void DirectoryEditWidget::changePath()
{
	d->m_lineEdit->changePath(parentWidget(), d->m_default);
}

void DirectoryEditWidget::focusInEvent(QFocusEvent * event)
{
	Q_UNUSED(event);

	d->m_lineEdit->setFocus();
}

/*!
 * \brief Return the DirectoryEdit created by this widget.
 */
DirectoryEdit * DirectoryEditWidget::lineEdit() const
{
	return d->m_lineEdit;
}
