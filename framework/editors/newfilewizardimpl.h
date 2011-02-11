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

#ifndef NEWFILEWIZARDIMPL_H
#define NEWFILEWIZARDIMPL_H

#include "ui_newfilewizard.h"
#include <QWizard>
#include <project/xinxprojectproject.h>

/*!
 * \internal
 */
class NewFileWizardImpl : public QWizard, private Ui::NewFileWizard
{
	Q_OBJECT
public:
	NewFileWizardImpl(QWidget* parent = 0, Qt::WindowFlags flags = 0);
	virtual ~NewFileWizardImpl();

	void setPath(const QString & value);
	QString path() const;

	void setFileName(const QString & value);
	QString filename() const;

	void setProject(XinxProject::ProjectPtr project);
	XinxProject::ProjectPtr project() const;

	void setSelectedType(IFileTypePlugin * plugin);
	IFileTypePlugin * selectedType() const;
protected:
	virtual void initializePage(int id);
	virtual void done ( int result );
private slots:
	void slotTextChanged (const QString & text);
private:
	XinxProject::ProjectPtr _project;
	IFileTypePlugin * _interface;
};

#endif // NEWFILEWIZARDIMPL_H
