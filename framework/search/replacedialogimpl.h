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

#ifndef REPLACEDIALOGIMPL_H
#define REPLACEDIALOGIMPL_H
#pragma once

// Xinx header
#include "ui_replace.h"
#include <editors/abstracteditor.h>

// Qt header
#include <QPushButton>

class ReplaceDialogImpl : public QDialog, public Ui::ReplaceDialog
{
	Q_OBJECT
public:
	ReplaceDialogImpl(QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint);

	void initialize(bool hasEditor = true);
	void setText(const QString &);
	void setReplace(bool);

signals:
	void find(const QString & from, const QString & to, const AbstractEditor::SearchOptions & options);
	void findInFiles(const QStringList & directories, const QStringList & filter, const QString & from, const QString & to, const AbstractEditor::SearchOptions & options);

private slots:
	void m_findButton_clicked();
	void on_m_replaceCheckBox_toggled(bool checked);

	void on_m_extedButtonGroup_buttonClicked(QAbstractButton * button);
private:

	QPushButton * m_findButton;
};
#endif






