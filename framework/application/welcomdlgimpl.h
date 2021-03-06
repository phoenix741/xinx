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

#ifndef WELCOMDLGIMPL_H
#define WELCOMDLGIMPL_H

// Xinx header
#include "ui_welcomdlg.h"

// Qt header
#include <QDialog>

/* WelcomDialogImpl */

class WelcomDialogImpl : public QDialog, public Ui::WelcomDialog
{
	Q_OBJECT
public:
	WelcomDialogImpl(QWidget * parent = 0, Qt::WindowFlags f = Qt::MSWindowsFixedSizeDialogHint);
	virtual ~WelcomDialogImpl();

	void addProjectFile(const QString & filename);
	void addSession(const QString & sessionName);
public slots:
	void slotUrlClicked(const QString &data);
	void slotProjectClicked(const QString &data);
	void slotProjectRemovedClicked(const QString &data);
	void slotSessionClicked(const QString &data);
	void slotSessionRemovedClicked(const QString &data);
signals:
	void requestProject(const QString & data);
	void removeProject(const QString & data);
	void requestSession(const QString & data);
	void removeSession(const QString & data);
	void createNewProject();
	void openExistingProject();
private:
	//void createWebsiteList();
	void updateTipOfTheDay();
};

#endif // WELCOMDLGIMPL_H
