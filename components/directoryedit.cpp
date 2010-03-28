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
#include "directoryedit.h"

// Qt header
#include <QFile>
#include <QPalette>
#include <QCompleter>
#include <QFileDialog>
#include <QToolButton>
#include <QHBoxLayout>
#include <QToolButton>

/* CompleterDirModel */

/*!
 * \ingroup Components
 * \class CompleterDirModel
 * \brief Class used by DirectoryEdit to complete on path.
 * Completer model class based on a QDirModel which permit to propose a completion on
 * the path.
 * The QDir model convert the slash on native separtor. If the path end with a separator,
 * the separator is deleted.
 */

/*!
 * Construct a CompleterDirModel.
 * \param parent The parent of the object.
 */
CompleterDirModel::CompleterDirModel(QObject *parent) : QDirModel(parent)
{
}

/*!
 * Return the data to change in the completion dir model. This model show the good
 * separator (with the \e QDir::toNativeSepartors() function).
 * \param index The model index, represents a path in the tree.
 * \param role Only the role Qt::DisplayRole is modified in this object.
 */
QVariant CompleterDirModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole && index.column() == 0)
	{
		QString path = QDir::toNativeSeparators(filePath(index));
		if (path.endsWith(QDir::separator()))
			path.chop(1);
		return path;
	}

	return QDirModel::data(index, role);
}

/* DirectoryEdit */

/*!
 * \ingroup Components
 * \class DirectoryEdit
 * \brief A line edit to edit, file or path.
 * A line editor using the QCompleter \e CompleterDirModel to simplify the editing of
 * the path.
 * If the path is wrong, he's writing in red.
 */

/*!
 * Construct a DirectoryEdit object.
 * \param parent The parent widget of the object.
 */
DirectoryEdit::DirectoryEdit(QWidget * parent) : QLineEdit(parent), m_fileMustExist(true)
{
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));
	QCompleter * completer = new QCompleter(this);
	this->setCompleter(completer);
	completer->setModel(new CompleterDirModel(completer));
}

/*!
 * Construct a DirectoryEdit object and change the content of the text at the creation.
 * \param contents Content of the Line edit.
 * \param parent The parent widget of the object.
 */
DirectoryEdit::DirectoryEdit(const QString & contents, QWidget * parent) : QLineEdit(contents, parent)
{
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));
}

/*!
 * Set to false, if the file can't not exist and you don't want to warn user
 * \param value true if you want the user
 * \sa warnIfExist
 */
void DirectoryEdit::setFileMustExist(bool value)
{
	if (value != m_fileMustExist)
	{
		m_fileMustExist = value;
		slotTextChanged(this->text());
	}
}

/*!
 * Return true if you want to warn the user when the file doesn't exist.
 * \sa setWarnIfExist
 */
bool DirectoryEdit::fileMustExist() const
{
	return m_fileMustExist;
}

/*!
 * Set the filter to use to change load a file
 * \param filter The new filter to use
 * \sa filter
 */
void DirectoryEdit::setFilter(const QString & filter)
{
	m_filter = filter;
}

/*!
 * Get the filter used to change the file
 * \sa setFilter
 */
const QString & DirectoryEdit::filter() const
{
	return m_filter;
}

/*!
 * Slot called when the text changed. This method change the color of the text if the
 * directory \e text not exist.
 */
void DirectoryEdit::slotTextChanged(QString text)
{
	QFile file(text);
	QPalette palette(this->palette());

	if ((!m_fileMustExist) || file.exists())
	{
		palette.setColor(QPalette::Text, QColor());
	}
	else
	{
		palette.setColor(QPalette::Text, Qt::red);
	}
	setPalette(palette);
}

/*!
 * Open a dialog to change the path in the line editor.
 * \param parent The parent windows of the dialog to open
 * \param defaultValue The value to propose, if there is no value in the editor.
 * \param directory If \e true, user must choose a directory, else a file
 */
void DirectoryEdit::changePath(QWidget * parent, const QString & defaultValue, bool directory)
{
	QString value = this->text();
	if (value.isEmpty()) value = defaultValue;

	if (directory)
		value = QFileDialog::getExistingDirectory(parent, tr("Change the path"), value);
	else if (m_fileMustExist)
		value = QFileDialog::getOpenFileName(parent, tr("Change the file"), value, m_filter, &m_filter);
	else
		value = QFileDialog::getSaveFileName(parent, tr("Change the file"), value, m_filter, &m_filter);

	if (! value.isEmpty())
	{
		this->setText(QDir::toNativeSeparators(value));
	}
}

/* PrivateDirectoryEditWidget */

class PrivateDirectoryEditWidget
{
public:
	PrivateDirectoryEditWidget(QWidget * o);

	DirectoryEdit * m_lineEdit;
	QToolButton * m_button;
	bool m_directory;
	QString m_default;
private:
};

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
 * \brief A widget to edit a path. This widget have button attached to open a dialog.
 * This class is an association of a Directory Line editor and a button.
 * The button call the \e changePath slot from the \e DirectoryEdit class.
 */

/*!
 * Construct the directory edit widget and tell if the widget is used
 * to edit directory or file.
 * \param isDirectory If \e true, the widget edit directory, otherwise file.
 * \param parent The parent of the widget.
 */
DirectoryEditWidget::DirectoryEditWidget(bool isDirectory, QWidget * parent) : QWidget(parent)
{
	d = new PrivateDirectoryEditWidget(this);
	d->m_directory = isDirectory;
	connect(d->m_button, SIGNAL(clicked()), this, SLOT(changePath()));
}

/*!
 * Construct the directory edit widget.
 * \param parent The parent of the object.
 */
DirectoryEditWidget::DirectoryEditWidget(QWidget * parent) : QWidget(parent)
{
	d = new PrivateDirectoryEditWidget(this);
	d->m_directory = true;
	connect(d->m_button, SIGNAL(clicked()), this, SLOT(changePath()));
}

/*!
 * Destroy the widget
 */
DirectoryEditWidget::~DirectoryEditWidget()
{
	delete d;
}

/*!
 * Return true if the widget is used to edit directory, or false if the widget is used
 * to edit file.
 */
bool DirectoryEditWidget::isDirectory() const
{
	return d->m_directory;
}

/*!
 * Change the directory/files goal of the widget.
 * \param value If \e true, the widget is used for edit directory, otherwise files.
 */
void DirectoryEditWidget::setDirectory(bool value)
{
	d->m_directory = value;
}

/*!
 * Return the default value used by the widget when the user click on the open button
 */
QString DirectoryEditWidget::defaultValue() const
{
	return d->m_default;
}

/*!
 * Used to change the default value to use, when no text is in the editor.
 * \param value The new default value to use when the user click on the button.
 */
void DirectoryEditWidget::setDefaultValue(const QString & value)
{
	d->m_default = value;
}

/*!
 * This slot is called to open a dialog to edit the new path.
 */
void DirectoryEditWidget::changePath()
{
	d->m_lineEdit->changePath(parentWidget(), d->m_default, d->m_directory);
}

void DirectoryEditWidget::focusInEvent(QFocusEvent * event)
{
	Q_UNUSED(event);

	d->m_lineEdit->setFocus();
}

/*!
 * Return the DirectoryEdit created by this widget.
 */
DirectoryEdit * DirectoryEditWidget::lineEdit() const
{
	return d->m_lineEdit;
}
