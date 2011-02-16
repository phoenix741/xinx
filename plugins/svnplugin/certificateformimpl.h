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

#ifndef CERTIFICATEFORMIMPL_H
#define CERTIFICATEFORMIMPL_H

#include <QDialog>

#include "ui_invalide_certificate.h"
#include <svncpp/context_listener.hpp>

class CertificateFormImpl : public QDialog, Ui::CertificateForm
{
	Q_OBJECT
public:
	CertificateFormImpl(QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~CertificateFormImpl();


	void setData(const svn::ContextListener::SslServerTrustData & data);
	const svn::ContextListener::SslServerTrustData & data() const;

	svn::ContextListener::SslServerTrustAnswer answer() const;
private:
	svn::ContextListener::SslServerTrustData _data;
	svn::ContextListener::SslServerTrustAnswer _answer;
};

#endif // CERTIFICATEFORMIMPL_H
