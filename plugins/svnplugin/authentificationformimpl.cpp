/*
 XINX
 Copyright (C) 2007-2010 by Ulrich Van Den Hekke
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

#include "authentificationformimpl.h"

AuthentificationFormImpl::AuthentificationFormImpl(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	setupUi(this);
}

AuthentificationFormImpl::~AuthentificationFormImpl()
{

}

void AuthentificationFormImpl::setRealm(const QString& realm)
{
	_repository_path->setText(realm);
}

QString AuthentificationFormImpl::realm() const
{
	return _repository_path->text();
}

void AuthentificationFormImpl::setUsername(const QString& username)
{
	_user_name_edit->setText(username);
}

QString AuthentificationFormImpl::username() const
{
	return _user_name_edit->text();
}

void AuthentificationFormImpl::setPassword(const QString& password)
{
	_password_edit->setText(password);
}

QString AuthentificationFormImpl::password() const
{
	return _password_edit->text();
}

void AuthentificationFormImpl::setIsPasswordSaved(bool save_password)
{
	_save_password_chk->setChecked(save_password);
}

bool AuthentificationFormImpl::isPasswordSaved() const
{
	return _save_password_chk->isChecked();
}
