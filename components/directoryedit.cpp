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
#include "directoryedit_p.h"
#include "completerdirmodel.h"

// Qt header
#include <QFile>
#include <QPalette>
#include <QCompleter>
#include <QFileDialog>
#include <QToolButton>
#include <QHBoxLayout>
#include <QToolButton>
#include <QKeyEvent>

/* DirectoryEditPrivate */

DirectoryEditPrivate::DirectoryEditPrivate(DirectoryEdit* parent) : _parent(parent), m_error(false), m_directory(true), m_fileMustExist(true), m_ignoreCursorPositionChanged(false)
{
}

/*!
* \internal
*/
void DirectoryEditPrivate::slotEditorCursorPositionChanged(int oldpos, int newpos)
{
	if (!_parent->hasSelectedText() && !m_ignoreCursorPositionChanged)
	{
		m_ignoreCursorPositionChanged = true;

		bool allowSelection = true;
		int pos = -1;
		if (newpos < m_prefix.size() && newpos != 0)
		{
			if (oldpos == 0)
			{
				allowSelection = false;
				pos = m_prefix.size();
			}
			else
			{
				pos = oldpos;
			}
		}

		if (pos != -1)
		{
			const int selSize = _parent->selectionStart() >= 0 && allowSelection ? (_parent->selectedText().size() * (newpos < pos ? -1 : 1)) - newpos + pos : 0;

			const bool wasBlocked = blockSignals(true);
			if (selSize != 0)
			{
				_parent->setSelection(pos - selSize, selSize);
			}
			else
			{
				_parent->setCursorPosition(pos);
			}
			blockSignals(wasBlocked);
		}
		m_ignoreCursorPositionChanged = false;
	}
}


/*!
* \brief Slot called when the text changed.
*
* This method change the color of the text if the directory \p text not exist.
*/
void DirectoryEditPrivate::slotTextChanged(QString text)
{
	QFileInfo fileInfo(text);
	QFile file(text);
	QPalette palette(_parent->palette());

	m_error = (m_directory != fileInfo.isDir()) || (m_fileMustExist && !file.exists());

	if (!m_error)
	{
		palette.setColor(QPalette::Text, QColor());
	}
	else
	{
		palette.setColor(QPalette::Text, Qt::red);
	}
	_parent->setPalette(palette);
}


/* DirectoryEdit */

/*!
 * \ingroup Components
 * \class DirectoryEdit
 * \since 0.7.0.0
 *
 * \brief A line edit to edit, file or path.
 *
 * A line editor using a QCompleter based on the model \p CompleterDirModel to
 * help the user to edit the path.
 *
 * If the path is wrong, he's writing in red.
 *
 * You can see the widget on completion :
 * \image html directoryedit1.png
 * \image latex directoryedit1.png
 *
 * And the same widget, if the path doesn't exist :
 * \image html directoryedit2.png
 * \image latex directoryedit2.png
 *
 *
 */

/*!
 * \brief Construct a DirectoryEdit object.
 * \param parent The parent widget of the object.
 */
DirectoryEdit::DirectoryEdit(QWidget * parent) : QLineEdit(parent), d(new DirectoryEditPrivate(this))
{
	connect(this, SIGNAL(textChanged(QString)), d.data(), SLOT(slotTextChanged(QString)));
	connect(this, SIGNAL(cursorPositionChanged(int,int)), d.data(), SLOT(slotEditorCursorPositionChanged(int,int)));
	QCompleter * completer = new QCompleter(this);
	this->setCompleter(completer);
	completer->setModel(new CompleterDirModel(completer));
}

/*!
 * \brief Construct a DirectoryEdit object and change the content of the text at the creation.
 * \param contents Content of the Line edit.
 * \param parent The parent widget of the object.
 */
DirectoryEdit::DirectoryEdit(const QString & contents, QWidget * parent) : QLineEdit(contents, parent), d(new DirectoryEditPrivate(this))
{
	connect(this, SIGNAL(textChanged(QString)), d.data(), SLOT(slotTextChanged(QString)));
	connect(this, SIGNAL(cursorPositionChanged(int,int)), d.data(), SLOT(slotEditorCursorPositionChanged(int,int)));
}

/*!
 * \brief Destroy the DirectoryEdit
 */
DirectoryEdit::~DirectoryEdit()
{

}

/*!
 * \brief Set to false, if the file can't not exist and you don't want to warn user
 * \param value true if you want the user
 * \sa fileMustExist
 */
void DirectoryEdit::setFileMustExist(bool value)
{
	if (value != d->m_fileMustExist)
	{
		d->m_fileMustExist = value;
		d->slotTextChanged(this->text());
	}
}

/*!
 * \brief Return true if you want to warn the user when the file doesn't exist.
 * \sa setFileMustExist
 */
bool DirectoryEdit::fileMustExist() const
{
	return d->m_fileMustExist;
}

