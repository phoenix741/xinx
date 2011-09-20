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

#ifndef ADDMODIFYLABELDIALOG_H
#define ADDMODIFYLABELDIALOG_H

#include <QDialog>

namespace Ui {
	class AddModifyLabelDialog;
}

class AddModifyLabelDialog : public QDialog
{
	Q_OBJECT
public:
	explicit AddModifyLabelDialog(QWidget *parent = 0);
	~AddModifyLabelDialog();

	void setCode(const QString & value);
	QString getCode() const;

	void setContext(const QString & value);
	QString getContext() const;

	void setLang(const QString &value);
	QString getLang() const;

	void setDescription(const QString & value);
	QString getDescription() const;
private:
	static QString localLangToGenerixLang(const QString & lang);
	Ui::AddModifyLabelDialog *ui;
};

#endif // ADDMODIFYLABELDIALOG_H
