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

#define WEBSERVICE_VERSION_0 0
#define WEBSERVICE_VERSION_1 1
#define WEBSERVICE_VERSION_CURRENT 1

// Xinx header
#include "ui_servicesprojectproperty.h"
#include <plugins/interfaces/gui.h>

// Qt header
#include <QWidget>
#include <QHttp>
#include <QProgressDialog>

class ServicesProjectPropertyImpl : public QWidget, public Ui::ServicesProjectProperty, public IXinxPluginProjectConfigurationPage
{
	Q_OBJECT
public:
	ServicesProjectPropertyImpl(QWidget* parent = 0, Qt::WFlags fl = 0);
	~ServicesProjectPropertyImpl();

	virtual QPixmap image();
	virtual QString name();

	virtual void setProject(XinxProject::ProjectPtr project);

	virtual bool loadSettingsDialog();
	virtual bool saveSettingsDialog();
	virtual bool cancelSettingsDialog();
	virtual QWidget * settingsDialog();
	virtual bool isSettingsValid(QString & message);
	virtual bool isVisible();

private slots:
	void wsdlSelectionChanged();
	void on_m_refreshWSDL_clicked();
	void on_m_adressLineEdit_editingFinished();
	void on_m_wsdlEdit_textChanged();

	void httpDataReadProgress(int done, int total);
	void httpRequestFinished(bool error);
private:
	QHash<QString,QString> m_wsdlContent;

	QHttp * m_http;
	QProgressDialog * m_httpProgressDialog;
	QString m_url;
	XinxProject::ProjectPtr m_project;
};

#endif // SERVICESPROJECTPROPERTYIMPL_H

