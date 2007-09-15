/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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

#ifndef PROJECTPROPERTYIMPL_H
#define PROJECTPROPERTYIMPL_H

#include "ui_projectproperty.h"
#include "threadedconfigurationfile.h"

class XSLProject;

class ProjectPropertyImpl : public QDialog, public Ui::ProjectProperty {
Q_OBJECT
public:
	ProjectPropertyImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	virtual ~ProjectPropertyImpl();
	
	void loadFromProject( XSLProject * );
	void saveToProject( XSLProject * );
private:
	void updateSpecifiquePath();
	void updateOkButton();
	ThreadedConfigurationFile * m_versionInstance;
	QString defaultSearchPath;
private slots:
	void on_m_servicesLineEdit_textChanged(QString );
	void on_m_searchPathBtnDef_clicked();
	void on_m_relativeLineEditPath_textChanged(QString );
	void on_m_standardProjectCheckBox_clicked();
	void on_m_searchPathBtnAdd_clicked();
	void on_m_searchPathBtnDel_clicked();
	void on_m_webServiceBtnDel_clicked();
	void on_m_webServiceBtnAdd_clicked();
	void on_m_langComboBox_currentIndexChanged( QString );
	void on_m_projectLineEdit_textChanged( QString );
	void on_m_projectButton_clicked();
	void on_m_prefixLineEdit_textChanged( QString );

	void versionFinded( ConfigurationVersion version );
};
#endif









