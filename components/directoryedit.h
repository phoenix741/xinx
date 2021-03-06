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

#pragma once
#ifndef __DIRECTORYEDIT_H__
#define __DIRECTORYEDIT_H__

// Qt header
#include <QLineEdit>

#include <components-config.h>

class DirectoryEditPrivate;

class COMPONENTSEXPORT DirectoryEdit : public QLineEdit
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("URL", "http://xinx.shadoware.org")
	Q_CLASSINFO("Licence", "GPL v2 or later")
	Q_PROPERTY(bool fileMustExist READ fileMustExist WRITE setFileMustExist)
	Q_PROPERTY(bool directory READ isDirectory WRITE setDirectory)
	Q_PROPERTY(QString filter READ filter WRITE setFilter)
	Q_PROPERTY(QString prefix READ prefix WRITE setPrefix)
	Q_PROPERTY(bool error READ isError)
public:
	DirectoryEdit(QWidget * parent = 0);
	DirectoryEdit(const QString & contents, QWidget * parent = 0);
    virtual ~DirectoryEdit();

	bool fileMustExist() const;
	void setFileMustExist(bool value);

	bool isDirectory() const;
	void setDirectory(bool value);

	const QString & filter() const;
	void setFilter(const QString & filter);

	const QString & prefix() const;
	void setPrefix(const QString & value);

	bool isError() const;
protected:
	void keyPressEvent(QKeyEvent *event);
public slots:
	void setText(const QString & value);
	void selectAll();
	void clear();
	void changePath(QWidget * parent = NULL, const QString & defaultValue = QString());
private:
    QScopedPointer<DirectoryEditPrivate> d;
};

#endif // __DIRECTORYEDIT_H__
