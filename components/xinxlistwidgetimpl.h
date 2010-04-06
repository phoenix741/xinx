/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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
#ifndef XINXLISTWIDGET_H_
#define XINXLISTWIDGET_H_

// Xinx header
#include "ui_xinxlistwidget.h"

class XinxListWidgetImpl : public QWidget, public Ui::XinxListWidget
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

	QString defaultProposedValue() const;
	void setDefaultProposedValue(const QString & value);
	void add(const QString & value);
signals:
	void defaultValueChanged(QString value);
	void currentRowChanged(int value);
	void itemChanged(const QString & text);
private slots:
	void on_m_btnDef_clicked();
	void on_m_btnAdd_clicked();
	void on_m_btnDel_clicked();
	void on_m_btnUp_clicked();
	void on_m_btnDown_clicked();
	void on_m_list_currentRowChanged(int row);
	void on_m_list_itemChanged(QListWidgetItem * item);
private:
	void updateDefault(const QString & def);
	int m_defaultValue;
	QString m_defaultProposedValue;
};

#endif /*XINXLISTXIDGET_H_*/
