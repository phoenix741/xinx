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

#ifndef PROJECTPROPERTYIMPL_H
#define PROJECTPROPERTYIMPL_H

#include "ui_projectproperty.h"
#include "threadedconfigurationfile.h"

class XinxProject;
class IXinxPluginProjectConfiguration;

class ProjectPropertyImpl : public QDialog, public Ui::ProjectProperty {
	Q_OBJECT
public:
	ProjectPropertyImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	virtual ~ProjectPropertyImpl();

	void loadFromProject( XinxProject * );
	void saveToProject( XinxProject * );
private:
	void updateOkButton();
	ThreadedConfigurationFile * m_versionInstance;

	QList< QPair<IXinxPluginProjectConfiguration*,QWidget*> > m_pluginPages;
private slots:
	void on_m_logButton_clicked();
	void on_m_specifiqueGroupBox_clicked();
	void on_m_projectLineEdit_textChanged( QString );
	void on_m_projectButton_clicked();
	void on_m_prefixList_defaultValueChanged( QString );
	void on_m_addPreloadedFile_clicked();
	void on_m_preloadedFiles_itemSelectionChanged();

	void versionFinded( ConfigurationFile configuration );
};

#endif









