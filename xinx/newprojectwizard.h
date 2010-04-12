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

#ifndef _NEWPROJECTWIZARD_H_
#define _NEWPROJECTWIZARD_H_
#pragma once

// Qt header
#include <QWizard>
#include <QRadioButton>

// Xinx header
#include "ui_newprojectwizard_project.h"
#include <plugins/plugininterfaces.h>

class NewProjectWizard;
class TemplateDialogImpl;
class XinxProject;

class ProjectPageImpl : public IXinxPluginNewProjectConfigurationPage, private Ui::ProjectPage
{
	Q_OBJECT
public:
	ProjectPageImpl();

	virtual void setVisible(bool visible);
	virtual bool isComplete() const;

	virtual QString pagePluginId() const;
	virtual bool pageIsVisible() const;
	virtual bool saveSettingsDialog(XinxProject * project);
private slots:
	void on_m_projectPathBtn_clicked();

	void on_m_projectNameEdit_textChanged(const QString & value);
};

class VersionsPageImpl : public IXinxPluginNewProjectConfigurationPage
{
	Q_OBJECT
public:
	VersionsPageImpl();

	virtual QString pagePluginId() const;
	virtual bool pageIsVisible() const;
	virtual bool saveSettingsDialog(XinxProject * project);
private:
	QRadioButton * m_noRevisionControl;
	QList< QPair<QRadioButton*, QString> > m_revisionBtn;

	friend class NewProjectWizard;
};

class LastPageImpl : public IXinxPluginNewProjectConfigurationPage
{
	Q_OBJECT
public:
	LastPageImpl();

	virtual QString pagePluginId() const;
	virtual bool pageIsVisible() const;
	virtual bool saveSettingsDialog(XinxProject * project);
private:
};

class NewProjectWizard : public QWizard
{
	Q_OBJECT
public:
	NewProjectWizard(QWidget * widget = 0, Qt::WFlags f = 0);

	XinxProject * createProject();

	virtual int nextId() const;
private slots:
	void on_customButton1_clicked();
private:
	XinxProject * m_project;

	mutable QHash<int,QString> m_nextId;
	mutable QStringList m_plugins;

	LastPageImpl * m_lastDialog;
	TemplateDialogImpl * m_templateDialog;
	ProjectPageImpl * m_projectPage;
	QHash<QString,IXinxPluginNewProjectConfigurationPage*> m_wizardPages;
};



#endif
