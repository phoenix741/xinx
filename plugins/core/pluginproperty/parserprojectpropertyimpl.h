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
#include "ui_parserprojectproperty.h"
#include <plugins/interfaces/gui.h>

// Qt header
#include <QWizardPage>

class ParserProjectPropertyImpl : public QWidget, public Ui::ParserProjectProperty, public IXinxPluginProjectConfigurationPage
{
	Q_OBJECT
public:
	ParserProjectPropertyImpl(QWidget * parent = 0, Qt::WindowFlags f = 0);
	virtual ~ParserProjectPropertyImpl();

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

class WebPluginProjectPropertyWizard : public IXinxPluginNewProjectConfigurationPage
{
	Q_OBJECT
public:
	WebPluginProjectPropertyWizard();

	virtual void initializePage();
	virtual bool saveSettingsDialog(XinxProject::ProjectPtr project);

	virtual QString pagePluginId() const;
	virtual bool pageIsVisible() const;
private:
	QLineEdit * m_urlLocationEdit;
	DirectoryEditWidget * m_dataStreamEdit;
};

#endif /* _WEBPLUGINPROJECTPROPERTYFORMIMPL_H_ */