/*!
 * \brief Set the filter to use to change load a file
 * \param filter The new filter to use
 * \sa filter
 */
void DirectoryEdit::setFilter(const QString & filter)
{
	d->m_filter = filter;
}

/*!
 * \brief Get the filter used to change the file
 * \sa setFilter
 */
const QString & DirectoryEdit::filter() const
{
	return d->m_filter;
}

/*!
 * \brief Set the limit from where the user can start editing
 * \param value The position of the first char that the user car edit.
 *
 * If the value is different of 0, the user can't edit first \e value characters.
 *
 * \sa prefix();
 */
void DirectoryEdit::setPrefix(const QString & value)
{
	if (d->m_prefix != value)
	{
		d->m_prefix = value;
		setText(text());
	}
}

/*!
 * \brief Get the value of the edit limit
 * \sa setPrefix()
 */
const QString & DirectoryEdit::prefix() const
{
	return d->m_prefix;
}

/*!
 * \brief This property indicate if the widget is used to edit directory or file
 * \sa setDirectory()
 */
bool DirectoryEdit::isDirectory() const
{
	return d->m_directory;
}

/*!
 * \brief Set to false to edit file.
 *
 * This method is used to tell if we edit directory or file. If we edit file, and user
 * edit a directory, the editor show the field in red.
 *
 * \sa isDirectory()
 */
void DirectoryEdit::setDirectory(bool value)
{
	if (d->m_directory != value)
	{
		d->m_directory = value;
		d->slotTextChanged(text());
	}
}

/*!
 * \brief Return true if the line edit is in error
 *
 * The Line edit can be in an error if the waiting text is a file and we want a directory (or inverse).
 * Or if the file must exist and this file does'nt exist.
 */
bool DirectoryEdit::isError() const
{
	return d->m_error;
}

/*!
 * \reimp
 */
void DirectoryEdit::setText(const QString & value)
{
	if (value.startsWith(d->m_prefix))
	{
		QLineEdit::setText(value);
	}
	else
	{
		QLineEdit::setText(d->m_prefix + value);
	}
}

/*!
 * \reimp
 */
void DirectoryEdit::selectAll()
{
	setSelection(d->m_prefix.size(), text().size() - d->m_prefix.size());
}

/*!
 * \reimp
 */
void DirectoryEdit::clear()
{
	setText(d->m_prefix);
	setCursorPosition(d->m_prefix.size());
}

/*!
 * \reimp
 */
void DirectoryEdit::keyPressEvent(QKeyEvent *event)
{
	if (!event->text().isEmpty() && cursorPosition() < d->m_prefix.size())
		setCursorPosition(d->m_prefix.size());

	switch (event->key())
	{
	case Qt::Key_End:
	case Qt::Key_Home:
		if (event->modifiers() & Qt::ShiftModifier)
		{
			int currentPos = cursorPosition();
			const QString text = displayText();
			if (event->key() == Qt::Key_End)
			{
				if (currentPos == 0 && !d->m_prefix.isEmpty())
				{
					break; // let lineedit handle this
				}
				else
				{
					setSelection(currentPos, text.size() - currentPos);
				}
			}
			else
			{
				if (currentPos <= d->m_prefix.size())
				{
					break; // let lineedit handle this
				}
				else
				{
					setSelection(currentPos, d->m_prefix.size() - currentPos);
				}
			}
			event->accept();
			return;
		}
		break;
	case Qt::Key_Backspace:
		if (cursorPosition() == d->m_prefix.size())
		{
			event->ignore();
			return;
		}
		break;
#ifdef Q_WS_X11 // only X11
	case Qt::Key_U:
		if (event->modifiers() & Qt::ControlModifier)
		{
			event->accept();
			if (!isReadOnly())
				clear();
			return;
		}
		break;
#endif

	default:
		if (event == QKeySequence::SelectAll)
		{
			selectAll();
			event->accept();
			return;
		}
		break;
	}

	QLineEdit::keyPressEvent(event);
}

/*!
 * \brief Open a dialog to change the path in the line editor.
 * \param parent The parent windows of the dialog to open
 * \param defaultValue The value to propose, if there is no value in the editor.
 */
void DirectoryEdit::changePath(QWidget * parent, const QString & defaultValue)
{
	QString value = this->text();
	if (value.isEmpty()) value = defaultValue;

	if (d->m_directory)
		value = QFileDialog::getExistingDirectory(parent, tr("Change the path"), value);
	else if (d->m_fileMustExist)
		value = QFileDialog::getOpenFileName(parent, tr("Change the file"), value, d->m_filter, &d->m_filter);
	else
		value = QFileDialog::getSaveFileName(parent, tr("Change the file"), value, d->m_filter, &d->m_filter);

	if (! value.isEmpty())
	{
		this->setText(QDir::toNativeSeparators(value));
	}
}
