/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _SERVICESPROJECTWIZARD_H_
#define _SERVICESPROJECTWIZARD_H_

// Qt header
#include <QWizard>

// Xinx header
#include "ui_newprojectwizard_services.h"
#include "ui_newprojectwizard_serviceslist.h"

class ServicesPageImpl : public QWizardPage, public Ui::ServicesPage {
	Q_OBJECT
public:
	ServicesPageImpl( int nextId, QWidget * parent = 0 );

	int nextId() const;
private:
	int m_nextId;
};

class ServicesListPageImpl : public QWizardPage, public Ui::ServicesListPage {
	Q_OBJECT
public:
	ServicesListPageImpl( int nextId, QWidget * parent = 0 );

	int nextId() const;
	QVariant field(const QString &name) const;
private:
	int m_nextId;
};

#endif // _SERVICESPROJECTWIZARD_H_
