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

#ifndef _GENERIXDERIVATIONOPTIONSPAGE_H_
#define _GENERIXDERIVATIONOPTIONSPAGE_H_
#pragma once

// Xinx header
#include "ui_generixderivationoptionspage.h"
#include <plugins/interfaces/gui.h>

class GenerixDerivationOptionsPageImpl : public QWidget, public Ui::GenerixDerivationOptionsPage, public IXinxPluginProjectConfigurationPage
{
	Q_OBJECT
public:
	GenerixDerivationOptionsPageImpl(QWidget * parent = 0, Qt::WFlags f = 0);
	virtual ~GenerixDerivationOptionsPageImpl();

	virtual void setProject(XinxProject::ProjectPtr project);

	virtual QPixmap image();
	virtual QString name();

	virtual bool loadSettingsDialog();
	virtual bool saveSettingsDialog();
	virtual bool cancelSettingsDialog();
	virtual QWidget * settingsDialog();
	virtual bool isSettingsValid(QString & message);
	virtual bool isVisible();
private:
	XinxProject::ProjectPtr m_project;
};

#endif // _GENERIXDERIVATIONOPTIONSPAGE_H_

