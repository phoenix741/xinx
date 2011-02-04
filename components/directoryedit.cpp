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
#include "directoryedit.h"

// Qt header
#include <QFile>
#include <QPalette>
#include <QCompleter>
#include <QFileDialog>
#include <QToolButton>
#include <QHBoxLayout>
#include <QToolButton>
#include <QKeyEvent>

/* CompleterDirModel */

/*!
 * \ingroup Components
 * \class CompleterDirModel
 * \since 0.7.0.0
 *
 * \brief Class used by DirectoryEdit to complete on path.
 *
 * Completer model class based on a QDirModel which permit to propose a completion on
 * the path.
 * The CompleterDirModel convert path on native separtor. If the path end with a separator,
 * the separator is deleted.
 *
 * To use this modele, we can use a QCompleter on a QLineEdit.
 *
 * \code
 *   QLineEdit * lineEdit = new QLineEdit;
 *
 *   QCompleter * completer = new QCompleter(lineEdit);
 *   lineEdit->setCompleter(completer);
 *
 *   CompleterDirModel * model = new CompleterDirModel(completer);
 *   completer->setModel(model);
 * \endcode
 */

/*!
 * \brief Construct a CompleterDirModel.
 * \param parent The parent of the object.
 */
CompleterDirModel::CompleterDirModel(QObject *parent) : QDirModel(parent)
{
}

/*!
 * \brief Return the data to change in the completion dir model.
 *
 * This model show the good separator (with the \p QDir::toNativeSepartors() function).
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
DirectoryEdit::DirectoryEdit(QWidget * parent) : QLineEdit(parent), m_error(false), m_directory(true), m_fileMustExist(true), m_ignoreCursorPositionChanged(false)
{
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));
	connect(this, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(slotEditorCursorPositionChanged(int,int)));
	QCompleter * completer = new QCompleter(this);
	this->setCompleter(completer);
	completer->setModel(new CompleterDirModel(completer));
}

/*!
 * \brief Construct a DirectoryEdit object and change the content of the text at the creation.
 * \param contents Content of the Line edit.
 * \param parent The parent widget of the object.
 */
DirectoryEdit::DirectoryEdit(const QString & contents, QWidget * parent) : QLineEdit(contents, parent), m_error(false), m_directory(true), m_fileMustExist(true), m_ignoreCursorPositionChanged(false)
{
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));
	connect(this, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(slotEditorCursorPositionChanged(int,int)));
}

/*!
 * \brief Set to false, if the file can't not exist and you don't want to warn user
 * \param value true if you want the user
 * \sa fileMustExist
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
 * \brief Return true if you want to warn the user when the file doesn't exist.
 * \sa setFileMustExist
 */
bool DirectoryEdit::fileMustExist() const
{
	return m_fileMustExist;
}

/*!
 * \brief Set the filter to use to change load a file
 * \param filter The new filter to use
 * \sa filter
 */
void DirectoryEdit::setFilter(const QString & filter)
{
	m_filter = filter;
}

/*!
 * \brief Get the filter used to change the file
 * \sa setFilter
 */
const QString & DirectoryEdit::filter() const
{
	return m_filter;
}

/*!
 * \brief Set the limit from where the user can start editing
 * \param value The position of the first char that the user car edit.
 *
 * If the value is different of 0, the user can't edit first \e value characters.
 *
 * \sa editLimit();
 */
void DirectoryEdit::setPrefix(const QString & value)
{
	if (m_prefix != value)
	{
		m_prefix = value;
		setText(text());
	}
}

/*!
 * \brief Get the value of the edit limit
 * \sa setEditLimit()
 */
const QString & DirectoryEdit::prefix() const
{
	return m_prefix;
}

/*!
 * \brief This property indicate if the widget is used to edit directory or file
 * \sa setDirectory()
 */
