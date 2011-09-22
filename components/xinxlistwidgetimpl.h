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
#ifndef XINXLISTWIDGET_H_
#define XINXLISTWIDGET_H_

// Xinx header
#include <components-config.h>

// Qt header
#include <QStringList>
#include <QWidget>

class XinxListWidgetImplPrivate;

class COMPONENTSEXPORT XinxListWidgetImpl : public QWidget
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("URL", "http://xinx.shadoware.org")
	Q_CLASSINFO("Licence", "GPL v2 or later")
	Q_PROPERTY(bool defaultVisible READ defaultVisible WRITE setDefaultVisible)
	Q_PROPERTY(QString defaultValue READ defaultValue WRITE setDefaultValue)
	Q_PROPERTY(QStringList values READ values WRITE setValues)
	Q_PROPERTY(QString defaultProposedValue READ defaultProposedValue WRITE setDefaultProposedValue)
public:
	XinxListWidgetImpl(QWidget * parent = 0, Qt::WindowFlags f = 0);
	virtual ~XinxListWidgetImpl();

	QString defaultValue() const;
	void setDefaultValue(const QString & value);

	bool defaultVisible() const;
	void setDefaultVisible(bool visible);

	QStringList values() const;
	void setValues(const QStringList & values);

	void add(const QString & value);
	void clear();

	QString defaultProposedValue() const;
	void setDefaultProposedValue(const QString & value);

	QString currentItem() const;
signals:
	void defaultValueChanged(QString value);
	void currentRowChanged(int value);
	void itemChanged(const QString & text);
	void itemAdded(const QString & value);
	void itemRemoved(const QString & value);
	void itemMoved(int orig, int dest);
protected:
	virtual QString addNewValue();
private:
	QScopedPointer<XinxListWidgetImplPrivate> d;
	friend class XinxListWidgetImplPrivate;
};

#endif /*XINXLISTXIDGET_H_*/
