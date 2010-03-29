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

#pragma once
#ifndef __DIRECTORYEDIT_H__
#define __DIRECTORYEDIT_H__

// Qt header
#include <QLineEdit>
#include <QDirModel>

class CompleterDirModel : public QDirModel
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("URL", "http://xinx.shadoware.org")
	Q_CLASSINFO("Licence", "GPL v2 or later")
public:
	CompleterDirModel(QObject *parent = 0);
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

class DirectoryEdit : public QLineEdit
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("URL", "http://xinx.shadoware.org")
	Q_CLASSINFO("Licence", "GPL v2 or later")
	Q_PROPERTY(bool fileMustExist READ fileMustExist WRITE setFileMustExist)
public:
	DirectoryEdit(QWidget * parent = 0);
	DirectoryEdit(const QString & contents, QWidget * parent = 0);

	void setFileMustExist(bool value);
	bool fileMustExist() const;

	void setFilter(const QString & filter);
	const QString & filter() const;
public slots:
	void changePath(QWidget * parent = NULL, const QString & defaultValue = QString(), bool directory = true);
protected slots:
	void slotTextChanged(QString text);

private:
	bool m_fileMustExist;
	QString m_filter;
};

class PrivateDirectoryEditWidget;

class DirectoryEditWidget : public QWidget
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("URL", "http://xinx.shadoware.org")
	Q_CLASSINFO("Licence", "GPL v2 or later")
	Q_PROPERTY(bool directory READ isDirectory WRITE setDirectory)
	Q_PROPERTY(QString defaultValue READ defaultValue WRITE setDefaultValue)
public:
	DirectoryEditWidget(QWidget * parent = 0);
	DirectoryEditWidget(bool isDirectory, QWidget * parent = 0);
	virtual ~DirectoryEditWidget();

	DirectoryEdit * lineEdit() const;

	bool isDirectory() const;
	void setDirectory(bool value);

	QString defaultValue() const;
	void setDefaultValue(const QString & value);
public slots:
	void changePath();
protected:
	virtual void focusInEvent(QFocusEvent * event);
private:
	PrivateDirectoryEditWidget * d;
	friend class PrivateDirectoryEditWidget;
};

#endif // __DIRECTORYEDIT_H__
