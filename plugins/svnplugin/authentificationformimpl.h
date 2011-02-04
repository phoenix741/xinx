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

#ifndef AUTHENTIFICATIONFORMIMPL_H
#define AUTHENTIFICATIONFORMIMPL_H

#include <QDialog>

#include "ui_authentification_form.h"

class AuthentificationFormImpl : public QDialog, public Ui::AuthentificationForm
{
	Q_OBJECT
public:
    AuthentificationFormImpl(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~AuthentificationFormImpl();

	void setRealm(const QString & realm);
	QString realm() const;

	void setUsername(const QString & username);
	QString username() const;

	void setPassword(const QString & password);
	QString password() const;

	void setIsPasswordSaved(bool save_password);
	bool isPasswordSaved() const;
};

#endif // AUTHENTIFICATIONFORMIMPL_H
