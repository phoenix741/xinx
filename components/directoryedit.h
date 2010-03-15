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

#ifndef __DIRECTORYEDIT_H__
#define __DIRECTORYEDIT_H__
#pragma once

// Qt header
#include <QLineEdit>
#include <QDirModel>

/*!
 * Completer model class based on a QDirModel which permit to propose a completion on
 * the path.
 */
class CompleterDirModel : public QDirModel
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("URL", "http://xinx.shadoware.org")
	Q_CLASSINFO("Licence", "GPL v2 or later")
public:
	/*!
	 * Construct a CompleterDirModel. The QDir model convert the slash on native separtor.
	 * If the path end with a separator, the separator is deleted.
	 * \param parent The parent of the object.
	 */
	CompleterDirModel(QObject *parent = 0);
	/*!
	 * Return the data to change in the completion dir model. This model show the good
	 * separator (with the \e QDir::toNativeSepartors() function).
	 * \param index The model index, represents a path in the tree.
	 * \param role Only the role Qt::DisplayRole is modified in this object.
	 */
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

/*!
 * A line editor using the QCompleter \e CompleterDirModel to simplify the editing of
 * the path.
 * If the path is wrong, he's writing in red.
 */
class DirectoryEdit : public QLineEdit
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("URL", "http://xinx.shadoware.org")
	Q_CLASSINFO("Licence", "GPL v2 or later")
	Q_PROPERTY(bool fileMustExist READ fileMustExist WRITE setFileMustExist)
public:
	/*!
	 * Construct a DirectoryEdit object.
	 * \param parent The parent widget of the object.
	 */
	DirectoryEdit(QWidget * parent = 0);
	/*!
	 * Construct a DirectoryEdit object and change the content of the text at the creation.
	 * \param contents Content of the Line edit.
	 * \param parent The parent widget of the object.
	 */
	DirectoryEdit(const QString & contents, QWidget * parent = 0);

	/*!
	 * Set to false, if the file can't not exist and you don't want to warn user
	 * \param value true if you want the user
	 * \sa warnIfExist
	 */
	void setFileMustExist(bool value);

	/*!
	 * Return true if you want to warn the user when the file doesn't exist.
	 * \sa setWarnIfExist
	 */
	bool fileMustExist() const;

	/*!
	 * Set the filter to use to change load a file
	 * \param filter The new filter to use
	 * \sa filter
	 */
	void setFilter(const QString & filter);
	/*!
	 * Get the filter used to change the file
	 * \sa setFilter
	 */
	const QString & filter() const;
public slots:
	/*!
	 * Open a dialog to change the path in the line editor.
	 * \param parent The parent windows of the dialog to open
	 * \param defaultValue The value to propose, if there is no value in the editor.
	 * \param directory If \e true, user must choose a directory, else a file
	 */
	void changePath(QWidget * parent = NULL, const QString & defaultValue = QString(), bool directory = true);
protected slots:
	/*!
	 * Slot called when the text changed. This method change the color of the text if the
	 * directory \e text not exist.
	 */
	void slotTextChanged(QString text);

private:
	bool m_fileMustExist;
	QString m_filter;
};

class PrivateDirectoryEditWidget;

/*!
 * This class is an association of a Directory Line editor and a button.
 * The button call the \e changePath slot from the \e DirectoryEdit class.
 */
class DirectoryEditWidget : public QWidget
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("URL", "http://xinx.shadoware.org")
	Q_CLASSINFO("Licence", "GPL v2 or later")
	Q_PROPERTY(bool directory READ isDirectory WRITE setDirectory)
	Q_PROPERTY(QString defaultValue READ defaultValue WRITE setDefaultValue)
public:
	/*!
	 * Construct the directory edit widget.
	 * \param parent The parent of the object.
	 */
	DirectoryEditWidget(QWidget * parent = 0);
	/*!
	 * Construct the directory edit widget and tell if the widget is used
	 * to edit directory or file.
	 * \param isDirectory If \e true, the widget edit directory, otherwise file.
	 * \param parent The parent of the widget.
	 */
	DirectoryEditWidget(bool isDirectory, QWidget * parent = 0);
	/*!
	 * Destroy the widget
	 */
	virtual ~DirectoryEditWidget();

	/*!
	 * Return the DirectoryEdit created by this widget.
	 */
	DirectoryEdit * lineEdit() const;

	/*!
	 * Return true if the widget is used to edit directory, or false if the widget is used
	 * to edit file.
	 */
	bool isDirectory() const;
	/*!
	 * Change the directory/files goal of the widget.
	 * \param value If \e true, the widget is used for edit directory, otherwise files.
	 */
	void setDirectory(bool value);

	/*!
	 * Return the default value used by the widget when the user click on the open button
	 */
	QString defaultValue() const;
	/*!
	 * Used to change the default value to use, when no text is in the editor.
	 * \param value The new default value to use when the user click on the button.
	 */
	void setDefaultValue(const QString & value);
public slots:
	/*!
	 * This slot is called to open a dialog to edit the new path.
	 */
	void changePath();
protected:
	virtual void focusInEvent(QFocusEvent * event);
private:
	PrivateDirectoryEditWidget * d;
	friend class PrivateDirectoryEditWidget;
};

#endif // __DIRECTORYEDIT_H__
