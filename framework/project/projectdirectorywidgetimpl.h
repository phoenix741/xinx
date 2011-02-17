/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PROJECTDIRECTORYWIDGETIMPL_H
#define PROJECTDIRECTORYWIDGETIMPL_H

#include "ui_projectdirectorywidget.h"
#include "xinxprojectproject.h"

class IFileTypePlugin;
class QDockWidget;
class PrivateProjectDirectoryWidgetImpl;

class ProjectDirectoryWidgetImpl : public QWidget, public Ui::ProjectDirectoryWidget
{
	Q_OBJECT
public:
	ProjectDirectoryWidgetImpl(QWidget* parent);
	virtual ~ProjectDirectoryWidgetImpl();

	void setDock(QDockWidget * dock);

	QAction * toggledViewAction() const;

	QStringList selectedFiles() const;
signals:
	void open(const QString & filename, IFileTypePlugin* interface, XinxProject::ProjectPtr project);
private:
	PrivateProjectDirectoryWidgetImpl * d;
	friend class PrivateProjectDirectoryWidgetImpl;
};

#endif // PROJECTDIRECTORYWIDGETIMPL_H