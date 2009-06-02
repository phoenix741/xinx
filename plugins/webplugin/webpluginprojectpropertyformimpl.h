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

#ifndef _WEBPLUGINPROJECTPROPERTYFORMIMPL_H_
#define _WEBPLUGINPROJECTPROPERTYFORMIMPL_H_
#pragma once

// Xinx header
#include "ui_webpluginprojectpropertyform.h"

// Qt header
#include <QWizardPage>

class WebPluginProjectPropertyFormImpl : public QWidget, public Ui::WebPluginProjectPropertyForm {
	Q_OBJECT
public:
	WebPluginProjectPropertyFormImpl( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	virtual ~WebPluginProjectPropertyFormImpl();
private:
};

class WebPluginProjectPropertyWizard : public QWizardPage, public Ui::WebPluginProjectPropertyForm {
	Q_OBJECT
public:
	WebPluginProjectPropertyWizard( int nextId, QWidget * parent = 0 );

	int nextId() const;
private:
	int m_nextId;
};

#endif /* _WEBPLUGINPROJECTPROPERTYFORMIMPL_H_ */
