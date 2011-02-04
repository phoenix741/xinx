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

#ifndef DERIVATIONDIALOGIMPL_H
#define DERIVATIONDIALOGIMPL_H
#pragma once

// Xinx header
#include "ui_derivationdialog.h"
#include <project/xinxprojectproject.h>

// Qt header
#include <QDialog>

/* DerivationDialogImpl */

class DerivationDialogImpl : public QDialog, Ui::DerivationDialog
{
	Q_OBJECT
public:
	 DerivationDialogImpl(XinxProject::Project * project, QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint);
	virtual ~DerivationDialogImpl();

	void load(const QString & filename, const QString & filter);
	QString getNewPath() const;

	static bool isDerivableFile(XinxProject::Project * project, const QString & filename);

private slots:
	void changePath();

private:
	QString m_filename, m_relativeFilename;
	 XinxProject::Project * _project;
};

#endif // DERIVATIONDIALOGIMPL_H
