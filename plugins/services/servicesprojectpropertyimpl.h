/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
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

#ifndef SERVICESPROJECTPROPERTYIMPL_H
#define SERVICESPROJECTPROPERTYIMPL_H
#pragma once

// Xinx header
#include "ui_servicesprojectproperty.h"

// Qt header
#include <QWidget>

class ServicesProjectPropertyImpl : public QWidget, public Ui::ServicesProjectProperty {
	Q_OBJECT
public:
	ServicesProjectPropertyImpl( QWidget* parent = 0, Qt::WFlags fl = 0 );
	~ServicesProjectPropertyImpl();
private slots:
	void wsdlSelectionChanged();

};

#endif // SERVICESPROJECTPROPERTYIMPL_H

