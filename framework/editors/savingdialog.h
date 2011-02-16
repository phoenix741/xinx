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
#ifndef SAVINGDIALOG_H
#define SAVINGDIALOG_H

// Xinx header
#include "ui_savingdialog.h"

// Qt header
#include <QPushButton>

class AbstractEditor;

class SavingDialog : public QDialog, private Ui::SavingDialog
{
	Q_OBJECT
public:
	explicit SavingDialog(QWidget *parent = 0);

	void addEditor(AbstractEditor * editor);
	int countEditor();
	QList<AbstractEditor*> selectedEditor();
protected:
	void changeEvent(QEvent *e);

private slots:
	void buttonBoxClicked(QAbstractButton*);
private:

	QPushButton * m_selectAll;
	QPushButton * m_deselectAll;
};

#endif // SAVINGDIALOG_H