bool DirectoryEdit::isDirectory() const
{
	return m_directory;
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
	if (m_directory != value)
	{
		m_directory = value;
		slotTextChanged(text());
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
	return m_error;
}

/*!
 * \reimp
 */
void DirectoryEdit::setText(const QString & value)
{
	if (value.startsWith (m_prefix))
	{
		QLineEdit::setText (value);
	}
	else
	{
		QLineEdit::setText (m_prefix + value);
	}
}

/*!
 * \reimp
 */
void DirectoryEdit::selectAll ()
{
	setSelection (m_prefix.size (), text ().size () - m_prefix.size ());
}

/*!
 * \reimp
 */
void DirectoryEdit::clear()
{
	setText (m_prefix);
	setCursorPosition (m_prefix.size ());
}

/*!
 * \reimp
 */
void DirectoryEdit::keyPressEvent(QKeyEvent *event)
{
	if (!event->text().isEmpty() && cursorPosition() < m_prefix.size())
		setCursorPosition(m_prefix.size());

	switch (event->key()) {
	case Qt::Key_End:
	case Qt::Key_Home:
		if (event->modifiers() & Qt::ShiftModifier) {
			int currentPos = cursorPosition();
			const QString text = displayText();
			if (event->key() == Qt::Key_End) {
				if (currentPos == 0 && !m_prefix.isEmpty()) {
					break; // let lineedit handle this
				} else {
					setSelection(currentPos, text.size() - currentPos);
				}
			} else {
				if (currentPos <= m_prefix.size()) {
					break; // let lineedit handle this
				} else {
					setSelection(currentPos, m_prefix.size() - currentPos);
				}
			}
			event->accept();
			return;
		}
		break;
	case Qt::Key_Backspace:
		if (cursorPosition() == m_prefix.size())
		{
			event->ignore();
			return;
		}
		break;
#ifdef Q_WS_X11 // only X11
	case Qt::Key_U:
		if (event->modifiers() & Qt::ControlModifier) {
			event->accept();
			if (!isReadOnly())
				clear();
			return;
		}
		break;
#endif

	default:
		if (event == QKeySequence::SelectAll) {
			selectAll();
			event->accept();
			return;
		}
		break;
	}

	QLineEdit::keyPressEvent (event);
}

/*!
 * \internal
 */
void DirectoryEdit::slotEditorCursorPositionChanged(int oldpos, int newpos)
{
	if (!hasSelectedText() && !m_ignoreCursorPositionChanged) {
		m_ignoreCursorPositionChanged = true;

		bool allowSelection = true;
		int pos = -1;
		if (newpos < m_prefix.size() && newpos != 0) {
			if (oldpos == 0) {
				allowSelection = false;
				pos = m_prefix.size();
			} else {
				pos = oldpos;
			}
		}

		if (pos != -1) {
			const int selSize = selectionStart() >= 0 && allowSelection
								? (selectedText().size() * (newpos < pos ? -1 : 1)) - newpos + pos
								: 0;

			const bool wasBlocked = blockSignals(true);
			if (selSize != 0) {
				setSelection(pos - selSize, selSize);
			} else {
				setCursorPosition(pos);
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
void DirectoryEdit::slotTextChanged(QString text)
{
	QFileInfo fileInfo(text);
	QFile file(text);
	QPalette palette(this->palette());

	m_error = (m_directory != fileInfo.isDir ()) || (m_fileMustExist && !file.exists ());

	if (!m_error)
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
 * \brief Open a dialog to change the path in the line editor.
 * \param parent The parent windows of the dialog to open
 * \param defaultValue The value to propose, if there is no value in the editor.
 */
void DirectoryEdit::changePath(QWidget * parent, const QString & defaultValue)
{
	QString value = this->text();
	if (value.isEmpty()) value = defaultValue;

	if (m_directory)
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
 * \since 0.7.0.0
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
	d->m_lineEdit->setDirectory (isDirectory);
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
	return d->m_lineEdit->isDirectory ();
}

/*!
 * \brief Change the directory/files goal of the widget.
 * \param value If \p true, the widget is used for edit directory, otherwise files.
 * \sa isDirectory()
 */
void DirectoryEditWidget::setDirectory(bool value)
{
	d->m_lineEdit->setDirectory (value);
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
