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
#pragma once

#include "ui_projectproperty.h"

class XinxProject;
class IXinxPluginProjectConfigurationPage;

class ProjectPropertyImpl : public QDialog, public Ui::ProjectProperty {
	Q_OBJECT
public:
	ProjectPropertyImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	virtual ~ProjectPropertyImpl();

	void loadFromProject( XinxProject * );
	void saveToProject( XinxProject * );
private:
	QList< IXinxPluginProjectConfigurationPage* > m_pluginPages;
	QHash< QString, QTreeWidgetItem* > m_pluginsCheck;
private slots:
	void on_m_tree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void updateOkButton();
	void on_m_addPreloadedFile_clicked();
	void on_m_preloadedFiles_itemSelectionChanged();
};

#endif









