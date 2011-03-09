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
#ifndef __DIRECTORYEDITWIDGET_H__
#define __DIRECTORYEDITWIDGET_H__

#include <QWidget>

#include <components-config.h>

class PrivateDirectoryEditWidget;
class DirectoryEdit;

class COMPONENTSEXPORT DirectoryEditWidget : public QWidget
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

#endif // __DIRECTORYEDITWIDGET_H__
