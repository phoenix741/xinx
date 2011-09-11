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

class QShowEvent;

class ReplaceDialogImpl : public QDialog, public Ui::ReplaceDialog
{
	Q_OBJECT
public:
	ReplaceDialogImpl(QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint);

	enum SearchSelection {
		SELECT_SEARCH_NONE,
		SELECT_SEARCH_EDITOR,
		SELECT_SEARCH_FILES
	};

	void setEditorSearch(bool value);
	bool isEditorSearch() const;

	void setSelectedExtention(const QStringList& values);
	QStringList selectedExtention() const;

	void setSelection(SearchSelection value);
	SearchSelection selection();

	void setText(const QString & value);
	QString text() const;

	void setTextReplace(const QString & value);
	QString textReplace() const;

	void setReplace(bool value);
	bool isReplace() const;

	AbstractEditor::SearchOptions getOptions() const;
	QStringList getDirectories() const;
protected:
    virtual void showEvent(QShowEvent* event);
private slots:
	void m_findButton_clicked();
	void on_m_replaceCheckBox_toggled(bool checked);

	void on_m_extendButtonGroup_buttonClicked(QAbstractButton * button);
private:
	void updateDialog();

	bool _editor_search;
	SearchSelection _search_selection;

	QPushButton * m_findButton;
};
#endif






